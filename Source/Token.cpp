#include "Tokens.h"
#include "Utility/Error.h"
#include <algorithm>
#include <unordered_map>

typedef std::function< unsigned int( const char*, unsigned int, unsigned int&, unsigned int& ) > TTokenCallback;



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

std::unordered_map< EShaderToken, std::vector< EShaderToken > > g_tIgnoreTokens;

std::unordered_map< EShaderToken, SPrecedence > g_atTokenPrecedence[ EOperatorType_MAX ];

//TODO:
// * Ternary
// * XOR
// * &=
// * |=
// * ^=
// * ~
// * ~=
// * Casts

SBasicTokenMap g_asBasicTokens[  GetCountFromTokenRange(EShaderToken_BeginBasic, EShaderToken_EndBasic)  ] = 
{
	{ "comma",					",", 0 },				// EShaderToken_Comma
	
	{ "left shift",				"<<", 0 },				// EShaderToken_Binary_Bitwise_Shift_Left
	{ "right shift",			">>", 0 },				// EShaderToken_Binary_Bitwise_Shift_Right
	{ "or",						"|", 0  },				// EShaderToken_Binary_Bitwise_Or		
	{ "and",					"&", 0  },				// EShaderToken_Binary_Bitwise_And		
	{ "lazy or",				"||", 0 },				// EShaderToken_Binary_Bitwise_Lazy_Or	
	{ "lazy and",				"&&", 0 },				// EShaderToken_Binary_Bitwise_Lazy_And	
	{ "plus",					"+", 0  },				// EShaderToken_Binary_Operator_Plus	
	{ "minus",					"-", 0  },				// EShaderToken_Binary_Operator_Minus	
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
	{ "bitwise not",			"~", 0 },				// EShaderToken_Unary_Bitwise_Not		
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
	{ "dot",					".", 0 },				// EShaderToken_Dot
	
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

	{ "ternary",				"?", 0 },				// EShaderToken_Ternary_QMark

	//Special tokens to resolve disambiguity
	{ "+++",					"+++", 0 },				//EShaderToken_Special_TriplePlus
	{ "---",					"---", 0 },				//EShaderToken_Special_TripleMinus

	//Variable direction operators
	{ "in",						"in", 0 },				//EShaderToken_In
	{ "out",					"out", 0 },				//EShaderToken_Out

	//Function decorators
	{ "intrinsic",				"intrinsic", 0 },		//EShaderToken_Intrinsic
	{ "inline",					"inline", 0 },			//EShaderToken_Inline

	//Annotations
	{ "sideEffect",				"sideEffect", 0 },		//EShaderToken_Annotation_SideEffect
	{ "unroll",					"unroll", 0 },			//EShaderToken_Annotation_Unroll
	{ "forceInline",			"forceInline", 0 },		//EShaderToken_Annotation_ForceInline
};

SRegexTokenMap g_asRegexTokens[  GetCountFromTokenRange(EShaderToken_BeginRegex, EShaderToken_EndRegex)  ] = 
{
	{ "float",		std::regex( "-?(?:(?:[0-9]+\\.[0-9]*)|(?:[0-9]*\\.[0-9]+))(?:f|(?:e[+\\-]?[0-9]+f?))?" ) }, //Allow .3f or 3.f but not .f
	{ "int",		std::regex( "-?(?:(?:0[Xx](?:[0-9a-fA-F]+))|(?:[0-9]+))" ) },
	{ "boolean",	std::regex( "(?:true|TRUE|false|FALSE)" ) },
	{ "identifier", std::regex( "[a-zA-Z_][a-zA-Z0-9_]*" ) },
};

inline bool IsWhitespace( char ch )
{
	switch( ch )
	{
		case ' ':
		case '\t':
		case '\n':
		case '\r':
		case '\0':
			return true;
		default:
			return false;
	}
}

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

					Error_Compiler( EError_Fatal, GetCurrentFilename(), uStartRow, uStartCol, "End of file reached before comment closed." );
				}
			}

			return uMatched;
		}
	},
};

std::string nextTokenString;

