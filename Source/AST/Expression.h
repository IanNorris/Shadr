#if !defined( SHADR_AST_EXPRESSION_H )
#define SHADR_AST_EXPRESSION_H

class CASTExpression : public CASTBase
{
public:

	CASTExpression( EShaderToken eToken )
	: CASTBase( eToken )
	{}
};

class CASTExpressionBinary : public CASTExpression
{
public:

	CASTExpressionBinary( EShaderToken eToken, CASTExpression* pLeft, CASTExpression* pRight )
	: CASTExpression( eToken )
	, m_pLeft( pLeft )
	, m_pRight( pRight )
	{}

	virtual llvm::Value* GenerateCode();

private:

	CASTExpression* m_pLeft;
	CASTExpression* m_pRight;
};

#endif //SHADR_AST_EXPRESSION_H