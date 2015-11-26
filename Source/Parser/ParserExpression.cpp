#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"

#include <regex>

std::regex g_tSwizzleRegex( "(?:(?:[argb][argb]?[argb]?[argb]?)|(?:[xyzw][xyzw]?[xyzw]?[xyzw]?)|)" );

bool IsSwizzle( const std::string& rtIdentifier )
{
	std::smatch matches;
	std::regex_match( rtIdentifier, matches, g_tSwizzleRegex );

	return matches.size() > 0;
}

CASTExpression* ParseParenthesisExpression( SParseContext& rtContext, CScope* pParentScope )
{
	if( !ConsumeToken( rtContext ) )
	{
		ParserError( rtContext, "Expected expression" );
	}

	CASTExpression* pSubExpression = ParseExpression( rtContext, pParentScope );

	if( rtContext.sNextToken.eToken != EShaderToken_Parenthesis_Close )
	{
		ParserError( rtContext, "Mismatched (, expected )" );
	}

	return new CASTExpressionParen( pSubExpression );
}

CASTExpression* ParseBinaryExpressionRight( SParseContext& rtContext, bool bRTL, int iLTRPrecedence, int iRTLPrecedence, CASTExpression* pLeft, CScope* pParentScope )
{
	bool bRejectComma = rtContext.IsFlagSet( EParseFlag_RejectComma );

	while( true )
	{
		if( rtContext.sNextToken.eToken == EShaderToken_Comma && bRejectComma )
		{
			return pLeft;
		}

		//Reject a colon, we will explicitly skip it
		if( rtContext.sNextToken.eToken == EShaderToken_Colon )
		{
			return pLeft;
		}

		EOperatorType eType;
		int iRightPrecedence = GetOperatorPrecedence( eType, rtContext.sNextToken.eToken, false ).iPrecedence;

		//Not an operator, 
		if( iRightPrecedence == -1 )
		{

		}

		//Left to Right Precedence
		if( iRightPrecedence < iLTRPrecedence )
		{
			return pLeft;
		}

		//Right to Left Precedence
		if( iRTLPrecedence <= iRightPrecedence )
		{
			return pLeft;
		}

		EShaderToken eOperator = rtContext.sNextToken.eToken;
		if( !ConsumeToken( rtContext ) )
		{
			ParserError( rtContext, "Unexpected end of expression." );
		}

		CASTExpression* pRight = nullptr;

		if( eType == EOperatorType_Unary )
		{
			pLeft = new CASTExpressionUnary( eOperator, false, pLeft );
		}
		else if( eType == EOperatorType_Ternary )
		{
			CASTExpression* pExpressionTrue = ParseExpression( rtContext, pParentScope );

			if( rtContext.sNextToken.eToken != EShaderToken_Colon )
			{
				ParserError( rtContext, "Expected colon for ternary operator." );

				return nullptr;
			}

			if( !ConsumeToken( rtContext ) )
			{
				ParserError( rtContext, "Unexpected end of file." );
				return nullptr;
			}

			EOperatorType eTypeTernary;
			int iTernaryPrecedence = GetOperatorPrecedence( eTypeTernary, EShaderToken_Ternary_QMark, false ).iPrecedence;

			CASTExpression* pExpressionFalse = ParseExpression( rtContext, pParentScope );

			pLeft = new CASTExpressionTernary( pLeft, pExpressionTrue, pExpressionFalse );
		}
		else
		{
			pRight = ParsePrimary( rtContext, eOperator, pParentScope );
			if( !pRight )
			{
			

				if( !pRight )
				{
					ParserError( rtContext, "Expected expression." );
					return nullptr;
				}
			}

			int iNextPrecedence = GetOperatorPrecedence( eType, rtContext.sNextToken.eToken, false ).iPrecedence;
			if( iRightPrecedence < iNextPrecedence )
			{
				pRight = ParseBinaryExpressionRight( rtContext, bRTL, iRightPrecedence + 1, iRTLPrecedence, pRight, pParentScope );
				if( !pRight )
				{
					ParserError( rtContext, "Expected expression." );
					return nullptr;
				}
			}

			pLeft = new CASTExpressionBinary( eOperator, pLeft, pRight );
		}
	}
}

