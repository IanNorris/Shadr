#include <stdio.h>
#include <stdlib.h>


#include "Tokens.h"
#include "Error.h"
#include "Utility.h"
#include "LLVM.h"
#include "AST/AST.h"

#include "UnitTesting.h"

CASTExpression* ParsePrimary( SParseContext& rtContext );

CASTExpression* ParseParenthesisExpression( SParseContext& rtContext )
{
	return NULL;
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

void ParseFile( const char* pszFilename, CModule* pModule )
{
	std::string tFile;
	ReadFile( tFile, pszFilename );

	const char* pszBuffer = &tFile[0];
	unsigned int uBufferSize = tFile.length();
	unsigned int uCurrentRow = 0;
	unsigned int uCurrentCol = 0;

	SParseContext tContext( pszBuffer, pModule );

	while( ConsumeToken( tContext ) )
	{
		CASTExpression* pExpression = ParseExpression( tContext );
		llvm::Value* pValue = pExpression->GenerateCode();
		pValue;
	}
}

int main( int iArgCount, char** apszArguments )
{
	InitialiseTokenTables();
	InitialiseLLVM();
	
	CModule tModule( "Shader" );

	RunUnitTests();
	
	if( iArgCount > 1 )
	{
		ParseFile( apszArguments[1], &tModule );
	}

	return 0;
}