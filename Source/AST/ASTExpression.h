#if !defined( SHADR_AST_EXPRESSION_H )
#define SHADR_AST_EXPRESSION_H

class CASTExpression : public CASTBase
{
public:

	CASTExpression( const CType& rtType, EShaderToken eToken )
	: CASTBase()
	, m_tType( rtType )
	, m_eToken( eToken )
	{}

	const CType& GetType() const { return m_tType; }
	CType& GetType() { return m_tType; }

private:

	CType			m_tType;
	EShaderToken	m_eToken;
};

class CASTExpressionBinary : public CASTExpression
{
public:

	CASTExpressionBinary( EShaderToken eToken, CASTExpression* pLeft, CASTExpression* pRight )
	: CASTExpression( EvaluateType( pLeft, pRight ), eToken )
	, m_pLeft( pLeft )
	, m_pRight( pRight )
	, m_eOperator( eToken )
	{}

	//virtual llvm::Value* GenerateCode( CModule* pModule );

protected:

	CType EvaluateType( CASTExpression* pLeft, CASTExpression* pRight );

private:

	CASTExpression* m_pLeft;
	CASTExpression* m_pRight;
	EShaderToken	m_eOperator;
};

#endif //SHADR_AST_EXPRESSION_H