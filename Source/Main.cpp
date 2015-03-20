#include <stdio.h>
#include <stdlib.h>


#include "Tokens.h"
#include "Error.h"
#include "Utility.h"

#include "UnitTesting.h"

void ParseFile( const char* pszFilename )
{
	std::string tFile;
	ReadFile( tFile, pszFilename );

	const char* pszBuffer = &tFile[0];
	unsigned int uBufferSize = tFile.length();
	unsigned int uCurrentRow = 0;
	unsigned int uCurrentCol = 0;

	std::vector<SPossibleToken> asPossibleTokens;

	while( uBufferSize && GetPossibleTokens( pszBuffer, uBufferSize, uCurrentRow, uCurrentCol, asPossibleTokens) )
	{
		SPossibleToken& rtToken = asPossibleTokens[0];

		if( uCurrentRow != rtToken.uAfterTokenRow )
		{
			printf( "\n" );
		}

		pszBuffer += rtToken.uLength;
		uCurrentRow = rtToken.uAfterTokenRow;
		uCurrentCol = rtToken.uAfterTokenColumn;
		uBufferSize -= rtToken.uLength;

		if( rtToken.eToken != EShaderToken_Whitespace && rtToken.eToken != EShaderToken_Comment )
		{
			if( asPossibleTokens.size() > 1 )
			{
				printf( "\nToken ambiguity, matched:\n" );
				for( auto sToken : asPossibleTokens )
				{
					printf( "\t%s\n", GetTokenName( sToken.eToken ) );
				}
			}

			printf( "%s ", GetTokenName( rtToken.eToken ) );
		}

		asPossibleTokens.clear();
	}

	if( uBufferSize == 0 )
	{
		printf( "END\n");
	}
	else
	{
		printf( "Parse failed at %s\n", pszBuffer );
	}
}

int main( int iArgCount, char** apszArguments )
{
	InitialiseTokenTables();

	RunUnitTests();
	
	if( iArgCount > 1 )
	{
		ParseFile( apszArguments[1] );
	}
	

	return 0;
}