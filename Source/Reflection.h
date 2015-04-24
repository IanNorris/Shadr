#if !defined( SHADR_REFLECTION_H )
#define SHADR_REFLECTION_H

enum EASTReflectionType
{
	EASTReflectionType_Token,
	EASTReflectionType_Variable,
	EASTReflectionType_Type,
	EASTReflectionType_TypeChild,
	EASTReflectionType_TypeScalar,
	EASTReflectionType_TypeSemantic,
	EASTReflectionType_TypeRegister,
	EASTReflectionType_ASTNode,
	EASTReflectionType_ASTNodeArray,
	EASTReflectionType_CString,
	EASTReflectionType_SString,
	EASTReflectionType_Double,
	EASTReflectionType_UInt,
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

	EASTReflectionType GetType() const { return m_eType; }

	template< class T >
	const T* GetData( ) const { return (T*)m_pData; }

private:

	void* m_pData;
	EASTReflectionType m_eType;
};

class CReflectionObject
{
public:

	virtual const char* GetElementName() const = 0;

	const CASTReflectionType* GetReflectionType( const std::string& rtName ) const
	{
		auto tIter = m_tReflection.find( rtName );
		if( tIter != m_tReflection.end() )
		{
			return (*tIter).second;
		}
		
		return nullptr;
	}

	template< class T>
	const T* GetReflectedData( const std::string& rtName ) const
	{
		auto tIter = m_tReflection.find( rtName );
		if( tIter != m_tReflection.end() )
		{
			return (*tIter).second->GetData<T>();
		}

		Assert( 0 , "Reflected data %s requested from class that is not available.", rtName.c_str() );
		
		return nullptr;
	}

	template< class T>
	const T* GetReflectedData( const char* pszName ) const
	{
		auto tIter = m_tReflection.find( pszName );
		if( tIter != m_tReflection.end() )
		{
			return (*tIter).second->GetData<T>();
		}

		Assert( 0 , "Reflected data %s requested from class that is not available.", pszName );
		
		return nullptr;
	}

	bool GetConditionOutcome( const std::string& rtName ) const
	{
		auto tIter = m_tReflectionConditions.find( rtName );
		if( tIter != m_tReflectionConditions.end() )
		{
			return (*tIter).second();
		}

		Error_Linker( EError_Warning, "Condition %s not found on object type %s.\n", rtName.c_str(), GetElementName() );

		return false;
	}

protected:

	virtual ~CReflectionObject() {}

	const std::unordered_map< std::string, CASTReflectionType* >& GetReflection() const
	{
		return m_tReflection;
	}

	template< class T >
	void AddReflection( const char* pszName, EASTReflectionType eType, T* pData )
	{
		m_tReflection[ pszName ] = new CASTReflectionType( eType, (void*)pData );
	}

	void AddCondition( const char* pszName, const std::function< bool(void) >& rtFunction )
	{
		m_tReflectionConditions[ pszName ] = rtFunction;
	}

private:
	
	std::unordered_map< std::string, CASTReflectionType* > m_tReflection;
	std::unordered_map< std::string, std::function< bool(void) > > m_tReflectionConditions;
};


#endif //SHADR_REFLECTION_H