CASTExpression* ParsePrimary( SParseContext& rtContext, EShaderToken eToken, CScope* pParentScope )
{
	CASTExpression* pResult = nullptr;

	bool bConsumedToken = false;

	EOperatorType eType;
	int iPrecedence = GetOperatorPrecedence( eType, rtContext.sNextToken.eToken, true ).iPrecedence;

	if( eType == EOperatorType_Unary )
	{
		EShaderToken eUnaryToken = rtContext.sNextToken.eToken;

		ConsumeToken( rtContext );

		CASTExpression* pChild = ParsePrimary( rtContext, EShaderToken_Invalid, pParentScope );

		if( pChild == nullptr )
		{
			ParserError( rtContext, "Expected expression." );
			return nullptr;
		}

		pResult = new CASTExpressionUnary( eUnaryToken, true, pChild );
	}
	else
	{
		switch( rtContext.sNextToken.eToken )
		{
			case EShaderToken_Parenthesis_Open:
				pResult = ParseParenthesisExpression( rtContext, pParentScope );
				break;

			case EShaderToken_Float:
				pResult = new CASTConstantFloat( rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength );
				break;

			case EShaderToken_Int:
				pResult = new CASTConstantInt( rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength );
				break;

			case EShaderToken_Boolean:
				pResult = new CASTConstantBool( rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength );
				break;

			case EShaderToken_Identifier:
				{
					std::string tIdentifierName( rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength );

					ConsumeToken( rtContext );
					bConsumedToken = true;

					//If the parent operator is the dot operator, we're looking for a swizzle or member identifier name
					if( eToken == EShaderToken_Dot )
					{
						if( IsSwizzle( tIdentifierName ) )
						{
							pResult = new CASTExpressionSwizzleMask( tIdentifierName );
						}
						else
						{
							pResult = new CASTExpressionMemberAccess( tIdentifierName );
						}
					}
					else
					{
						if( rtContext.sNextToken.eToken == EShaderToken_Parenthesis_Open )
						{
							pResult = ParseFunctionCall( rtContext, tIdentifierName, pParentScope );
						}
						else
						{	
							SVariable* pVariable = pParentScope->FindVariable( tIdentifierName );

							if( !pVariable )
							{
								CType* pFindType = GetType( tIdentifierName );
								if( pFindType )
								{
									return nullptr;
								}

								ParserError( rtContext, "Undeclared identifier '%s'.", tIdentifierName.c_str() );
								SVariable* pDummy = SVariable::CreateDummyVariable();
								pDummy->tName = tIdentifierName;
								pParentScope->AddVariable( rtContext, pDummy );
								pResult = new CASTVariableReference( pDummy );
							}
							else
							{
								pResult = new CASTVariableReference( pVariable );
							}
						}
					}

					break;
				}
		}

		if( !bConsumedToken )
		{
			ConsumeToken( rtContext );
		}
	}

	return pResult;
}

CASTExpression* ParseExpression( SParseContext& rtContext, CScope* pParentScope, int iRTLPrecedence, bool bRTL )
{
	CASTExpression* pLeft = ParsePrimary( rtContext, EShaderToken_Invalid, pParentScope );
	if( !pLeft )
	{
		return nullptr;
	}

	return ParseBinaryExpressionRight( rtContext, 0, bRTL, iRTLPrecedence, pLeft, pParentScope );
}

CASTExpression* ParseFunctionCall( SParseContext& rtContext, const std::string& rtFunctionName, CScope* pParentScope )
{
	CASTExpressionFunctionCall* pCall = new CASTExpressionFunctionCall( rtFunctionName );

	bool bFirst = true;

	if( !ConsumeToken( rtContext ) )
	{
		ParserError( rtContext, "Unexpected end of file" );
	}

	while( true )
	{
		if( rtContext.sNextToken.eToken == EShaderToken_Parenthesis_Close )
		{
			break;
		}

		if( !bFirst )
		{
			if( rtContext.sNextToken.eToken == EShaderToken_Comma )
			{
				if( !ConsumeToken( rtContext ) )
				{
					ParserError( rtContext, "Unexpected end of file" );
				}
			}
			else
			{
				ParserError( rtContext, "Expected comma." );
			}
		}
		bFirst = false;

		CASTExpression* pExpression = ParseExpression( rtContext, pParentScope );
		if( pExpression )
		{
			pCall->AddParameter( pExpression );
		}
		else
		{
			ParserError( rtContext, "Expected expression." );
			break;
		}
	}

	if( !ConsumeToken( rtContext ) )
	{
		ParserError( rtContext, "Unexpected end of file" );
	}

	return pCall;
}