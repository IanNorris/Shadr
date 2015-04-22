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

	return pSubExpression;
}

CASTExpression* ParseBinaryExpressionRight( SParseContext& rtContext, int iLeftPrecedence, CASTExpression* pLeft, CScope* pParentScope )
{
	bool bRejectComma = rtContext.IsFlagSet( EParseFlag_RejectComma );

	while( true )
	{
		if( rtContext.sNextToken.eToken == EShaderToken_Comma && bRejectComma )
		{
			return pLeft;
		}

		int iRightPrecedence = GetOperatorPrecedence( EOperatorType_Binary, rtContext.sNextToken.eToken ).iPrecedence;

		if( iRightPrecedence < iLeftPrecedence )
		{
			return pLeft;
		}

		EShaderToken eBinaryOperator = rtContext.sNextToken.eToken;
		if( !ConsumeToken( rtContext ) )
		{
			ParserError( rtContext, "Unexpected end of expression." );
		}

		CASTExpression* pRight = ParsePrimary( rtContext, eBinaryOperator, pParentScope );
		if( !pRight )
		{
			ParserError( rtContext, "Expected expression." );
			return nullptr;
		}

		int iNextPrecedence = GetOperatorPrecedence( EOperatorType_Binary, rtContext.sNextToken.eToken ).iPrecedence;
		if( iRightPrecedence < iNextPrecedence )
		{
			pRight = ParseBinaryExpressionRight( rtContext, iRightPrecedence + 1, pRight, pParentScope );
			if( !pRight )
			{
				ParserError( rtContext, "Expected expression." );
				return nullptr;
			}
		}

		pLeft = new CASTExpressionBinary( eBinaryOperator, pLeft, pRight );
	}
}

CASTExpression* ParsePrimary( SParseContext& rtContext, EShaderToken eBinaryToken, CScope* pParentScope )
{
	CASTExpression* pResult = nullptr;

	bool bConsumedToken = false;

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
				if( eBinaryToken == EShaderToken_Dot )
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

	return pResult;
}

CASTExpression* ParseExpression( SParseContext& rtContext, CScope* pParentScope )
{
	CASTExpression* pLeft = ParsePrimary( rtContext, EShaderToken_Invalid, pParentScope );
	if( !pLeft )
	{
		return nullptr;
	}

	return ParseBinaryExpressionRight( rtContext, 0, pLeft, pParentScope );
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