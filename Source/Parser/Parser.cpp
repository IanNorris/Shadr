#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"

std::vector< SParseContext > g_tParseContextStack;

CASTProgram* ParseBuffer( const char* pszFilename, const std::string& tBuffer, CCompilationUnit* pCU )
{
	unsigned int uBufferSize = tBuffer.length();
	unsigned int uCurrentRow = 0;
	unsigned int uCurrentCol = 0;

	const char* pszBuffer = &tBuffer[0];

	SParseContext tContext( pszBuffer, pCU );

	if( ConsumeToken( tContext ) )
	{
		CASTProgram* pProgram = ParseProgram( tContext );
		return pProgram;
	}

	return nullptr;
}

CASTProgram* ParseFile( const char* pszFilename, CCompilationUnit* pCU )
{
	PushCurrentFile( pszFilename );

	std::string tFile;
	ReadFile( tFile, pszFilename );

	CASTProgram* pProgram = ParseBuffer( pszFilename, tFile, pCU );

	PopCurrentFile();

	return pProgram;
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