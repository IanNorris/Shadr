#include "Tokens.h"
#include "Error.h"
#include <algorithm>
#include <unordered_map>

typedef std::function< unsigned int( const char*, unsigned int, unsigned int&, unsigned int& ) > TTokenCallback;

std::unordered_map< EShaderToken, std::vector< EShaderToken > > g_tIgnoreTokens;

struct SBasicTokenMap
{
	const char*		pszTokenName;
	const char*		pszTokenString;
	unsigned int	uTokenLength;
};

struct SRegexTokenMap
{
	const char*		pszTokenName;
	std::regex		tRegex;
};

struct SCallbackTokenMap
{
	const char*		pszTokenName;
	TTokenCallback	pfnTokenCallback;
};

SBasicTokenMap g_asBasicTokens[  GetCountFromTokenRange(EShaderToken_BeginBasic, EShaderToken_EndBasic)  ] = 
{
	{ "comma",					",", 0 },				// EShaderToken_Comma
	
	{ "left shift",				"<<", 0 },				// EShaderToken_Binary_Bitwise_Shift_Left
	{ "right shift",			">>", 0 },				// EShaderToken_Binary_Bitwise_Shift_Right
	{ "or",						"|", 0 },				// EShaderToken_Binary_Bitwise_Or		
	{ "and",					"&", 0 },				// EShaderToken_Binary_Bitwise_And		
	{ "lazy or",				"||", 0 },				// EShaderToken_Binary_Bitwise_Lazy_Or	
	{ "lazy and",				"&&", 0 },				// EShaderToken_Binary_Bitwise_Lazy_And	
	{ "plus",					"+", 0 },				// EShaderToken_Binary_Operator_Plus	
	{ "minus",					"-", 0 },				// EShaderToken_Binary_Operator_Minus	
	{ "divide",					"/", 0 },				// EShaderToken_Binary_Operator_Divide	
	{ "multiply",				"*", 0 },				// EShaderToken_Binary_Operator_Multiply
	{ "modulo",					"%", 0 },				// EShaderToken_Binary_Operator_Modulo	
	{ "less than",				"<", 0 },				// EShaderToken_Binary_Comparison_LT	
	{ "greater than",			">", 0 },				// EShaderToken_Binary_Comparison_GT	
	{ "less than or equal",		"<=", 0 },				// EShaderToken_Binary_Comparison_LTE	
	{ "greater than or equal",	">=", 0 },				// EShaderToken_Binary_Comparison_GTE	
	{ "equal",					"==", 0 },				// EShaderToken_Binary_Comparison_Equal	
	{ "not equal",				"!=", 0 },				// EShaderToken_Binary_Comparison_NotEqual
	
	{ "not",					"!", 0 },				// EShaderToken_Unary_Not				
	{ "increment",				"++", 0 },				// EShaderToken_Unary_Increment			
	{ "decrement",				"--", 0 },				// EShaderToken_Unary_Deccrement		
	
	{ "assign",					"=", 0 },				// EShaderToken_Assign					
	{ "assign add",				"+=", 0 },				// EShaderToken_Assign_Add		
	{ "assign subtract",		"-=", 0 },				// EShaderToken_Assign_Subtract		
	{ "assign divide",			"/=", 0 },				// EShaderToken_Assign_Divide			
	{ "assign multiply",		"*=", 0 },				// EShaderToken_Assign_Multiply			
	{ "assign modulo",			"%=", 0 },				// EShaderToken_Assign_Modulo	
	{ "assign shift left",		"<<=", 0 },				// EShaderToken_Assign_Shift_Left	
	{ "assign shift right",		">>=", 0 },				// EShaderToken_Assign_Shift_Right

	{ "semi-colon",				";", 0 },				// EShaderToken_SemiColon				
	{ "colon",					":", 0 },				// EShaderToken_Colon					
	{ "dot",					".", 0 },				// EShaderToken_Colon					
	{ "reference",				"&", 0 },				// EShaderToken_Reference				
	
	{ "return",					"return", 0 },			// EShaderToken_Return					
	{ "if",						"if", 0 },				// EShaderToken_If						
	{ "else",					"else", 0 },			// EShaderToken_Else					
	{ "for",					"for", 0 },				// EShaderToken_For						
	{ "do",						"do", 0 },				// EShaderToken_Do						
	{ "while",					"while", 0 },			// EShaderToken_While					
	{ "const",					"const", 0 },			// EShaderToken_Const					
	{ "struct",					"struct", 0 },			// EShaderToken_Struct					
	{ "cbuffer",				"cbuffer", 0 },			// EShaderToken_Constant_Buffer			
	{ "include",				"include", 0 },			// EShaderToken_Include					
	{ "buffer",					"Buffer", 0 },			// EShaderToken_Buffer					
	
	{ "full precision",			"full", 0 },			// EShaderToken_Modifier_Precision_Full	
	{ "half precision",			"half", 0 },			// EShaderToken_Modifier_Precision_Half	
	
	{ "unorm",					"unorm", 0 },			// EShaderToken_Modifier_Precision_UNorm
	{ "snorm",					"snorm", 0 },			// EShaderToken_Modifier_Precision_SNorm
	
	{ "feature",				"feature", 0 },			// EShaderToken_Feature					
	{ "weak alias",				"weakAlias", 0 },		// EShaderToken_Alias_Weak				
	{ "strict alias",			"strictAlias", 0 },		// EShaderToken_Alias_Strict			

	{ "open parenthesis",		"(", 0 },				// EShaderToken_Parenthesis_Open	
	{ "close parenthesis",		")", 0 },				// EShaderToken_Parenthesis_Close	
	{ "open brace",				"{", 0 },				// EShaderToken_Brace_Open	
	{ "close brace",			"}", 0 },				// EShaderToken_Brace_Close	
	{ "open square bracket",	"[", 0 },				// EShaderToken_Square_Open	
	{ "close square bracket",	"]", 0 },				// EShaderToken_Square_Close	
};

