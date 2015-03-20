#include <stdio.h>
#include <stdlib.h>
#include "dirent.h"

#include "Tokens.h"
#include "Error.h"

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

void IterateDirectory( const char* pszPath, unsigned int& ruTestsRun, unsigned int& ruTestsPassed, unsigned int& ruTestsFailed, std::function< void( const char*, const char*, const char*, unsigned int&, unsigned int&, unsigned int& ) > pfnFileCallback )
{
	DIR* pDir = opendir( pszPath );
	dirent* pEnt;
	if( pDir )
	{
		while( (pEnt = readdir( pDir )) != NULL )
		{
			//We only want files with no extension and that don't start with a dot
			if( !strstr( pEnt->d_name, "." ) )
			{
				printf( "Running tests for token %s\n", pEnt->d_name );

				std::string tFileContent;

				std::string tFilename = pszPath;
				tFilename += "/";
				tFilename += pEnt->d_name;

				if( !ReadFile( tFileContent, tFilename.c_str() ))
				{
					fprintf( stderr, "\tUnable to open file %s\n", tFilename.c_str() );
					continue;
				}

				pfnFileCallback( pEnt->d_name, tFilename.c_str(), tFileContent.c_str(), ruTestsRun, ruTestsPassed, ruTestsFailed );
			}
		}

		closedir( pDir );
	}
}

void TokenTest( const char* pszFilename, const char* pszFullFilename, const char* pszFileContent, unsigned int& ruTestsRun, unsigned int& ruTestsPassed, unsigned int& ruTestsFailed )
{
	std::vector< std::string > tTestLines = SplitString( pszFileContent, "\n", EStringTrim_Trim, EStringStrip_RemoveEmpty );

	EShaderToken eToken = GetTokenByName( pszFilename );

	if( eToken == EShaderToken_Invalid )
	{
		fprintf( stderr, "\tTest %s is not a valid token name\n",  pszFilename );

		ruTestsFailed++;

		return;
	}

	for( auto& rtTest : tTestLines )
	{
		ruTestsRun++;

		std::vector< std::string > tTest = SplitString( rtTest, "\t", EStringTrim_Trim, EStringStrip_RemoveEmpty );
		if( tTest.size() != 2 )
		{
			fprintf( stderr, "\tTest %s has incorrect syntax, should be <string><TAB><Y/N> per line\n", rtTest.c_str() );

			ruTestsFailed++;

			continue;
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
					fprintf( stderr, "\tTest %s failed, expected to match %s, matched instead:\n", rtTest.c_str(), pszFilename );
					for( auto& rtResult : tResults )
					{
						fprintf( stderr, "\t\t\n", GetTokenName( rtResult.eToken ) );
					}

					ruTestsFailed++;

					continue;
				}
			}
		}
		else
		{
			if( bMatched != bExpectedResult )
			{
				fprintf( stderr, "\tTest \"%s\" failed, expected %c, got %c\n", tTest[0].c_str(), bExpectedResult ? 'Y' : 'N', bResult ? 'Y' : 'N' );

				ruTestsFailed++;

				continue;
			}
		}

		ruTestsPassed++;
	}
}

void TokenChainTest( const char* pszFilename, const char* pszFullFilename, const char* pszFileContent, unsigned int& ruTestsRun, unsigned int& ruTestsPassed, unsigned int& ruTestsFailed )
{
	ruTestsRun++;

	const char* pszBuffer = pszFileContent;
	unsigned int uBufferSize = strlen( pszFileContent );
	unsigned int uCurrentRow = 0;
	unsigned int uCurrentCol = 0;

	PushCurrentFile( pszFilename );

	std::vector<SPossibleToken> asPossibleTokens;


	std::string tExpectedTokens;

	std::string tTokenFilename = pszFullFilename;
	tTokenFilename += ".tokens";
	if( !ReadFile( tExpectedTokens, tTokenFilename.c_str() ) )
	{
		Assert( 0, "Unable to open token file %s ", tTokenFilename.c_str() );
	}

	std::string tTokens;

	while( uBufferSize && GetPossibleTokens( pszBuffer, uBufferSize, uCurrentRow, uCurrentCol, asPossibleTokens) )
	{
		SPossibleToken& rtToken = asPossibleTokens[0];

		pszBuffer += rtToken.uLength;
		uCurrentRow = rtToken.uAfterTokenRow;
		uCurrentCol = rtToken.uAfterTokenColumn;
		uBufferSize -= rtToken.uLength;

		//if( rtToken.eToken != EShaderToken_Whitespace && rtToken.eToken != EShaderToken_Comment )
		{
			if( asPossibleTokens.size() > 1 )
			{
				std::string tOptions;

				for( auto sToken : asPossibleTokens )
				{
					tOptions += GetTokenName( sToken.eToken );
					tOptions += " ";
				}

				Error_Compiler( EError_Debug, uCurrentRow, uCurrentCol, "Token ambiguity, matched: ", tOptions.c_str() );
			}

			tTokens += GetTokenName( rtToken.eToken );
			tTokens += " ";
		}

		asPossibleTokens.clear();
	}

	if( uBufferSize == 0 )
	{
		tTokens += "END";
	}
	else
	{
		Error_Compiler( EError_Error, uCurrentRow, uCurrentCol, "Parse failed at %s\n", pszBuffer );
	}

	if( tTokens.compare( tExpectedTokens ) == 0 )
	{
		ruTestsPassed++;
	}
	else
	{
		fprintf( stderr, "Failed %s\n\tExpected: %s\n\tGot:      %s\n", pszFilename, tExpectedTokens.c_str(), tTokens.c_str() );
		ruTestsFailed++;
	}

	PopCurrentFile();
}

int main( int iArgCount, char** apszArguments )
{
	InitialiseTokenTables();

	unsigned int uTestsRun = 0;
	unsigned int uTestsPassed = 0;
	unsigned int uTestsFailed = 0;

	IterateDirectory( "Tests/Tokens", uTestsRun, uTestsPassed, uTestsFailed, TokenTest );
	IterateDirectory( "Tests/TokenChains", uTestsRun, uTestsPassed, uTestsFailed, TokenChainTest );

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

	return 0;
}