std::cmatch regexMatches;

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

	const char* findNextWhitespace = pszInputString;
	while( !IsWhitespace( *(findNextWhitespace++) ) );
	size_t bytesUntilEndOfRange = findNextWhitespace - pszInputString;
	nextTokenString.assign( pszInputString, bytesUntilEndOfRange );

	for( unsigned int uToken = 0; uToken < uRegexTokenCount; ++uToken )
	{
		std::regex_search( nextTokenString.c_str(), regexMatches, g_asRegexTokens[ uToken ].tRegex, std::regex_constants::match_continuous );

		if( regexMatches.size() > 0 && regexMatches.position() == 0 )
		{
			SPossibleToken tToken;
			tToken.eToken = (EShaderToken)(EShaderToken_BeginRegex - uToken);
			tToken.uLength = regexMatches[0].length();

			bool bKeepToken = true;

			//If we're about to create an identifier token
			if( tToken.eToken == EShaderToken_Identifier )
			{
				// yet we've already identified a keyword
				if( !rsPossibleTokens.empty() )
				{
					for( auto tIter = rsPossibleTokens.begin(); tIter != rsPossibleTokens.end(); )
					{
						auto& rtToken = (*tIter);

						//Check we've got a keyword/basic token (this test is redundant given the order, but we'll check anyway)
						if(		(rtToken.eToken <= EShaderToken_BeginBasic && rtToken.eToken >= EShaderToken_EndBasic)
							||	rtToken.eToken == EShaderToken_Boolean )
						{
							//And that keyword is shorter than the identifier
							if( rtToken.uLength < tToken.uLength )
							{
								//Ditch the old token, keep the new one instead
								tIter = rsPossibleTokens.erase( tIter );

								//Do not break here because multiple tokens may need to be rejected
								//eg 'in' in 'inline'
								//break;
							}
							else
							{
								bKeepToken = false;

								++tIter;
							}
						}
						else
						{
							++tIter;
						}
					}
				}
			}
			
			if( bKeepToken )
			{
				tToken.uAfterTokenRow = uCurrentRow;
				tToken.uAfterTokenColumn = uCurrentCol + tToken.uLength;
				tToken.pszToken = pszInputString;
				rsPossibleTokens.push_back( tToken );
			}
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

const char* GetTokenString( EShaderToken eToken )
{
	if( eToken <= EShaderToken_BeginBasic && eToken >= EShaderToken_EndBasic )
	{
		return g_asBasicTokens[ GetIndexFromRange( eToken, EShaderToken_BeginBasic ) ].pszTokenString;
	}

	return "???";
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
	g_tIgnoreTokens[ EShaderToken_Binary_Bitwise_And ].push_back( EShaderToken_Binary_Bitwise_Lazy_And );
	g_tIgnoreTokens[ EShaderToken_Binary_Bitwise_Or ].push_back( EShaderToken_Binary_Bitwise_Lazy_Or );
	g_tIgnoreTokens[ EShaderToken_Binary_Operator_Multiply ].push_back( EShaderToken_Assign_Multiply );
	g_tIgnoreTokens[ EShaderToken_Binary_Operator_Divide ].push_back( EShaderToken_Assign_Divide );
	g_tIgnoreTokens[ EShaderToken_Binary_Operator_Minus ].push_back( EShaderToken_Assign_Subtract );
	g_tIgnoreTokens[ EShaderToken_Binary_Operator_Plus ].push_back( EShaderToken_Assign_Add );
	g_tIgnoreTokens[ EShaderToken_Binary_Operator_Modulo ].push_back( EShaderToken_Assign_Modulo );
	g_tIgnoreTokens[ EShaderToken_Binary_Bitwise_Shift_Left ].push_back( EShaderToken_Assign_Shift_Left );
	g_tIgnoreTokens[ EShaderToken_Binary_Bitwise_Shift_Right ].push_back( EShaderToken_Assign_Shift_Right );
	g_tIgnoreTokens[ EShaderToken_Binary_Comparison_LT ].push_back( EShaderToken_Binary_Comparison_LTE );
	g_tIgnoreTokens[ EShaderToken_Binary_Comparison_LT ].push_back( EShaderToken_Assign_Shift_Left );
	g_tIgnoreTokens[ EShaderToken_Binary_Comparison_GT ].push_back( EShaderToken_Binary_Comparison_GTE );
	g_tIgnoreTokens[ EShaderToken_Binary_Comparison_GT ].push_back( EShaderToken_Assign_Shift_Right );
	g_tIgnoreTokens[ EShaderToken_Assign ].push_back( EShaderToken_Binary_Comparison_Equal );
	g_tIgnoreTokens[ EShaderToken_Unary_Not ].push_back( EShaderToken_Binary_Comparison_NotEqual );
	g_tIgnoreTokens[ EShaderToken_Binary_Operator_Plus ].push_back( EShaderToken_Unary_Increment );
	g_tIgnoreTokens[ EShaderToken_Binary_Operator_Minus ].push_back( EShaderToken_Unary_Decrement );

	//Special replacements
	// i +++ j => (i++) + j

	g_tIgnoreTokens[ EShaderToken_Special_TriplePlus ].push_back( EShaderToken_Unary_Increment );
	g_tIgnoreTokens[ EShaderToken_Special_TripleMinus ].push_back( EShaderToken_Unary_Decrement );

	//Set up operator precedence

	g_atTokenPrecedence[ EOperatorType_None ][ EShaderToken_Identifier ] = SPrecedence( 0 );
	g_atTokenPrecedence[ EOperatorType_None ][ EShaderToken_Parenthesis_Open ] = SPrecedence( 2 );
	g_atTokenPrecedence[ EOperatorType_None ][ EShaderToken_Parenthesis_Close ] = SPrecedence( INT_MAX );

	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Dot ] = SPrecedence( 2 );
	g_atTokenPrecedence[ EOperatorType_Unary ][ EShaderToken_Square_Open ] = SPrecedence( 2 );

	g_atTokenPrecedence[ EOperatorType_Unary ][ EShaderToken_Unary_Increment ] = SPrecedence( 3, EAssociativity_RightToLeft );
	g_atTokenPrecedence[ EOperatorType_Unary ][ EShaderToken_Unary_Decrement ] = SPrecedence( 3, EAssociativity_RightToLeft );
	g_atTokenPrecedence[ EOperatorType_Unary ][ EShaderToken_Binary_Operator_Plus ] = SPrecedence( 3, EAssociativity_RightToLeft );
	g_atTokenPrecedence[ EOperatorType_Unary ][ EShaderToken_Binary_Operator_Minus ] = SPrecedence( 3, EAssociativity_RightToLeft );
	g_atTokenPrecedence[ EOperatorType_Unary ][ EShaderToken_Unary_Not ] = SPrecedence( 3, EAssociativity_RightToLeft );
	g_atTokenPrecedence[ EOperatorType_Unary ][ EShaderToken_Unary_Bitwise_Not ] = SPrecedence( 3, EAssociativity_RightToLeft );

	//TODO: Cast

	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Operator_Multiply ] = SPrecedence( 5 );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Operator_Divide ] = SPrecedence( 5 );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Operator_Modulo ] = SPrecedence( 5 );

	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Operator_Plus ] = SPrecedence( 6 );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Operator_Minus ] = SPrecedence( 6 );

	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Bitwise_Shift_Left ] = SPrecedence( 7 );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Bitwise_Shift_Right ] = SPrecedence( 7 );

	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Comparison_LT ] = SPrecedence( 8 );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Comparison_LTE ] = SPrecedence( 8 );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Comparison_GT ] = SPrecedence( 8 );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Comparison_GTE ] = SPrecedence( 8 );

	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Comparison_Equal ] = SPrecedence( 9 );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Comparison_NotEqual ] = SPrecedence( 9 );

	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Bitwise_And ] = SPrecedence( 10 );

	//TODO: ^
	//g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Bitwise_Xor ] = SPrecedence( 11 );

	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Bitwise_Or ] = SPrecedence( 12 );

	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Bitwise_Lazy_And ] = SPrecedence( 13 );

	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Binary_Bitwise_Lazy_Or ] = SPrecedence( 14 );

	g_atTokenPrecedence[ EOperatorType_Ternary ][ EShaderToken_Ternary_QMark ] = SPrecedence( 15, EAssociativity_RightToLeft );
	g_atTokenPrecedence[ EOperatorType_Ternary ][ EShaderToken_Colon ] = SPrecedence( 16, EAssociativity_RightToLeft );

	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Assign ] = SPrecedence( 17, EAssociativity_RightToLeft );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Assign_Add ] = SPrecedence( 17, EAssociativity_RightToLeft );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Assign_Subtract ] = SPrecedence( 17, EAssociativity_RightToLeft );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Assign_Multiply ] = SPrecedence( 17, EAssociativity_RightToLeft );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Assign_Divide ] = SPrecedence( 17, EAssociativity_RightToLeft );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Assign_Modulo ] = SPrecedence( 17, EAssociativity_RightToLeft );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Assign_Shift_Left ] = SPrecedence( 17, EAssociativity_RightToLeft );
	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Assign_Shift_Right ] = SPrecedence( 17, EAssociativity_RightToLeft );

	//TODO
	//g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Assign_And ] = SPrecedence( 17, EAssociativity_RightToLeft );
	//g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Assign_Xor ] = SPrecedence( 17, EAssociativity_RightToLeft );
	//g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Assign_Or ] = SPrecedence( 17, EAssociativity_RightToLeft );

	g_atTokenPrecedence[ EOperatorType_Binary ][ EShaderToken_Comma ] = SPrecedence( 18 );
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

