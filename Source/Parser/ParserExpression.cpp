#include "LLVM.h"
#include "AST/AST.h"
#include "Parser.h"
#include "Utility/Error.h"

CASTExpression* ParseParenthesisExpression( SParseContext& rtContext )
{
	if( !ConsumeToken( rtContext ) )
	{
		Error_Compiler( EError_Error, rtContext.uCurrentRow, rtContext.uCurrentCol, "Expected expression" );
	}

	CASTExpression* pSubExpression = ParseExpression( rtContext );

	if( rtContext.sNextToken.eToken != EShaderToken_Parenthesis_Close )
	{
		Error_Compiler( EError_Error, rtContext.uCurrentRow, rtContext.uCurrentCol, "Mismatched (, expected )" );
	}

	return pSubExpression;
}

CASTExpression* ParseBinaryExpressionRight( SParseContext& rtContext, int iLeftPrecedence, CASTExpression* pLeft )
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
			Error_Compiler( EError_Error, rtContext.uCurrentRow, rtContext.uCurrentCol, "Unexpected end of expression." );
		}

		CASTExpression* pRight = ParsePrimary( rtContext );
		if( !pRight )
		{
			Error_Compiler( EError_Debug, rtContext.uCurrentRow, rtContext.uCurrentCol, "Expected expression." );
			return nullptr;
		}

		int iNextPrecedence = GetOperatorPrecedence( EOperatorType_Binary, rtContext.sNextToken.eToken ).iPrecedence;
		if( iRightPrecedence < iNextPrecedence )
		{
			pRight = ParseBinaryExpressionRight( rtContext, iRightPrecedence + 1, pRight );
			if( !pRight )
			{
				Error_Compiler( EError_Debug, rtContext.uCurrentRow, rtContext.uCurrentCol, "Expected expression." );
				return nullptr;
			}
		}

		pLeft = new CASTExpressionBinary( eBinaryOperator, pLeft, pRight );
	}
}

CASTExpression* ParsePrimary( SParseContext& rtContext )
{
	CASTExpression* pResult = nullptr;

	switch( rtContext.sNextToken.eToken )
	{
		case EShaderToken_Parenthesis_Open:
			pResult = ParseParenthesisExpression( rtContext );
			break;

		case EShaderToken_Float:
			pResult = new CASTConstantFloat( rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength );
			break;

		case EShaderToken_Int:
			pResult = new CASTConstantInt( rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength );
			break;

		case EShaderToken_Identifier:
			pResult = NULL; //TODO
			break;
	}

	ConsumeToken( rtContext );

	return pResult;
}

CASTExpression* ParseExpression( SParseContext& rtContext )
{
	CASTExpression* pLeft = ParsePrimary( rtContext );
	if( !pLeft )
	{
		Error_Compiler( EError_Debug, rtContext.uCurrentRow, rtContext.uCurrentCol, "Expected expression." );
		return nullptr;
	}

	return ParseBinaryExpressionRight( rtContext, 0, pLeft );
}
