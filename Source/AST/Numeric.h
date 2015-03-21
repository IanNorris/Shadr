#if !defined( SHADR_AST_NUMERIC_H )
#define SHADR_AST_NUMERIC_H

#include "Tokens.h"

class CASTConstantInt : public CASTExpression
{
public:

	CASTConstantInt( const char* pszString, unsigned int uCharacters )
	: CASTExpression( EShaderToken_Int )
	, m_uValue( 0 )
	, m_uBits( 0 )
	, m_bSigned( 0 )
	{
		ParseString( pszString, uCharacters );
	}

	void ParseString( const char* pszString, unsigned int uCharacters );

	virtual llvm::Value* GenerateCode();

private:

	union
	{
		uint64_t		m_uValue;
		int64_t			m_iValue;
	};
	unsigned int		m_uBits;
	bool				m_bSigned;
};

class CASTConstantFloat : public CASTExpression
{
public:

	CASTConstantFloat( const char* pszString, unsigned int uCharacters )
	: CASTExpression( EShaderToken_Float )
	, m_fValue( 0.0 )
	{
		ParseString( pszString, uCharacters );
	}

	void ParseString( const char* pszString, unsigned int uCharacters );

	virtual llvm::Value* GenerateCode();

private:

	double m_fValue;
};

#endif //SHADR_AST_NUMERIC_H