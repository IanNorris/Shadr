#include <stdio.h>
#include <stdlib.h>

#include "Tokens.h"

bool ReadFile( std::string& rtOutFile, const char* pszFilename )
{
	FILE* pFile = fopen( pszFilename, "r" );
	
	if( pFile == NULL )
	{
		return false;
	}

	fseek( pFile, 0, SEEK_END );

	unsigned int uFileLength = ftell( pFile );

	fseek( pFile, 0, SEEK_SET );

	char* pBuffer = new char[ uFileLength + 1 ];

	fread( pBuffer, 1, uFileLength, pFile );

	pBuffer[ uFileLength ] = '\0';

	rtOutFile = pBuffer;

	delete[] pBuffer;

	fclose( pFile );

	return true;
}

int main( int iArgCount, char** apszArguments )
{
	InitialiseTokenTables();

	std::string tFile;
	ReadFile( tFile, apszArguments[1] );

	const char* pszBuffer = &tFile[0];
	unsigned int uBufferSize = tFile.length();
	unsigned int uCurrentRow = 0;
	unsigned int uCurrentCol = 0;

	std::vector<SPossibleToken> rsPossibleTokens;

	while( uBufferSize && GetPossibleTokens( pszBuffer, uBufferSize, uCurrentRow, uCurrentCol, rsPossibleTokens) )
	{
		SPossibleToken& rtToken = rsPossibleTokens[0];

		if( uCurrentRow != rtToken.uAfterTokenRow )
		{
			printf( "\n" );
		}

		pszBuffer += rtToken.uLength;
		uCurrentRow = rtToken.uAfterTokenRow;
		uCurrentCol = rtToken.uAfterTokenColumn;
		uBufferSize -= rtToken.uLength;

		if( rtToken.eToken != EShaderToken_Whitespace )
		{
			printf( "%s ", GetTokenName( rtToken.eToken ) );
		}

		rsPossibleTokens.clear();
	}

	if( uBufferSize == 0 )
	{
		printf( "END\n");
	}
	else
	{
		printf( "Parse failed at %s\n", pszBuffer );
	}

	return 0;
}