SRegexTokenMap g_asRegexTokens[  GetCountFromTokenRange(EShaderToken_BeginRegex, EShaderToken_EndRegex)  ] = 
{
	{ "identifier", std::regex( "[a-zA-Z_][a-zA-Z0-9_]+" ) },
	{ "float",		std::regex( "[\\-]?(?:(?:[0-9]+\\.[0-9]*)|(?:[0-9]*\\.[0-9]+))f?" ) }, //Allow .3f or 3.f but not .f
	{ "int",		std::regex( "-?[0-9]+" ) },
};



SCallbackTokenMap g_asCallbackTokens[  GetCountFromTokenRange(EShaderToken_BeginCallback, EShaderToken_EndCallback)  ] = 
{
	{ "whitespace", []( const char* pszInputString, unsigned int uCharactersLeft, unsigned int& ruCurrentRow, unsigned int& ruCurrentCol ) -> unsigned int
		{
			unsigned int uMatched = 0;
			while( *pszInputString )
			{
				switch( *(pszInputString++) )
				{
					case ' ':
						uMatched++;
						ruCurrentCol++;
						uCharactersLeft--;
						break;
					case '\t':
						uMatched++;
						ruCurrentCol+=4;
						uCharactersLeft--;
						break;
					case '\n':
						uMatched++;
						ruCurrentCol = 0;
						ruCurrentRow++;
						uCharactersLeft--;
						break;
					case '\r':
						uMatched++;
						uCharactersLeft--;
						break;
					default:
						return uMatched;
				}
			}

			return uMatched;
		}
	},
	{ "comment", []( const char* pszInputString, unsigned int uCharactersLeft, unsigned int& ruCurrentRow, unsigned int& ruCurrentCol ) -> unsigned int
		{
			unsigned int uStartRow = ruCurrentRow;
			unsigned int uStartCol = ruCurrentCol;

			unsigned int uMatched = 0;

			if( uCharactersLeft >= 2 )
			{
				//Comments starting with '//'
				if( *(pszInputString+0) == '/' && *(pszInputString+1) == '/' )
				{
					uMatched += 2;

					pszInputString+=2;
					uCharactersLeft-=2;
					ruCurrentCol+=2;

					while( *pszInputString && uCharactersLeft )
					{
						bool bIsUnixNewline = *pszInputString == '\n';
						bool bIsWindowsNewline = (uCharactersLeft >= 2) && (*pszInputString == '\r') && (*(pszInputString+1) == '\n');

						//Found the end of the comment
						if( bIsUnixNewline || bIsWindowsNewline )
						{
							ruCurrentRow++;
							ruCurrentCol = 0;	

							//Comment was escaped
							if( *(pszInputString-1) == '\\' )
							{
								//Just carry on
							}
							else
							{

								uMatched++;

								return uMatched;
							}

							//Eat the newline or our row will be out
							if( bIsWindowsNewline )
							{
								pszInputString++;
							}
						}

						uMatched++;

						pszInputString++;
						uCharactersLeft--;
					}
				}
				//Comments starting with '/*'
				else if( *(pszInputString+0) == '/' && *(pszInputString+1) == '*' )
				{
					uMatched += 2;

					pszInputString+=2;
					uCharactersLeft--;
					ruCurrentCol+=2;

					while( *pszInputString && uCharactersLeft >= 2 )
					{
						if( *(pszInputString+0) == '*' && *(pszInputString+1) == '/' )
						{
							uMatched += 2;

							pszInputString+=2;
							uCharactersLeft--;
							ruCurrentCol+=2;

							return uMatched;
						}

						uMatched++;

						pszInputString++;
						uCharactersLeft--;
					}

					Error_Compiler( EError_Fatal, uStartRow, uStartCol, "End of file reached before comment closed." );
				}
			}

			return uMatched;
		}
	},
};

