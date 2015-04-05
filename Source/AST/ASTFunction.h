#if !defined( SHADR_AST_FUNCTION_H )
#define SHADR_AST_FUNCTION_H

class CASTPrototype : public CASTDefinition
{
public:

	CASTPrototype( const char* pszName, unsigned int uNameLength, const CType& rtReturnType )
	: CASTDefinition( rtReturnType )
	, m_tName( pszName, uNameLength )
	{}

	//virtual llvm::Value* GenerateCode( CModule* pModule );

	void AddParameter( CASTVariableDefinition* pParameter )
	{
		m_apParameters.push_back( pParameter );
	}

	const std::vector< CASTVariableDefinition* >& GetParameters() const { return m_apParameters; }

private:

	std::string m_tName;
	std::vector< CASTVariableDefinition* > m_apParameters;
};

class CASTFunction : public CASTDefinition
{
public:

	CASTFunction( CASTPrototype* pPrototype, CASTExpression* pBody )
	: CASTDefinition( pPrototype->GetType() )
	, m_pProrotype( pPrototype )
	, m_pBody( pBody )
	{}

	//virtual llvm::Value* GenerateCode( CModule* pModule );

private:

	CASTPrototype* m_pProrotype;
	CASTExpression* m_pBody;
};

#endif //SHADR_AST_FUNCTION_H