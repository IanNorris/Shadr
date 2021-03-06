#if !defined( SHADR_TOKEN_H )
#define SHADR_TOKEN_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <regex>
#include <string>

#include "Utility/Error.h"

const char* GetCurrentFilename();

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
	EShaderToken_Unary_Bitwise_Not			= -20,
	EShaderToken_Unary_Increment			= -21,
	EShaderToken_Unary_Decrement			= -22,

	EShaderToken_Assign						= -23,
	EShaderToken_Assign_Add					= -24,
	EShaderToken_Assign_Subtract			= -25,
	EShaderToken_Assign_Divide				= -26,
	EShaderToken_Assign_Multiply			= -27,
	EShaderToken_Assign_Modulo				= -28,
	EShaderToken_Assign_Shift_Left			= -29,
	EShaderToken_Assign_Shift_Right			= -30,

	EShaderToken_SemiColon					= -31,
	EShaderToken_Colon						= -32,
	EShaderToken_Dot						= -33,

	EShaderToken_Return						= -34,
	EShaderToken_If							= -35,
	EShaderToken_Else						= -36,
	EShaderToken_For						= -37,
	EShaderToken_Do							= -38,
	EShaderToken_While						= -39,
	EShaderToken_Const						= -40,
	EShaderToken_Struct						= -41,
	EShaderToken_Constant_Buffer			= -42,
	EShaderToken_Include					= -43,
	EShaderToken_Buffer						= -44,

	EShaderToken_Modifier_Precision_Full	= -45,
	EShaderToken_Modifier_Precision_Half	= -46,

	EShaderToken_Modifier_Precision_UNorm	= -47,
	EShaderToken_Modifier_Precision_SNorm	= -48,

	EShaderToken_Feature					= -49,
	EShaderToken_Alias_Weak					= -50,
	EShaderToken_Alias_Strict				= -51,

	EShaderToken_Parenthesis_Open			= -52,
	EShaderToken_Parenthesis_Close			= -53,
	EShaderToken_Brace_Open					= -54,
	EShaderToken_Brace_Close				= -55,
	EShaderToken_Square_Open				= -56,
	EShaderToken_Square_Close				= -57,

	EShaderToken_Ternary_QMark				= -58,

	EShaderToken_Special_TriplePlus			= -59,
	EShaderToken_Special_TripleMinus		= -60,

	EShaderToken_In							= -61,
	EShaderToken_Out						= -62,
	EShaderToken_Intrinsic					= -63,
	EShaderToken_Inline						= -64,

	//Annotations
	EShaderToken_Annotation_SideEffect		= -65,
	EShaderToken_Annotation_Unroll			= -66,
	EShaderToken_Annotation_ForceInline		= -67,

	EShaderToken_EndBasic					= -67,

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

struct SParsePosition
{
	SParsePosition( const char* pszInputString )
	: pszFilename( GetCurrentFilename() )
	, pszBuffer( pszInputString )
	, uBytesLeft( strlen( pszInputString ) )
	, uCurrentRow( 0 )
	, uCurrentCol( 0 )
	{}

	SParsePosition()
	: pszFilename( GetCurrentFilename() )
	, pszBuffer( nullptr )
	, uBytesLeft( 0 )
	, uCurrentRow( 0 )
	, uCurrentCol( 0 )
	{}

	const char* pszFilename;

	const char* pszBuffer;
	unsigned int uBytesLeft;
	unsigned int uCurrentRow;
	unsigned int uCurrentCol;
};

struct SParseContext : public SParsePosition
{
	SParseContext( const char* pszInputString, CCompilationUnit* pCU )
	: SParsePosition( pszInputString )
	, uFlags( 0 )
	, pCompilationUnit( pCU )
	, bIgnoreAmbiguity( false )
	{}

	
	unsigned int uFlags;

	bool		bIgnoreAmbiguity;

	void SetFlag( EParseFlag eFlag ) { uFlags |= eFlag; }
	void UnsetFlag( EParseFlag eFlag ) { uFlags &= ~eFlag; }
	bool IsFlagSet( EParseFlag eFlag ) { return (uFlags & eFlag) != 0; }

	std::string GetLinePreview()
	{
		std::string tShortString( pszBuffer, 30 );
		size_t uPos = tShortString.find_first_of( '\n' );
		if( uPos != std::string::npos )
		{
			tShortString = tShortString.substr( 0, uPos );
		}

		return tShortString;
	}

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

void ReportErrorOnAmbiguousToken( SParseContext& rtContext );
void ResolveTokenAmbiguity( SParseContext& rtContext, bool bExpectOperator );
void AdvanceToken( SParseContext& rtContext, SPossibleToken& rtToken );
bool ConsumeToken( SParseContext& rtContext );

bool TokenModifiesLeft( EShaderToken eToken );

#endif //SHADR_TOKEN_H