bool GetPossibleTokens( const char* pszInputString, unsigned int uCharactersLeft, unsigned int uCurrentRow, unsigned int uCurrentCol, std::vector<SPossibleToken>& rsPossibleTokens )
{
	unsigned int uBasicTokenCount = GetCountFromTokenRange( EShaderToken_BeginBasic, EShaderToken_EndBasic );

	for( unsigned int uToken = 0; uToken < uBasicTokenCount; ++uToken )
	{
		if( uCharactersLeft >= g_asBasicTokens[ uToken ].uTokenLength )
		{
			if( strncmp( g_asBasicTokens[ uToken ].pszTokenString, pszInputString, g_asBasicTokens[ uToken ].uTokenLength ) == 0 )
			{
				SPossibleToken tToken;
				tToken.eToken = (EShaderToken)(EShaderToken_BeginBasic - uToken);
				tToken.uLength = g_asBasicTokens[ uToken ].uTokenLength;
				tToken.uAfterTokenRow = uCurrentRow;
				tToken.uAfterTokenColumn = uCurrentCol + tToken.uLength;
				tToken.pszToken = pszInputString;
				rsPossibleTokens.push_back( tToken );
			}
		}
	}

	unsigned int uRegexTokenCount = GetCountFromTokenRange( EShaderToken_BeginRegex, EShaderToken_EndRegex );

	for( unsigned int uToken = 0; uToken < uRegexTokenCount; ++uToken )
	{
		std::cmatch matches;
		std::regex_search( pszInputString, matches, g_asRegexTokens[ uToken ].tRegex, std::regex_constants::match_continuous );

		if( matches.size() > 0 && matches.position() == 0 )
		{
			SPossibleToken tToken;
			tToken.eToken = (EShaderToken)(EShaderToken_BeginRegex - uToken);

			//If we're about to create an identifier token yet we've already identified a keyword, bail
			if( (tToken.eToken == EShaderToken_Identifier && !rsPossibleTokens.empty() ) )
			{
				continue;
			}

			tToken.uLength = matches[0].length();
			tToken.uAfterTokenRow = uCurrentRow;
			tToken.uAfterTokenColumn = uCurrentCol + tToken.uLength;
			tToken.pszToken = pszInputString;
			rsPossibleTokens.push_back( tToken );
		}
	}

	unsigned int uCallbackTokenCount = GetCountFromTokenRange( EShaderToken_BeginCallback, EShaderToken_EndCallback );

	for( unsigned int uToken = 0; uToken < uCallbackTokenCount; ++uToken )
	{
		unsigned int uTempCurrentRow = uCurrentRow;
		unsigned int uTempCurrentCol = uCurrentCol;

		unsigned int uMatched = g_asCallbackTokens[ uToken ].pfnTokenCallback( pszInputString, uCharactersLeft, uTempCurrentRow, uTempCurrentCol );

		if( uMatched > 0 )
		{
			SPossibleToken tToken;
			tToken.eToken = (EShaderToken)(EShaderToken_BeginCallback - uToken);
			tToken.uLength = uMatched;
			tToken.uAfterTokenRow = uTempCurrentRow;
			tToken.uAfterTokenColumn = uTempCurrentCol;
			tToken.pszToken = pszInputString;
			rsPossibleTokens.push_back( tToken );
		}
	}

	FilterTokens( rsPossibleTokens );

	std::sort( rsPossibleTokens.begin(), rsPossibleTokens.end() );

	return !rsPossibleTokens.empty();
}

