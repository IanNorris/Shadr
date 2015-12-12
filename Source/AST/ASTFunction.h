#if !defined( SHADR_AST_FUNCTION_H )
#define SHADR_AST_FUNCTION_H

class CASTPrototype : public CASTBase, public CASTScope, public CASTAnnotationSupport
{
public:

	CASTPrototype( const SParsePosition& rtParsePosition, const char* pszName, unsigned int uNameLength, const CType& rtReturnType, CScope* pParentScope, bool bIntrinsic, bool bInline )
	: CASTBase( rtParsePosition )
	, CASTScope( pParentScope )
	, m_tReturnType( rtReturnType )
	, m_pReturnType( &m_tReturnType )
	, m_tName( pszName, uNameLength )
	, m_bIntrinsic( bIntrinsic )
	, m_bInline( bInline )
	{
		AddReflection( "ReturnType", EASTReflectionType_Type, &m_pReturnType );
		AddReflection( "Name", EASTReflectionType_SString, &m_tName );
		AddReflection( "Parameters", EASTReflectionType_ASTNodeArray, &m_apParameters );

		AddCondition( "HasParameters", [&](){ return !m_apParameters.empty(); } );
		AddCondition( "IsIntrinsic", [&](){ return m_bIntrinsic; } );
		AddCondition( "IsInline", [&](){ return m_bInline; } );
	}

	const char* GetElementName() const { return "Prototype"; }

	void AddParameter( CASTVariableDefinition* pParameter )
	{
		m_apParameters.push_back( pParameter );
	}

	const CType& GetReturnType() { return m_tReturnType; }

	const std::vector< CASTVariableDefinition* >& GetParameters() const { return m_apParameters; }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		for( auto& child : m_apParameters )
		{
			tChildren.push_back( child );
		}
		return tChildren;
	}

	const std::string& GetFunctionName() const { return m_tName; }

	bool CanFitParameterCount( unsigned int uParameterCount )
	{
		//TODO: Implement default parameters here.

		if( m_apParameters.size() == uParameterCount )
		{
			return true;
		}

		return false;
	}

	bool IsIntrinsic()
	{
		return m_bIntrinsic;
	}

	bool IsInline()
	{
		return m_bInline;
	}

private:

	CType*		m_pReturnType; //Workaround for the type expecting CType** in AddReflection
	CType		m_tReturnType;
	std::string m_tName;
	std::vector< CASTVariableDefinition* > m_apParameters;

	bool		m_bIntrinsic;
	bool		m_bInline;
};

class CASTFunction : public CASTBase, public CASTAnnotationSupport
{
public:

	CASTFunction( const SParsePosition& rtParsePosition, CASTPrototype* pPrototype, CASTBlockStatement* pBody )
	: CASTBase( rtParsePosition )
	, m_pPrototype( pPrototype )
	, m_pBody( pBody )
	, m_bInline( false )
	{
		AddReflection( "Prototype", EASTReflectionType_ASTNode, &m_pPrototype );
		AddReflection( "Body", EASTReflectionType_ASTNode, &m_pBody );
		AddReflection( "Inline", EASTReflectionType_Bool, &m_bInline );
	}

	const char* GetElementName() const { return "Function"; }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.push_back( m_pPrototype );
		tChildren.push_back( m_pBody );
		return tChildren;
	}

	bool IsInline( void )
	{
		return m_bInline;
	}

	CASTPrototype* GetPrototype() { return m_pPrototype; }

private:

	CASTPrototype* m_pPrototype;
	CASTBlockStatement* m_pBody;
	bool m_bInline;
};

class CASTExpressionFunctionCall : public CASTExpression
{
public:
	CASTExpressionFunctionCall( const SParsePosition& rtParsePosition, const std::string& rtName )
	: CASTExpression( rtParsePosition, CType::GetVoidType(), false )
	, m_tName( rtName )
	, m_pPrototype( nullptr )
	, m_pFunctionBody( nullptr )
	, m_bIntrinsic( false )
	{
		AddReflection( "Name", EASTReflectionType_SString, &m_tName );
		AddReflection( "Parameters", EASTReflectionType_ASTNodeArray, &m_apParameters );

		AddCondition( "HasParameters", [&](){ return !m_apParameters.empty(); } );
	}

	const std::string& GetFunctionName() const { return m_tName; }

	const char* GetElementName() const { return "FunctionCall"; }

	void AddParameter( CASTExpression* pParameter )
	{
		m_apParameters.push_back( pParameter );
	}

	const std::vector< CASTExpression* >& GetParameters() const { return m_apParameters; }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		for( auto& child : m_apParameters )
		{
			tChildren.push_back( child );
		}
		return tChildren;
	}

	bool FindMatchingFunction( CScope* pScope );
	bool FindMatchingFunctionBody( CScope* pScope );

	void EvaluateType() override
	{
		//TODO
		if( m_pPrototype )
		{
			m_tType = m_pPrototype->GetReturnType();
		}
		else
		{
			m_tType = CType::GetConstIntType();
		}
	}

	bool IsIntrinsic()
	{
		return m_bIntrinsic;
	}

	bool IsInline()
	{
		if( m_pPrototype )
		{
			return m_pPrototype->IsInline();
		}
		else
		{
			return false;
		}
	}

	CASTPrototype* GetPrototype()
	{
		return m_pPrototype;
	}

	CASTFunction* GetFunctionBody()
	{
		return m_pFunctionBody;
	}

	virtual CASTExpression* ReplaceAndClone( std::vector< std::pair< CASTExpression*, CASTExpression* > >& expressionsToReplace, unsigned int uDepthToRecurse ) override
	{
		auto pFC = new CASTExpressionFunctionCall( GetParserPosition(), m_tName );
		
		for( auto pParam : m_apParameters )
		{
			pFC->AddParameter( ReplaceAndCloneSingle( pParam, expressionsToReplace, uDepthToRecurse - 1 ) );
		}

		//Functions themselves should not be cloned
		pFC->m_pPrototype = m_pPrototype;
		pFC->m_pFunctionBody = m_pFunctionBody;
		pFC->m_bIntrinsic = m_bIntrinsic;

		return pFC;
	}

private:

	std::string m_tName;
	std::vector< CASTExpression* > m_apParameters;

	CASTPrototype* m_pPrototype;
	CASTFunction* m_pFunctionBody;

	bool			m_bIntrinsic;
};

#endif //SHADR_AST_FUNCTION_H