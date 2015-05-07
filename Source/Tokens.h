#if !defined( SHADR_TOKEN_H )
#define SHADR_TOKEN_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <regex>
#include <string>

#include "Utility/Error.h"

class CCompilationUnit;

enum EOperatorType
{
	EOperatorType_None,

	//Order is inverted so +/- only show up as binary unless explicitly asked for as unary.

	EOperatorType_Ternary,
	EOperatorType_Binary,
	EOperatorType_Unary,

	EOperatorType_MAX
};

enum EAssociativity
{
	EAssociativity_LeftToRight,
	EAssociativity_RightToLeft,

	EAssociativity_MAX,
};

struct SPrecedence
{
	SPrecedence( int iPrec, EAssociativity eAssoc = EAssociativity_LeftToRight )
	: iPrecedence( iPrec )
	, eAssociativity( eAssoc )
	{}

	SPrecedence()
	: iPrecedence( -1 )
	, eAssociativity( EAssociativity_LeftToRight )
	{}

	int iPrecedence;
	EAssociativity eAssociativity;
};

//Positive tokens are reserved for system values such as identifiers
enum EShaderToken
{
	//Basic tokens
	EShaderToken_BeginBasic					= -1,

	EShaderToken_Comma						= -1,

	EShaderToken_Binary_Bitwise_Shift_Left	= -2,
	EShaderToken_Binary_Bitwise_Shift_Right	= -3,
	EShaderToken_Binary_Bitwise_Or			= -4,
	EShaderToken_Binary_Bitwise_And			= -5,
	EShaderToken_Binary_Bitwise_Lazy_Or		= -6,
	EShaderToken_Binary_Bitwise_Lazy_And	= -7,
	EShaderToken_Binary_Operator_Plus		= -8,
	EShaderToken_Binary_Operator_Minus		= -9,
	EShaderToken_Binary_Operator_Divide		= -10,
	EShaderToken_Binary_Operator_Multiply	= -11,
	EShaderToken_Binary_Operator_Modulo		= -12,
	EShaderToken_Binary_Comparison_LT		= -13,
	EShaderToken_Binary_Comparison_GT		= -14,
	EShaderToken_Binary_Comparison_LTE		= -15,
	EShaderToken_Binary_Comparison_GTE		= -16,
	EShaderToken_Binary_Comparison_Equal	= -17,
	EShaderToken_Binary_Comparison_NotEqual	= -18,

	EShaderToken_Unary_Not					= -19,
	EShaderToken_Unary_Increment			= -20,
	EShaderToken_Unary_Decrement			= -21,

	EShaderToken_Assign						= -22,
	EShaderToken_Assign_Add					= -23,
	EShaderToken_Assign_Subtract			= -24,
	EShaderToken_Assign_Divide				= -25,
	EShaderToken_Assign_Multiply			= -26,
	EShaderToken_Assign_Modulo				= -27,
	EShaderToken_Assign_Shift_Left			= -28,
	EShaderToken_Assign_Shift_Right			= -29,

	EShaderToken_SemiColon					= -30,
	EShaderToken_Colon						= -31,
	EShaderToken_Dot						= -32,

	EShaderToken_Return						= -33,
	EShaderToken_If							= -34,
	EShaderToken_Else						= -35,
	EShaderToken_For						= -36,
	EShaderToken_Do							= -37,
	EShaderToken_While						= -38,
	EShaderToken_Const						= -39,
	EShaderToken_Struct						= -40,
	EShaderToken_Constant_Buffer			= -41,
	EShaderToken_Include					= -42,
	EShaderToken_Buffer						= -43,

	EShaderToken_Modifier_Precision_Full	= -44,
	EShaderToken_Modifier_Precision_Half	= -45,

	EShaderToken_Modifier_Precision_UNorm	= -46,
	EShaderToken_Modifier_Precision_SNorm	= -47,

