#if !defined( SHADR_AST_H )
#define SHADR_AST_H

#include "Tokens.h"
#include "Type.h"
#include "Scope.h"
#include "Utility/Error.h"

enum EASTReflectionType
{
	EASTReflectionType_Token,
	EASTReflectionType_Variable,
	EASTReflectionType_Type,
	EASTReflectionType_ASTNode,
	EASTReflectionType_ASTNodeArray,
	EASTReflectionType_CString,
	EASTReflectionType_SString,
	EASTReflectionType_Double,
	EASTReflectionType_Int,
	EASTReflectionType_Bool,
};

class CASTReflectionType
{
public:

	CASTReflectionType( EASTReflectionType eType, void* pData )
	: m_pData( pData )
	, m_eType( eType )
	{}

	EASTReflectionType GetType() { return m_eType; }

	template< class T >
	T* GetData( ) { return (T*)m_pData; }

private:

	void* m_pData;
	EASTReflectionType m_eType;
};

class CASTScope
{
public:

	CASTScope( CScope* pScope )
	: m_tScope( pScope )
	{}

	CScope& GetScope() { return m_tScope; }
	const CScope& GetScope() const { return m_tScope; }

private:
	
	CScope							m_tScope;
};

class CASTBase
{
public:

	CASTBase()
	{}

	virtual ~CASTBase() {}

	virtual const char* GetElementName() = 0;

	const std::unordered_map< std::string, CASTReflectionType* >& GetReflection() const
	{
		return m_tReflection;
	}

	template< class T >
	void AddReflection( const char* pszName, EASTReflectionType eType, T* pData )
	{
		m_tReflection[ pszName ] = new CASTReflectionType( eType, (void*)pData );
	}

	template< class T>
	T* GetReflectedData( const std::string& rtName )
	{
		return GetReflectedData<T>( rtName.c_str() );
	}

	template< class T>
	T* GetReflectedData( const char* pszName )
	{
		auto tIter = m_tReflection.find( pszName );
		if( tIter != m_tReflection.end() )
		{
			return (*tIter).second->GetData<T>();
		}

		Assert( 0 , "Reflected data %s requested from class that is not available.", pszName );
		
		return nullptr;
	}
	
	std::unordered_map< std::string, CASTReflectionType* > m_tReflection;
};

class CASTProgram : public CASTBase, public CASTScope
{
public:

	CASTProgram()
	: CASTBase()
	, CASTScope( NULL )
	{
		AddReflection( "Elements", EASTReflectionType_ASTNodeArray, &m_apElements );
	}

	const char* GetElementName() { return "Program"; }

	void AddElement( CASTBase* pElement ) { m_apElements.push_back( pElement ); }

private:

	std::vector< CASTBase* > m_apElements;
};

#include "ASTExpression.h"
#include "ASTStatement.h"
#include "ASTNumeric.h"
#include "ASTFunction.h"

#endif //SHADR_AST_H