SPrecedence GetOperatorPrecedence( EOperatorType& reOperatorType, EShaderToken eToken, bool bForceUnary )
{
	for( unsigned int uType = 0; uType < EOperatorType_MAX; uType++ )
	{
		if( bForceUnary && uType != EOperatorType_Unary )
		{
			continue;
		}

		auto tIter = g_atTokenPrecedence[ uType ].find( eToken );
	
		if( tIter != g_atTokenPrecedence[ uType ].end() )
		{
			reOperatorType = (EOperatorType)uType;

			SPrecedence tReturn = (*tIter).second; 
			tReturn.iPrecedence = INT_MAX - (1000000 * tReturn.iPrecedence);

			return tReturn;
		}
	}

	return -1;
}

void ReportErrorOnAmbiguousToken( SParseContext& rtContext )
{
	if( rtContext.asPossibleTokens.size() > 1 && !rtContext.bIgnoreAmbiguity )
	{
		std::string tTokens;
		for( auto sToken : rtContext.asPossibleTokens )
		{
			tTokens += GetTokenName( sToken.eToken );
			tTokens += " ";
		}

		Error_Compiler( EError_Warning, rtContext.pszFilename, rtContext.uCurrentRow, rtContext.uCurrentCol, "Token ambiguity, could be: %s at %s", tTokens.c_str(), rtContext.GetLinePreview().c_str() );
	}
}

