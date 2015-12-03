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
	InitialiseFormats( rootPath );

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
		CCompilationUnit tCU( apszArguments[1] );

		CASTProgram* pProgram = ParseFile( apszArguments[1], &tCU, pIntrinsicsScope );

		//NOTE: At this point all the pointers from the file data will be invalidated. Do all the work in ParseBuffer.

		const char* pszFormatter = apszArguments[2];
		CFormatter* pFormatter = GetFormatter( pszFormatter );
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