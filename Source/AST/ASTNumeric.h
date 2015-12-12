#if !defined( SHADR_AST_NUMERIC_H )
#define SHADR_AST_NUMERIC_H

#include <stdint.h>
#include "Tokens.h"

class CASTConstantInt : public CASTExpression
{
public:

	CASTConstantInt( const SParsePosition& rtParsePosition, const char* pszString, unsigned int uCharacters )
	: CASTExpression( rtParsePosition, CType::GetConstIntType(), false )
	, m_uValue( 0 )
	, m_uBits( 0 )
	, m_bSigned( 0 )
	{
		AddReflection( "ValueUnsigned", EASTReflectionType_UInt, &m_uValue );
		AddReflection( "ValueSigned", EASTReflectionType_Int, &m_iValue );

		AddCondition( "IsSigned", [&](){ return m_bSigned; } );

		ParseString( pszString, uCharacters );

		if( m_iValue >= UINT_MAX )
		{
			Error_Compiler( EError_Error, rtParsePosition.pszFilename, rtParsePosition.uCurrentRow, rtParsePosition.uCurrentCol, "Numeric value %s is larger than a uint can hold", std::string(pszString, uCharacters).c_str() );
		}
		else if( m_iValue <= -INT_MAX )
		{
			Error_Compiler( EError_Error, rtParsePosition.pszFilename, rtParsePosition.uCurrentRow, rtParsePosition.uCurrentCol, "Numeric value %s is smaller than an int can hold", std::string(pszString, uCharacters).c_str() );
		}

		if( m_iValue > INT_MAX && m_iValue < UINT_MAX )
		{
			GetType().SetScalarType( EScalarType_UnsignedInt );
		}
		else
		{
			GetType().SetScalarType( EScalarType_Int );
		}
	}

	CASTConstantInt( const SParsePosition& rtParsePosition, uint64_t uValue, unsigned int uBits, bool bSigned )
	: CASTExpression( rtParsePosition, CType::GetConstIntType(), false )
	, m_uValue( uValue )
	, m_uBits( uBits )
	, m_bSigned( bSigned )
	{
		AddReflection( "ValueUnsigned", EASTReflectionType_UInt, &m_uValue );
		AddReflection( "ValueSigned", EASTReflectionType_Int, &m_iValue );

		AddCondition( "IsSigned", [&](){ return m_bSigned; } );

		if( m_iValue > INT_MAX && m_iValue < UINT_MAX )
		{
			GetType().SetScalarType( EScalarType_UnsignedInt );
		}
		else
		{
			GetType().SetScalarType( EScalarType_Int );
		}
	}

	const char* GetElementName() const { return "ConstInt"; }

	void ParseString( const char* pszString, unsigned int uCharacters );

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		return tChildren;
	}

	void EvaluateType() override
	{

	}

	virtual CASTExpression* ReplaceAndClone( std::vector< std::pair< CASTExpression*, CASTExpression* > >& expressionsToReplace, unsigned int uDepthToRecurse ) override
	{
		return new CASTConstantInt( GetParserPosition(), m_uValue, m_uBits, m_bSigned );
	}

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

	CASTConstantFloat( const SParsePosition& rtParsePosition, const char* pszString, unsigned int uCharacters )
	: CASTExpression( rtParsePosition, CType::GetConstFloatType(), false )
	, m_fValue( 0.0 )
	{
		AddReflection( "Value", EASTReflectionType_Double, &m_fValue );

		ParseString( pszString, uCharacters );
		GetType().SetScalarType( EScalarType_Float );
	}

	CASTConstantFloat( const SParsePosition& rtParsePosition, double fValue )
	: CASTExpression( rtParsePosition, CType::GetConstFloatType(), false )
	, m_fValue( fValue )
	{
		AddReflection( "Value", EASTReflectionType_Double, &m_fValue );

		GetType().SetScalarType( EScalarType_Float );
	}

	const char* GetElementName() const { return "ConstFloat"; }

	void ParseString( const char* pszString, unsigned int uCharacters );

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		return tChildren;
	}

	void EvaluateType() override
	{

	}

	virtual CASTExpression* ReplaceAndClone( std::vector< std::pair< CASTExpression*, CASTExpression* > >& expressionsToReplace, unsigned int uDepthToRecurse ) override
	{
		return new CASTConstantFloat( GetParserPosition(), m_fValue );
	}

private:

	double m_fValue;
};

class CASTConstantBool : public CASTExpression
{
public:

	CASTConstantBool( const SParsePosition& rtParsePosition, const char* pszString, unsigned int uCharacters )
	: CASTExpression( rtParsePosition, CType::GetConstFloatType(), false )
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

		GetType().SetScalarType( EScalarType_Bool );
	}

	CASTConstantBool( const SParsePosition& rtParsePosition, bool bValue )
	: CASTExpression( rtParsePosition, CType::GetConstFloatType(), false )
	, m_bValue( bValue )
	{
		AddReflection( "Value", EASTReflectionType_Bool, &m_bValue );

		GetType().SetScalarType( EScalarType_Bool );
	}

	const char* GetElementName() const { return "ConstBool"; }

	void ParseString( const char* pszString, unsigned int uCharacters );

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		return tChildren;
	}

	void EvaluateType() override
	{

	}

	virtual CASTExpression* ReplaceAndClone( std::vector< std::pair< CASTExpression*, CASTExpression* > >& expressionsToReplace, unsigned int uDepthToRecurse ) override
	{
		return new CASTConstantBool( GetParserPosition(), m_bValue );
	}

private:

	bool m_bValue;
};

class CASTVariableReference : public CASTExpression
{
public:

	CASTVariableReference( const SParsePosition& rtParsePosition, CASTVariable* pVariable )
	: CASTExpression( rtParsePosition, pVariable->GetType(), false )
	, m_pVariable( pVariable )
	{
		AddReflection( "Variable", EASTReflectionType_Variable, &m_pVariable );
	}

	const char* GetElementName() const { return "VariableRef"; }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.reserve(1);
		tChildren.push_back( m_pVariable );
		return tChildren;
	}

	void EvaluateType() override
	{
		m_tType = m_pVariable->GetType();
	}

	CASTVariable* GetVariable()
	{
		return m_pVariable;
	}

	virtual CASTExpression* ReplaceAndClone( std::vector< std::pair< CASTExpression*, CASTExpression* > >& expressionsToReplace, unsigned int uDepthToRecurse ) override
	{
		CASTExpression* pNewChild = ReplaceAndCloneSingle( m_pVariable, expressionsToReplace, uDepthToRecurse - 1 );
		CASTVariable* pNewChildAsVariable = dynamic_cast<CASTVariable*>(pNewChild);

		//Special case, if we previously has a reference to a variable yet we just replaced it with a non-variable expression then we should just return the child instead surrounded by parentheses
		if( pNewChildAsVariable )
		{
			return new CASTVariableReference( GetParserPosition(), pNewChildAsVariable );
		}
		else
		{
			return new CASTExpressionParen( GetParserPosition(), pNewChild );
		}
	}

private:

	CASTVariable* m_pVariable;
};

#endif //SHADR_AST_NUMERIC_H