void ResolveTokenAmbiguity( SParseContext& rtContext, bool bExpectOperator )
{
	//If the user specified a specific token type to consume and we have an ambiguity
	//then we can use this hint to resolve it.
	if( rtContext.asPossibleTokens.size() > 1 )
	{
		int foundCount = 0;
		int foundIndex = 0;

		int foundNotOfCount = 0;
		int foundNotOfIndex = 0;

		int index = 0;
		for( auto sToken : rtContext.asPossibleTokens )
		{
			bool found = false;
			for( unsigned int uType = EOperatorType_Ternary; uType <= EOperatorType_Unary; uType++ )
			{
				auto tIter = g_atTokenPrecedence[ uType ].find( sToken.eToken );
	
				if( tIter != g_atTokenPrecedence[ uType ].end() )
				{
					found = true;
				}
			}

			if( found )
			{
				foundCount++;
				foundIndex = index;
			}
			else
			{
				foundNotOfCount++;
				foundNotOfIndex = index;
			}

			index++;
		}

		if( bExpectOperator )
		{
			if( foundCount == 0 )
			{
				Error_Compiler( EError_Error, rtContext.pszFilename, rtContext.uCurrentRow, rtContext.uCurrentCol, "Expected an operator at %s", rtContext.GetLinePreview().c_str() );
			}
			else if( foundCount == 1 )
			{
				rtContext.bIgnoreAmbiguity = true;

				rtContext.sNextToken = rtContext.asPossibleTokens[ foundIndex ];

				AdvanceToken( rtContext, rtContext.sNextToken );
			}
			else
			{
				ReportErrorOnAmbiguousToken( rtContext );
				Assert( foundCount <= 1, "Expected an operator, but was unable to resolve token ambiguity." );
			}
		}
		else
		{
			if( foundNotOfCount == 0 )
			{
				Error_Compiler( EError_Error, rtContext.pszFilename, rtContext.uCurrentRow, rtContext.uCurrentCol, "Did not expect an operator at %s", rtContext.GetLinePreview().c_str() );
			}
			else if( foundNotOfCount == 1 )
			{
				rtContext.bIgnoreAmbiguity = true;

				rtContext.sNextToken = rtContext.asPossibleTokens[ foundNotOfIndex ];

				AdvanceToken( rtContext, rtContext.sNextToken );
			}
			else
			{
				ReportErrorOnAmbiguousToken( rtContext );
				Assert( foundNotOfCount <= 1, "Did not expect an operator, and was unable to resolve token ambiguity." );
			}
		}

		
	}
}