const char* GetTokenName( EShaderToken eToken )
{
	if( eToken <= EShaderToken_BeginBasic && eToken >= EShaderToken_EndBasic )
	{
		return g_asBasicTokens[ GetIndexFromRange( eToken, EShaderToken_BeginBasic ) ].pszTokenName;
	}

	if( eToken <= EShaderToken_BeginRegex && eToken >= EShaderToken_EndRegex )
	{
		return g_asRegexTokens[ GetIndexFromRange( eToken, EShaderToken_BeginRegex ) ].pszTokenName;
	}

	if( eToken <= EShaderToken_BeginCallback && eToken >= EShaderToken_EndCallback )
	{
		return g_asCallbackTokens[ GetIndexFromRange( eToken, EShaderToken_BeginCallback ) ].pszTokenName;
	}

	return "Unknown";
}

EShaderToken GetTokenByName( const char* pszTokenName )
{
	unsigned int uBasicTokenCount = GetCountFromTokenRange( EShaderToken_BeginBasic, EShaderToken_EndBasic );

	for( unsigned int uToken = 0; uToken < uBasicTokenCount; ++uToken )
	{
		if( _stricmp( g_asBasicTokens[ uToken ].pszTokenName, pszTokenName ) == 0 )
		{
			return (EShaderToken)(EShaderToken_BeginBasic - uToken);
		}
	}

	unsigned int uRegexTokenCount = GetCountFromTokenRange( EShaderToken_BeginRegex, EShaderToken_EndRegex );

	for( unsigned int uToken = 0; uToken < uRegexTokenCount; ++uToken )
	{
		if( _stricmp( g_asRegexTokens[ uToken ].pszTokenName, pszTokenName ) == 0 )
		{
			return (EShaderToken)(EShaderToken_BeginRegex - uToken);
		}
	}

	unsigned int uCallbackTokenCount = GetCountFromTokenRange( EShaderToken_BeginCallback, EShaderToken_EndCallback );

	for( unsigned int uToken = 0; uToken < uCallbackTokenCount; ++uToken )
	{
		if( _stricmp( g_asCallbackTokens[ uToken ].pszTokenName, pszTokenName ) == 0 )
		{
			return (EShaderToken)(EShaderToken_BeginCallback - uToken);
		}
	}

	return EShaderToken_Invalid;
}

void InitialiseTokenTables( void )
{
	unsigned int uTokenCount = GetCountFromTokenRange( EShaderToken_BeginBasic, EShaderToken_EndBasic );

	for( unsigned int uToken = 0; uToken < uTokenCount; ++uToken )
	{
		g_asBasicTokens[ uToken ].uTokenLength = strlen( g_asBasicTokens[ uToken ].pszTokenString );
	}

	//List of tokens to ignore
	//Throw away a token X if we encounter Y
	//X												Y
	g_tIgnoreTokens[ EShaderToken_Int ].push_back( EShaderToken_Float );
	g_tIgnoreTokens[ EShaderToken_Dot ].push_back( EShaderToken_Float );
	g_tIgnoreTokens[ EShaderToken_Binary_Operator_Divide ].push_back( EShaderToken_Comment );
}

void FilterTokens( std::vector<SPossibleToken>& rsPossibleTokens )
{
	auto tIter = rsPossibleTokens.begin();
	while( tIter != rsPossibleTokens.end() )
	{
		bool bErase = false;

		auto tIgnoreIter = g_tIgnoreTokens.find( (*tIter).eToken );
		if( tIgnoreIter != g_tIgnoreTokens.end() )
		{
			for( auto tFindIter = rsPossibleTokens.begin(); tFindIter != rsPossibleTokens.end(); ++tFindIter )
			{
				auto& rtIgnoreList = (*tIgnoreIter).second;

				for( auto tListIter = rtIgnoreList.begin(); tListIter != rtIgnoreList.end(); ++tListIter )
				{
					if( (*tFindIter).eToken == (*tListIter) )
					{
						bErase = true;
						break;
					}
				}
			}
		}

		if( bErase )
		{
			tIter = rsPossibleTokens.erase( tIter );
		}
		else
		{
			++tIter;
		}
	}
}