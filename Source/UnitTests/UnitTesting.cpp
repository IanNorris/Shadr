#include "UnitTesting.h"
#include "Utility/Error.h"
#include "Utility/Utility.h"
#include "Tokens.h"

#include <functional>
#include "dirent.h"


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
						fprintf( stderr, "%s\t\t\n", GetTokenName( rtResult.eToken ) );
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

				Error_Compiler( EError_Debug, pszFilename, uCurrentRow, uCurrentCol, "Token ambiguity, matched: ", tOptions.c_str() );
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
		Error_Compiler( EError_Error, pszFilename, uCurrentRow, uCurrentCol, "Parse failed at %s\n", pszBuffer );
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

void RunUnitTests( std::string rootPath )
{
	unsigned int uTestsRun = 0;
	unsigned int uTestsPassed = 0;
	unsigned int uTestsFailed = 0;

	IterateDirectory( std::string(rootPath + "Tests/Tokens").c_str(), uTestsRun, uTestsPassed, uTestsFailed, TokenTest );
	IterateDirectory( std::string(rootPath + "Tests/TokenChains").c_str(), uTestsRun, uTestsPassed, uTestsFailed, TokenChainTest );

	if( uTestsFailed == 0 )
	{
		printf( "\nTests Run: %u\nTests Passed: %u\nTests Failed: %u\n", uTestsRun, uTestsPassed, uTestsFailed );
	}
	else
	{
		fprintf( stderr, "\nTests Run: %u\nTests Passed: %u\nTests Failed: %u\n", uTestsRun, uTestsPassed, uTestsFailed );
	}
}