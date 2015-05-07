#include "CompilationUnit.h"
#include "AST.h"

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

void CASTConstantFloat::ParseString( const char* pszString, unsigned int uCharacters )
{
	m_fValue = strtod( pszString, NULL );
}
