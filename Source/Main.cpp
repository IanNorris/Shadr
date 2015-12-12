#include <stdio.h>
#include <stdlib.h>

#include "Tokens.h"
#include "Utility/Error.h"
#include "Utility/Utility.h"
#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser/Parser.h"
#include "Format/Format.h"

#include "UnitTests/UnitTesting.h"

#include <iostream>

extern unsigned int g_uErrorCount;
extern unsigned int g_uWarningCount;

int main( int iArgCount, char** apszArguments )
{
	std::string executablePath = apszArguments[0];
#if defined( _WIN32 )
	size_t rootPos = executablePath.find_last_of( '\\' );
#else
	size_t rootPos = executablePath.find_last_of( '/' );
#endif
	if( rootPos == std::string::npos )
	{
		fprintf( stderr, "Unable to calculate path of the current executable %s.\n", executablePath.c_str() );
		exit( 1 );
	}

	std::string rootPath = executablePath.substr( 0, rootPos + 1 );

	InitialiseTokenTables();
	InitialiseBasicTypes();

	RunUnitTests( rootPath );

	//Parse intrinsics
	std::string intrinsicsPath = rootPath + "Config\\Intrinsics.hlsl";
	CCompilationUnit tIntrinsicsCU( intrinsicsPath.c_str() );
	CASTProgram* pIntrinsics = ParseFile( intrinsicsPath.c_str(), &tIntrinsicsCU, nullptr );
	if( pIntrinsics == nullptr )
	{
		fprintf( stderr, "Unable to find or parse %s.\n", intrinsicsPath.c_str() );
		exit( 1 );
	}
	CScope* pIntrinsicsScope = &pIntrinsics->GetScope();
	
	if( iArgCount > 1 )
	{
		//Our formatter may have some extensions we need to pull in.
		const char* pszFormatter = apszArguments[2];
		std::string pathToFormatterExtensions = rootPath + "Formats\\";
		pathToFormatterExtensions += pszFormatter;
		std::string pathToFormatterDefinition = pathToFormatterExtensions;
		pathToFormatterExtensions += ".hlsl";
		pathToFormatterDefinition += ".xml";
		CCompilationUnit tFormatterCU( pathToFormatterExtensions.c_str() );
		CASTProgram* pFormatterExtensions = ParseFile( pathToFormatterExtensions.c_str(), &tFormatterCU, &pIntrinsics->GetScope() );

		CScope* pIntrinsicsScope = pFormatterExtensions ? &pFormatterExtensions->GetScope() : &pIntrinsics->GetScope();

		CCompilationUnit tCU( apszArguments[1] );
		CASTProgram* pProgram = ParseFile( apszArguments[1], &tCU, pIntrinsicsScope );

		//NOTE: At this point all the pointers from the file data will be invalidated. Do all the work in ParseBuffer.
		
		CFormatter* pFormatter = InitialiseFormat( pathToFormatterDefinition.c_str() );
		if( pFormatter )
		{
			CASTFormatter* pProgramFormatter = pFormatter->GetASTType( "Program" );
			if( pProgramFormatter )
			{
				CFormatterContext tContext( pFormatter );

				pProgramFormatter->Action( &tContext, pProgram );

				printf( "%s\n", tContext.tCurrentElement.c_str() );
			}
		}

		if( g_uErrorCount > 0 || g_uWarningCount > 0 )
		{
			fprintf( stderr, "%s has %u error(s) and %u warning(s).\n", apszArguments[1], g_uErrorCount, g_uWarningCount );
		}

		return DidCompilationSucceed() ? 0 : 1;
	}
	else
	{
		std::string tLine;
		while( true )
		{
			std::getline( std::cin, tLine );

			CCompilationUnit tCU( "stdin" );

			ParseBuffer( "test", tLine, &tCU, pIntrinsicsScope );
		}

		return 0;
	}
}