	EShaderToken_Feature					= -48,
	EShaderToken_Alias_Weak					= -49,
	EShaderToken_Alias_Strict				= -50,

	EShaderToken_Parenthesis_Open			= -51,
	EShaderToken_Parenthesis_Close			= -52,
	EShaderToken_Brace_Open					= -53,
	EShaderToken_Brace_Close				= -54,
	EShaderToken_Square_Open				= -55,
	EShaderToken_Square_Close				= -56,

	EShaderToken_Ternary_QMark				= -57,

	EShaderToken_Special_TriplePlus			= -58,
	EShaderToken_Special_TripleMinus		= -59,

	EShaderToken_EndBasic					= -59,

	//Callback based tokens
	EShaderToken_BeginCallback				= -1000,

	EShaderToken_Whitespace					= -1000,
	EShaderToken_Comment					= -1001,

	EShaderToken_EndCallback				= -1001,

	//Regex based tokens
	EShaderToken_BeginRegex					= -2000,

	EShaderToken_Float						= -2000,
	EShaderToken_Int						= -2001,
	EShaderToken_Boolean					= -2002,
	EShaderToken_Identifier					= -2003,

	EShaderToken_EndRegex					= -2003,

	//Special tokens
	EShaderToken_BeginSpecial				= -3000,

	EShaderToken_EOF						= -3000,

	EShaderToken_EndSpecial					= -3000,

	EShaderToken_Invalid					= 0x7fffffff,
};

#define GetIndexFromRange( iIndex, iBegin )		(-(iIndex - iBegin))
#define GetCountFromTokenRange( iBegin, iEnd )	((-iEnd - -iBegin)+1)

struct SPossibleToken
{
	SPossibleToken()
	: eToken( EShaderToken_Invalid )
	, uLength( 0 )
	, uAfterTokenRow( 0 )
	, uAfterTokenColumn( 0 )
	, pszToken( NULL )
	{}

	EShaderToken eToken;
	unsigned int uLength;
	unsigned int uAfterTokenRow;
	unsigned int uAfterTokenColumn;

	const char* pszToken;

	bool operator < ( const SPossibleToken& other ) const
	{
		return ( other.uLength < uLength );
	}
};

enum EParseFlag
{
	EParseFlag_RejectComma		= 1 << 0,
};

struct SParseContext
{
	SParseContext( const char* pszInputString, CCompilationUnit* pCU )
	: pszBuffer( pszInputString )
	, uBytesLeft( strlen( pszInputString ) )
	, uCurrentRow( 0 )
	, uCurrentCol( 0 )
	, uFlags( 0 )
	, pCompilationUnit( pCU )
	{}

	const char* pszBuffer;
	unsigned int uBytesLeft;
	unsigned int uCurrentRow;
	unsigned int uCurrentCol;
	unsigned int uFlags;

	void SetFlag( EParseFlag eFlag ) { uFlags |= eFlag; }
	void UnsetFlag( EParseFlag eFlag ) { uFlags &= ~eFlag; }
	bool IsFlagSet( EParseFlag eFlag ) { return (uFlags & eFlag) != 0; }

	SPossibleToken sNextToken;

	std::vector<SPossibleToken> asPossibleTokens;

	CCompilationUnit* pCompilationUnit;
};

bool GetPossibleTokens( const char* pszInputString, unsigned int uCharactersLeft, unsigned int uCurrentRow, unsigned int uCurrentCol, std::vector<SPossibleToken>& rsPossibleTokens );

const char* GetTokenName( EShaderToken eToken );
const char* GetTokenString( EShaderToken eToken );

EShaderToken GetTokenByName( const char* pszTokenName );

void InitialiseTokenTables( void );

void FilterTokens( std::vector<SPossibleToken>& rsPossibleTokens );

SPrecedence GetOperatorPrecedence( EOperatorType& reOperatorType, EShaderToken eToken, bool bForceUnary );

bool ConsumeToken( SParseContext& rtContext );

#endif //SHADR_TOKEN_H