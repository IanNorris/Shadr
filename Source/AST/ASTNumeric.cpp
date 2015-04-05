#include "CompilationUnit.h"
#include "AST.h"

/*#if defined( _WIN32 )
#include <intrin.h>
#pragma intrinsic(_BitScanReverse)
#else

#endif*/

void CASTConstantInt::ParseString( const char* pszString, unsigned int uCharacters )
{
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
}

/*llvm::Value* CASTConstantInt::GenerateCode( CModule* pModule )
{
	return llvm::ConstantInt::get( llvm::getGlobalContext(), llvm::APInt( m_uBits, m_uValue, m_bSigned ));
}*/

void CASTConstantFloat::ParseString( const char* pszString, unsigned int uCharacters )
{
	m_fValue = strtod( pszString, NULL );
}

/*llvm::Value* CASTConstantFloat::GenerateCode( CModule* pModule )
{
	return llvm::ConstantFP::get( llvm::getGlobalContext(), llvm::APFloat( m_fValue ) );
}*/