#if !defined( SHADR_AST_FUNCTION_H )
#define SHADR_AST_FUNCTION_H

class CASTPrototype : public CASTBase, public CASTScope, public CASTAnnotationSupport
{
public:

	CASTPrototype( const SParsePosition& rtParsePosition, const char* pszName, unsigned int uNameLength, const CType& rtReturnType, CScope* pParentScope )
	: CASTBase( rtParsePosition )
	, CASTScope( pParentScope )
	, m_tReturnType( rtReturnType )
	, m_pReturnType( &m_tReturnType )
	, m_tName( pszName, uNameLength )
	{
		AddReflection( "ReturnType", EASTReflectionType_Type, &m_pReturnType );
		AddReflection( "Name", EASTReflectionType_SString, &m_tName );
		AddReflection( "Parameters", EASTReflectionType_ASTNodeArray, &m_apParameters );

		AddCondition( "HasParameters", [&](){ return !m_apParameters.empty(); } );
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

private:

	CType*		m_pReturnType; //Workaround for the type expecting CType** in AddReflection
	CType		m_tReturnType;
	std::string m_tName;
	std::vector< CASTVariableDefinition* > m_apParameters;
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

private:

	CASTPrototype* m_pPrototype;
	CASTBlockStatement* m_pBody;
	bool m_bInline;
};

class CASTExpressionFunctionCall : public CASTExpression
{
public:
	CASTExpressionFunctionCall( const SParsePosition& rtParsePosition, const std::string& rtName )
	: CASTExpression( rtParsePosition, CType::GetVoidType() )
	, m_tName( rtName )
	, m_pPrototype( nullptr )
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

private:

	std::string m_tName;
	std::vector< CASTExpression* > m_apParameters;

	CASTPrototype* m_pPrototype;
};

#endif //SHADR_AST_FUNCTION_H