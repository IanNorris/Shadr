#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"

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
	while( true )
	{
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

		CASTExpression* pRight = ParsePrimary( rtContext, pParentScope );
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

CASTExpression* ParsePrimary( SParseContext& rtContext, CScope* pParentScope )
{
	CASTExpression* pResult = nullptr;

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

		case EShaderToken_Identifier:
			{
				std::string tIdentifierName( rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength );
		
				SVariable* pVariable = pParentScope->FindVariable( tIdentifierName );

				if( !pVariable )
				{
					ParserError( rtContext, "Variable '%s' undeclared.", tIdentifierName.c_str() );
					pResult = nullptr;
				}
				else
				{
					pResult = new CASTVariableReference( pVariable );
				}

				break;
			}
	}

	ConsumeToken( rtContext );

	return pResult;
}

CASTExpression* ParseExpression( SParseContext& rtContext, CScope* pParentScope )
{
	CASTExpression* pLeft = ParsePrimary( rtContext, pParentScope );
	if( !pLeft )
	{
		ParserError( rtContext, "Expected expression." );
		return nullptr;
	}

	return ParseBinaryExpressionRight( rtContext, 0, pLeft, pParentScope );
}
