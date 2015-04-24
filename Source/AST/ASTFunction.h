#if !defined( SHADR_AST_FUNCTION_H )
#define SHADR_AST_FUNCTION_H

class CASTPrototype : public CASTBase, public CASTScope
{
public:

	CASTPrototype( const char* pszName, unsigned int uNameLength, const CType& rtReturnType, CScope* pParentScope )
	: CASTBase()
	, CASTScope( pParentScope )
	, m_tReturnType( rtReturnType )
	, m_tName( pszName, uNameLength )
	{
		AddReflection( "ReturnType", EASTReflectionType_Type, &m_tReturnType );
		AddReflection( "Name", EASTReflectionType_SString, &m_tName );
		AddReflection( "Parameters", EASTReflectionType_ASTNodeArray, &m_apParameters );
	}

	const char* GetElementName() const { return "Prototype"; }

	void AddParameter( CASTVariableDefinition* pParameter )
	{
		m_apParameters.push_back( pParameter );
	}

	const std::vector< CASTVariableDefinition* >& GetParameters() const { return m_apParameters; }

private:

	CType		m_tReturnType;
	std::string m_tName;
	std::vector< CASTVariableDefinition* > m_apParameters;
};

class CASTFunction : public CASTBase
{
public:

	CASTFunction( CASTPrototype* pPrototype, CASTBlockStatement* pBody )
	: CASTBase()
	, m_pPrototype( pPrototype )
	, m_pBody( pBody )
	{
		AddReflection( "Prototype", EASTReflectionType_ASTNode, m_pPrototype );
		AddReflection( "Body", EASTReflectionType_ASTNode, m_pBody );
	}

	const char* GetElementName() const { return "Function"; }

private:

	CASTPrototype* m_pPrototype;
	CASTBlockStatement* m_pBody;
};

class CASTExpressionFunctionCall : public CASTExpression
{
public:
	CASTExpressionFunctionCall( const std::string& rtName )
	: CASTExpression( CType::GetVoidType(), EShaderToken_Identifier )
	, m_tName( rtName )
	{
		AddReflection( "Name", EASTReflectionType_SString, &m_tName );
		AddReflection( "Parameters", EASTReflectionType_ASTNodeArray, &m_apParameters );
	}

	const char* GetElementName() const { return "FunctionCall"; }

	void AddParameter( CASTExpression* pParameter )
	{
		m_apParameters.push_back( pParameter );
	}

	const std::vector< CASTExpression* >& GetParameters() const { return m_apParameters; }

private:

	std::string m_tName;
	std::vector< CASTExpression* > m_apParameters;
};

#endif //SHADR_AST_FUNCTION_H