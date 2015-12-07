#if !defined( SHADR_SCOPE_H )
#define SHADR_SCOPE_H

#include "Utility/Error.h"
#include "Reflection.h"

enum EVariableFlag
{
	EVariableFlag_Read			= 1 << 0,
	EVariableFlag_Written		= 1 << 1,
};

class CASTExpressionStatement;
class CASTPrototype;
class CASTFunction;

struct SVariable : public CReflectionObject
{
	SVariable()
	: pType( nullptr )
	, pAssignment( nullptr )
	, uFlags( 0 )
	{
		AddReflection( "Type", EASTReflectionType_Type, &pType );
		AddReflection( "Name", EASTReflectionType_SString, &tName );
		AddReflection( "Assignment", EASTReflectionType_ASTNode, &pAssignment );

		AddCondition( "HasAssignment", [&]() { return pAssignment != nullptr; } );
	}

	const char* GetElementName() const { return "Variable"; }

	static SVariable* CreateDummyVariable()
	{
		SVariable* pVariable = new SVariable();
		pVariable->pType = new CType( "dummy", EScalarType_Dummy );

		return pVariable;
	}

	std::string tName;

	CType* pType;

	CASTExpressionStatement* pAssignment;

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

	void FindPrototypes( const std::string& rtName, std::vector<CASTPrototype*>& prototypes )
	{
		auto range = m_pPrototypes.equal_range( rtName );

		for_each( range.first, range.second,
			[&prototypes]( std::unordered_multimap< std::string, CASTPrototype* >::value_type& x )
			{
				prototypes.push_back( x.second );
			}
		);
		
		if( m_pParentScope )
		{
			m_pParentScope->FindPrototypes( rtName, prototypes );
		}
	}

	void FindFunctions( const std::string& rtName, std::vector<CASTFunction*>& functions )
	{
		auto range = m_pFunctions.equal_range( rtName );

		for_each( range.first, range.second,
			[&functions]( std::unordered_multimap< std::string, CASTFunction* >::value_type& x )
			{
				functions.push_back( x.second );
			}
		);
		
		if( m_pParentScope )
		{
			m_pParentScope->FindFunctions( rtName, functions );
		}
	}

	void AddVariable( SParseContext& rtContext, SVariable* pVariable )
	{
		auto tIter = m_pVariables.find( pVariable->tName );
		if( tIter != m_pVariables.end() )
		{
			if( (*tIter).second->pType->GetScalarType() != EScalarType_Dummy )
			{
				Error_Compiler( EError_Error, rtContext.pszFilename, rtContext.uCurrentRow, rtContext.uCurrentCol, "Identifier %s is already defined in this scope.", pVariable->tName.c_str() );
			}
		}

		m_pVariables[ pVariable->tName ] = pVariable;
	}

	void AddPrototype( SParseContext& rtContext, CASTPrototype* pPrototype );

	void AddFunction( SParseContext& rtContext, CASTFunction* pFunction );

	CScope* GetParentScope() { return m_pParentScope; }

private:

	CScope*													m_pParentScope;
	std::unordered_map< std::string, SVariable* >			m_pVariables;
	std::unordered_multimap< std::string, CASTPrototype* >	m_pPrototypes;
	std::unordered_multimap< std::string, CASTFunction* >	m_pFunctions;
};

#endif //SHADR_SCOPE_H
