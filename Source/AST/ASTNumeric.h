#if !defined( SHADR_AST_NUMERIC_H )
#define SHADR_AST_NUMERIC_H

#include <stdint.h>
#include "Tokens.h"

class CASTConstantInt : public CASTExpression
{
public:

	CASTConstantInt( const char* pszString, unsigned int uCharacters )
	: CASTExpression( CType::GetConstIntType(), EShaderToken_Int )
	, m_uValue( 0 )
	, m_uBits( 0 )
	, m_bSigned( 0 )
	{
		AddReflection( "ValueUnsigned", EASTReflectionType_UInt, &m_uValue );
		AddReflection( "ValueSigned", EASTReflectionType_Int, &m_iValue );

		AddCondition( "IsSigned", [&](){ return m_bSigned; } );

		ParseString( pszString, uCharacters );
		GetType().SetScalarType( m_bSigned ? EScalarType_Int : EScalarType_UnsignedInt);
	}

	const char* GetElementName() const { return "ConstInt"; }

	void ParseString( const char* pszString, unsigned int uCharacters );

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
	: CASTExpression( CType::GetConstFloatType(), EShaderToken_Float )
	, m_fValue( 0.0 )
	{
		AddReflection( "Value", EASTReflectionType_Double, &m_fValue );

		ParseString( pszString, uCharacters );
		GetType().SetScalarType( EScalarType_Double );
	}

	const char* GetElementName() const { return "ConstFloat"; }

	void ParseString( const char* pszString, unsigned int uCharacters );

private:

	double m_fValue;
};

class CASTConstantBool : public CASTExpression
{
public:

	CASTConstantBool( const char* pszString, unsigned int uCharacters )
	: CASTExpression( CType::GetConstFloatType(), EShaderToken_Float )
	, m_bValue( false )
	{
		AddReflection( "Value", EASTReflectionType_Bool, &m_bValue );

		if( strncmp( pszString, "true", uCharacters) == 0 || strncmp( pszString, "TRUE", uCharacters) == 0 )
		{
			m_bValue = true;
		}
		else if( strncmp( pszString, "false", uCharacters) == 0 || strncmp( pszString, "FALSE", uCharacters) == 0 )
		{
			m_bValue = false;
		}
		else
		{
			Assert( 0, "Invalid boolean value" );
		}
	}

	const char* GetElementName() const { return "ConstBool"; }

	void ParseString( const char* pszString, unsigned int uCharacters );

private:

	bool m_bValue;
};

class CASTVariableReference : public CASTExpression
{
public:

	CASTVariableReference( SVariable* pVariable )
	: CASTExpression( *pVariable->pType, EShaderToken_Identifier )
	, m_pVariable( pVariable )
	{
		AddReflection( "Variable", EASTReflectionType_Variable, &m_pVariable );
	}

	const char* GetElementName() const { return "VariableRef"; }

private:

	SVariable* m_pVariable;
};

#endif //SHADR_AST_NUMERIC_H