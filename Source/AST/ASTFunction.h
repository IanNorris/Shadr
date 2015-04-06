#if !defined( SHADR_AST_FUNCTION_H )
#define SHADR_AST_FUNCTION_H

class CASTPrototype : public CASTDefinition
{
public:

	CASTPrototype( const char* pszName, unsigned int uNameLength, const CType& rtReturnType )
	: CASTDefinition()
	, m_tReturnType( rtReturnType )
	, m_tName( pszName, uNameLength )
	{}

	//virtual llvm::Value* GenerateCode( CModule* pModule );

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

class CASTFunction : public CASTDefinition
{
public:

	CASTFunction( CASTPrototype* pPrototype, CASTBlockStatement* pBody )
	: CASTDefinition()
	, m_pPrototype( pPrototype )
	, m_pBody( pBody )
	{}

	//virtual llvm::Value* GenerateCode( CModule* pModule );

private:

	CASTPrototype* m_pPrototype;
	CASTBlockStatement* m_pBody;
};

#endif //SHADR_AST_FUNCTION_H