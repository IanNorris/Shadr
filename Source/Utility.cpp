#include "Utility.h"

#include <algorithm>

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