#if !defined( SHADR_AST_FUNCTION_H )
#define SHADR_AST_FUNCTION_H

class CASTPrototype : public CASTDefinition
{
public:

	CASTPrototype( const char* pszName, unsigned int uNameLength, const CType& rtReturnType )
	: CASTDefinition( rtReturnType )
	, m_tName( pszName, uNameLength )
	{}

	virtual llvm::Value* GenerateCode( CModule* pModule );

private:

	std::string m_tName;
};

class CASTFunction : public CASTDefinition
{
public:

	CASTFunction( CASTPrototype* pPrototype, CASTExpression* pBody )
	: CASTDefinition( pPrototype->GetType() )
	, m_pProrotype( pPrototype )
	, m_pBody( pBody )
	{}

	virtual llvm::Value* GenerateCode( CModule* pModule );

private:

	CASTPrototype* m_pProrotype;
	CASTExpression* m_pBody;
};

#endif //SHADR_AST_FUNCTION_H