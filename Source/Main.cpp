#include <stdio.h>
#include <stdlib.h>


#include "Tokens.h"
#include "Error.h"
#include "Utility.h"
#include "LLVM.h"
#include "AST/AST.h"

#include "UnitTesting.h"

#include <iostream>

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

void ParseBuffer( const char* pszFilename, const std::string& tBuffer, CModule* pModule )
{
	unsigned int uBufferSize = tBuffer.length();
	unsigned int uCurrentRow = 0;
	unsigned int uCurrentCol = 0;

	const char* pszBuffer = &tBuffer[0];

	SParseContext tContext( pszBuffer, pModule );

	while( ConsumeToken( tContext ) )
	{
		CASTExpression* pExpression = ParseExpression( tContext );

		CASTPrototype* pPrototype = new CASTPrototype( "hello", 5, CType(EScalarType_Double) );
		CASTFunction* pFunction = new CASTFunction( pPrototype, pExpression );

		llvm::Function* pLLVMFunction = (llvm::Function*)pFunction->GenerateCode( pModule );

		pModule->GetExecutionEngine()->finalizeObject();


		void* pRawFP = pModule->GetExecutionEngine()->getPointerToFunction( pLLVMFunction );

		double(*pRawFPCasted)() = (double(*)())(intptr_t)pRawFP;

		double fReturnVal = pRawFPCasted();

		printf( "%.3f\n", fReturnVal );
	}

	pModule->GetModule()->dump();
}

void ParseFile( const char* pszFilename, CModule* pModule )
{
	std::string tFile;
	ReadFile( tFile, pszFilename );

	ParseBuffer( pszFilename, tFile, pModule );
}

int main( int iArgCount, char** apszArguments )
{
	InitialiseTokenTables();
	InitialiseLLVM();
	
	CModule tModule( "Shader" );

	RunUnitTests();
	
	/*if( iArgCount > 1 )
	{
		ParseFile( apszArguments[1], &tModule );
	}*/

	/*std::string tExpression;
	while( true )
	{
		std::getline (std::cin, tExpression);

		ParseBuffer( "test", tExpression, &tModule );
	}*/

	return 0;
}