#if !defined( SHADR_AST_FUNCTION_H )
#define SHADR_AST_FUNCTION_H

class CASTPrototype : public CASTBase
{
public:

	CASTPrototype( const char* pszName, unsigned int uNameLength, const CType& rtReturnType )
	: CASTBase( rtReturnType )
	, m_tName( pszName, uNameLength )
	{}

	virtual llvm::Value* GenerateCode( CModule* pModule );

private:

	std::string m_tName;
};

class CASTFunction : public CASTBase
{
public:

	CASTFunction( CASTPrototype* pPrototype, CASTExpression* pBody )
	: CASTBase( pPrototype->GetType() )
	, m_pProrotype( pPrototype )
	, m_pBody( pBody )
	{}

	virtual llvm::Value* GenerateCode( CModule* pModule );

private:

	CASTPrototype* m_pProrotype;
	CASTExpression* m_pBody;
};

#endif //SHADR_AST_FUNCTION_H