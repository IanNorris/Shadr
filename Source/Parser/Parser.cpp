#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"
#include "Reconcile.h"
#include "Optimise.h"

CASTProgram* ParseBuffer( const char* pszFilename, const std::string& tBuffer, CCompilationUnit* pCU, CScope* pParentScope )
{
	unsigned int uBufferSize = tBuffer.length();
	unsigned int uCurrentRow = 0;
	unsigned int uCurrentCol = 0;

	const char* pszBuffer = &tBuffer[0];

	SParseContext tContext( pszBuffer, pCU );

	if( ConsumeToken( tContext ) )
	{
		CASTProgram* pProgram = ParseProgram( tContext, pParentScope );

		Reconcile( pProgram, nullptr );
		IdentifySideEffects( pProgram, nullptr );

		std::vector< std::pair< CASTExpression*, CASTExpression* > > tExpressionReplacements;
		Optimise( pProgram, nullptr, nullptr, tExpressionReplacements );

		Assert( tExpressionReplacements.empty(), "There should be no replacements at this level" );

		return pProgram;
	}

	return nullptr;
}

CASTProgram* ParseFile( const char* pszFilename, CCompilationUnit* pCU, CScope* pParentScope )
{
	PushCurrentFile( pszFilename );

	std::string tFile;
	ReadFile( tFile, pszFilename );

	CASTProgram* pProgram = ParseBuffer( pszFilename, tFile, pCU, pParentScope );

	PopCurrentFile();

	return pProgram;
}
