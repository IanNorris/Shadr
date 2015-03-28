#include "LLVM.h"
#include "AST/AST.h"
#include "Parser.h"
#include "Utility/Error.h"
#include "Utility/Utility.h"

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
