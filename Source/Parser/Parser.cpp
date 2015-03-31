#include "LLVM.h"
#include "AST/AST.h"
#include "Parser.h"

std::vector< SParseContext > g_tParseContextStack;

void ParseBuffer( const char* pszFilename, const std::string& tBuffer, CModule* pModule )
{
	unsigned int uBufferSize = tBuffer.length();
	unsigned int uCurrentRow = 0;
	unsigned int uCurrentCol = 0;

	const char* pszBuffer = &tBuffer[0];

	SParseContext tContext( pszBuffer, pModule );

	if( ConsumeToken( tContext ) )
	{
		CASTBlock* pProgram = ParseProgram( tContext );
		pProgram->GenerateCode( pModule );
	}

	pModule->GetExecutionEngine()->finalizeObject();

	/*while( ConsumeToken( tContext ) )
	{
		CASTExpression* pExpression = ParseExpression( tContext );

		CASTPrototype* pPrototype = new CASTPrototype( "hello", 5, CType::GetConstDoubleType() );
		CASTFunction* pFunction = new CASTFunction( pPrototype, pExpression );

		llvm::Function* pLLVMFunction = (llvm::Function*)pFunction->GenerateCode( pModule );

		pModule->GetExecutionEngine()->finalizeObject();


		void* pRawFP = pModule->GetExecutionEngine()->getPointerToFunction( pLLVMFunction );

		double(*pRawFPCasted)() = (double(*)())(intptr_t)pRawFP;

		double fReturnVal = pRawFPCasted();

		printf( "%.3f\n", fReturnVal );
	}*/

	pModule->GetModule()->dump();
}

void ParseFile( const char* pszFilename, CModule* pModule )
{
	std::string tFile;
	ReadFile( tFile, pszFilename );

	ParseBuffer( pszFilename, tFile, pModule );
}

void PushParseContext( SParseContext& rtContext )
{
	g_tParseContextStack.push_back( rtContext );
}

SParseContext PopParseContext( void )
{
	SParseContext tContext = g_tParseContextStack.back();
	g_tParseContextStack.pop_back();

	return tContext;
}