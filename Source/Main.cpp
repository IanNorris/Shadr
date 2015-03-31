#include <stdio.h>
#include <stdlib.h>

#include "Tokens.h"
#include "Utility/Error.h"
#include "Utility/Utility.h"
#include "LLVM.h"
#include "AST/AST.h"
#include "Parser/Parser.h"

#include "UnitTests/UnitTesting.h"

#include <iostream>

int main( int iArgCount, char** apszArguments )
{
	InitialiseTokenTables();
	InitialiseBasicTypes();
	InitialiseLLVM();
	
	CModule tModule( "Shader" );

	RunUnitTests();
	
	if( iArgCount > 1 )
	{
		ParseFile( apszArguments[1], &tModule );

		return DidCompilationSucceed() ? 0 : 1;
	}
	else
	{
		std::string tLine;
		while( true )
		{
			std::getline( std::cin, tLine );

			ParseBuffer( "test", tLine, &tModule );
		}

		return 0;
	}
}