void AdvanceToken( SParseContext& rtContext, SPossibleToken& rtToken )
{
	rtContext.pszBuffer += rtToken.uLength;
	rtContext.uCurrentRow = rtToken.uAfterTokenRow;
	rtContext.uCurrentCol = rtToken.uAfterTokenColumn;
	rtContext.uBytesLeft -= rtToken.uLength;
}

bool ConsumeToken( SParseContext& rtContext )
{
	ReportErrorOnAmbiguousToken( rtContext );

	rtContext.bIgnoreAmbiguity = false;
	rtContext.asPossibleTokens.clear();

	while( rtContext.uBytesLeft && GetPossibleTokens( rtContext.pszBuffer, rtContext.uBytesLeft, rtContext.uCurrentRow, rtContext.uCurrentCol, rtContext.asPossibleTokens ) )
	{
		SPossibleToken& rtToken = rtContext.asPossibleTokens[0];

		if( rtToken.eToken == EShaderToken_Whitespace || rtToken.eToken == EShaderToken_Comment )
		{
			AdvanceToken( rtContext, rtToken );

			rtContext.asPossibleTokens.clear();
			continue;
		}
		else
		{
			if( rtContext.asPossibleTokens.size() == 1 )
			{
				AdvanceToken( rtContext, rtToken );
			}

			//Don't advance if we have ambiguity

			rtContext.sNextToken = rtContext.asPossibleTokens[0];
			return true;
		}
	}

	if( rtContext.uBytesLeft )
	{
		Error_Compiler( EError_Error, rtContext.pszFilename, rtContext.uCurrentRow, rtContext.uCurrentCol, "Unable to parse at %s", rtContext.GetLinePreview().c_str() );
	}

	rtContext.sNextToken.eToken = EShaderToken_Invalid;
	return false;
}

bool TokenModifiesLeft( EShaderToken eToken )
{
	switch( eToken )
	{
		case EShaderToken_Unary_Increment:
		case EShaderToken_Unary_Decrement:
		case EShaderToken_Assign:
		case EShaderToken_Assign_Add:
		case EShaderToken_Assign_Subtract:
		case EShaderToken_Assign_Divide:
		case EShaderToken_Assign_Multiply:
		case EShaderToken_Assign_Modulo:
		case EShaderToken_Assign_Shift_Left:
		case EShaderToken_Assign_Shift_Right:
		case EShaderToken_Special_TriplePlus:
		case EShaderToken_Special_TripleMinus:
			return true;

		default:
			return false;
	}
}
