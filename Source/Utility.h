#if !defined( SHADR_UTILITY_H )
#define SHADR_UTILITY_H

#include <string>
#include <vector>

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

std::string Trim( std::string tInput );

std::vector< std::string > SplitString( std::string tInput, std::string tSeparator, EStringTrim eTrim, EStringStrip eStrip );

bool ReadFile( std::string& rtOutFile, const char* pszFilename );

#endif //SHADR_UTILITY_H