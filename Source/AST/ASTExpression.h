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

class CASTExpressionUnary : public CASTExpression
{
public:

	CASTExpressionUnary( EShaderToken eToken, bool bPre, CASTExpression* pExpression )
	: CASTExpression( EvaluateType( pExpression ), eToken )
	, m_pExpression( pExpression )
	, m_eOperator( eToken )
	, m_bPre( bPre )
	{}

	//virtual llvm::Value* GenerateCode( CModule* pModule );

protected:

	CType EvaluateType( CASTExpression* pExpression );

private:

	CASTExpression* m_pExpression;
	EShaderToken	m_eOperator;
	bool			m_bPre;
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

class CASTExpressionSwizzleMask : public CASTExpression
{
public:

	CASTExpressionSwizzleMask( const std::string& rtSwizzle )
	: CASTExpression( CType::GetVoidType(), EShaderToken_Identifier )
	, m_tSwizzle( rtSwizzle )
	{}

	//virtual llvm::Value* GenerateCode( CModule* pModule );

private:

	std::string m_tSwizzle;
};

class CASTExpressionMemberAccess : public CASTExpression
{
public:

	CASTExpressionMemberAccess( const std::string& rtIdentifier )
	: CASTExpression( CType::GetVoidType(), EShaderToken_Identifier )
	, m_tIdentifier( rtIdentifier )
	{}

	//virtual llvm::Value* GenerateCode( CModule* pModule );

private:

	std::string m_tIdentifier;
};

#endif //SHADR_AST_EXPRESSION_H