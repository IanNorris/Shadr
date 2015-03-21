#include "LLVM.h"
#include "AST.h"

/*#if defined( _WIN32 )
#include <intrin.h>
#pragma intrinsic(_BitScanReverse)
#else

#endif*/

void CASTConstantInt::ParseString( const char* pszString, unsigned int uCharacters )
{
	llvm::StringRef str( pszString, uCharacters );

	if( pszString[ 0 ] == '-' )
	{
		m_iValue = strtoll( pszString, nullptr, 0 );
		m_bSigned = true;
	}
	else
	{
		m_iValue = strtoull( pszString, nullptr, 0 );
		m_bSigned = false;
	}

	unsigned long uHighestBit = 0;

/*#if defined( _WIN32 )
	m_uBits = _BitScanReverse( &uHighestBit, m_uValue );
#else
	m_uBits = 64 - __builtin_clzll( m_uValue );
#endif

	m_uBits = max( m_uBits, 4 );*/
	m_uBits = 64;
}

llvm::Value* CASTConstantInt::GenerateCode()
{
	return llvm::ConstantInt::get( llvm::getGlobalContext(), llvm::APInt( m_uBits, m_uValue, m_bSigned ));
}

void CASTConstantFloat::ParseString( const char* pszString, unsigned int uCharacters )
{
	m_fValue = strtod( pszString, NULL );
}

llvm::Value* CASTConstantFloat::GenerateCode()
{
	return llvm::ConstantFP::get( llvm::getGlobalContext(), llvm::APFloat( m_fValue ) );
}