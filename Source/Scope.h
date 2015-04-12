#if !defined( SHADR_SCOPE_H )
#define SHADR_SCOPE_H

#include "Utility/Error.h"

enum EVariableFlag
{
	EVariableFlag_Read			= 1 << 0,
	EVariableFlag_Written		= 1 << 1,
};

struct SVariable
{
	SVariable()
	: pType( nullptr )
	, uFlags( 0 )
	{}

	static SVariable* CreateDummyVariable()
	{
		SVariable* pVariable = new SVariable();
		pVariable->pType = new CType( "dummy", EScalarType_Dummy );

		return pVariable;
	}

	CType* pType;
	unsigned int uFlags;
};

class CScope
{
public:

	CScope( CScope* pParent )
	: m_pParentScope( pParent )
	{}

	SVariable* FindVariable( const std::string& rtName )
	{
		auto tIter = m_pVariables.find( rtName );

		if( tIter != m_pVariables.end() )
		{
			return (*tIter).second;
		}

		if( m_pParentScope )
		{
			return m_pParentScope->FindVariable( rtName );
		}

		return nullptr;
	}

	void AddVariable( SParseContext& rtContext, const std::string& rtName, SVariable* pVariable )
	{
		auto tIter = m_pVariables.find( rtName );
		if( tIter != m_pVariables.end() )
		{
			if( (*tIter).second->pType->GetScalarType() != EScalarType_Dummy )
			{
				Error_Compiler( EError_Error, rtContext.uCurrentRow, rtContext.uCurrentCol, "Identifier %s is already defined in this scope.", rtName.c_str() );
			}
		}

		m_pVariables[ rtName ] = pVariable;
	}

private:

	CScope*											m_pParentScope;
	std::unordered_map< std::string, SVariable* >	m_pVariables;
};

#endif //SHADR_SCOPE_H
