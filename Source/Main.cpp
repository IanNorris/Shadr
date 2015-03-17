#include <stdio.h>
#include <stdlib.h>
#include "dirent.h"

#include "Tokens.h"

///Enum to determine whether a string should be trimmed when being split
enum EStringTrim
{
	EStringTrim_DoNotTrim,  //!< Do not trim the string
	EStringTrim_Trim,		//!< Trim whitespace from either side of a string when splitting
};

///Enum to determine if a split string that contains character should be added to the resulting list
enum EStringStrip
{
	EStringStrip_DoNotRemoveEmpty,  //!< Keep empty strings
	EStringStrip_RemoveEmpty,		//!< Remove empty strings
};

std::string Trim( std::string tInput )
{
	static const char pszWhitespace[] = " \t\n\r";
	tInput = tInput.erase( tInput.find_last_not_of( pszWhitespace )+1 );
	tInput = tInput.erase( 0, tInput.find_first_not_of( pszWhitespace ) );
	return tInput;
}

std::vector< std::string > SplitString( std::string tInput, std::string tSeparator, EStringTrim eTrim, EStringStrip eStrip )
{
	std::vector< std::string > tResult;

	std::string::const_iterator tStart = tInput.begin();
	std::string::const_iterator tEnd;

	if( tInput.size() == 0 )
	{
			return tResult;
	}

	while( true )
	{
		tEnd = std::search< std::string::const_iterator, std::string::const_iterator >( tStart, tInput.end(), tSeparator.begin(), tSeparator.end() );

		std::string tSubString( tStart, tEnd );

		if( eTrim == EStringTrim_Trim )
		{
			tSubString = Trim( tSubString );
		}

		if( !tSubString.empty() || eStrip == EStringStrip_DoNotRemoveEmpty )
		{
			tResult.push_back( tSubString );
		}

		if( tEnd == tInput.end() )
		{
			break;
		}

		tStart = tEnd + tSeparator.size();
	}

	return tResult;
}

bool ReadFile( std::string& rtOutFile, const char* pszFilename )
{
	FILE* pFile = fopen( pszFilename, "rb" );
	
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

	unsigned int uTestsRun = 0;
	unsigned int uTestsPassed = 0;
	unsigned int uTestsFailed = 0;

	const char* pszTestDirectory = "Tests/Tokens";

	DIR* pDir = opendir( pszTestDirectory );
	dirent* pEnt;
	if( pDir )
	{
		while( (pEnt = readdir( pDir )) != NULL )
		{
			if( pEnt->d_name[0] != '.' )
			{
				printf( "Running tests for token %s\n", pEnt->d_name );

				std::string tFileContent;

				std::string tFilename = pszTestDirectory;
				tFilename += "/";
				tFilename += pEnt->d_name;

				if( !ReadFile( tFileContent, tFilename.c_str() ))
				{
					fprintf( stderr, "\tUnable to open file %s\n", tFilename.c_str() );
					continue;
				}

				std::vector< std::string > tTestLines = SplitString( tFileContent, "\n", EStringTrim_Trim, EStringStrip_RemoveEmpty );

				EShaderToken eToken = GetTokenByName( pEnt->d_name );

				if( eToken == EShaderToken_Invalid )
				{
					fprintf( stderr, "\tTest %s is not a valid token name\n",  pEnt->d_name );

					uTestsFailed++;

					continue;
				}

				for( auto& rtTest : tTestLines )
				{
					uTestsRun++;

					std::vector< std::string > tTest = SplitString( rtTest, "\t", EStringTrim_Trim, EStringStrip_RemoveEmpty );
					if( tTest.size() != 2 )
					{
						fprintf( stderr, "\tTest %s has incorrect syntax, should be <string><TAB><Y/N> per line\n", rtTest.c_str() );

						uTestsFailed++;

						goto error;
					}

					std::vector< SPossibleToken > tResults;

					bool bResult = GetPossibleTokens( tTest[0].c_str(), tTest[0].size(), 0, 0, tResults );
					bool bExpectedResult = tTest[1][0] == 'Y';

					bool bMatched = false;
					for( auto& rtResult : tResults )
					{
						if(		rtResult.eToken == eToken
							&&	rtResult.uLength == tTest[0].size() )
						{
							bMatched = true;
						}
					}

					if( bExpectedResult == bResult )
					{
						if( bExpectedResult )
						{
							if( !bMatched )
							{
								fprintf( stderr, "\tTest %s failed, expected to match %s, matched instead:\n", rtTest.c_str(), pEnt->d_name );
								for( auto& rtResult : tResults )
								{
									fprintf( stderr, "\t\t\n", GetTokenName( rtResult.eToken ) );
								}

								uTestsFailed++;

								goto error;
							}
						}
					}
					else
					{
						if( bMatched != bExpectedResult )
						{
							fprintf( stderr, "\tTest \"%s\" failed, expected %c, got %c\n", tTest[0].c_str(), bExpectedResult ? 'Y' : 'N', bResult ? 'Y' : 'N' );

							uTestsFailed++;

							goto error;
						}
					}

					uTestsPassed++;
				}
			}
error:;
		}

		closedir( pDir );
	}

	if( uTestsFailed == 0 )
	{
		printf( "\nTests Run: %u\nTests Passed: %u\nTests Failed: %u\n", uTestsRun, uTestsPassed, uTestsFailed );
	}
	else
	{
		fprintf( stderr, "\nTests Run: %u\nTests Passed: %u\nTests Failed: %u\n", uTestsRun, uTestsPassed, uTestsFailed );
	}

	std::string tFile;
	ReadFile( tFile, apszArguments[1] );

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

		if( rtToken.eToken != EShaderToken_Whitespace )
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

	return 0;
}