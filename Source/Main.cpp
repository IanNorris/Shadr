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
	InitialiseTokenTables();
	InitialiseBasicTypes();
	InitialiseFormats();

	RunUnitTests();
	
	if( iArgCount > 1 )
	{
		CCompilationUnit tCU( apszArguments[1] );

		ParseFile( apszArguments[1], &tCU );

		return DidCompilationSucceed() ? 0 : 1;
	}
	else
	{
		std::string tLine;
		while( true )
		{
			std::getline( std::cin, tLine );

			CCompilationUnit tCU( "stdin" );

			ParseBuffer( "test", tLine, &tCU );
		}

		return 0;
	}
}