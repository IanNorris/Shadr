#if !defined( SHADR_AST_EXPRESSION_H )
#define SHADR_AST_EXPRESSION_H

class CASTExpression : public CASTBase
{
public:

	CASTExpression( const CType& rtType )
	: CASTBase()
	, m_tType( rtType )
	{}

	const CType& GetType() const { return m_tType; }
	CType& GetType() { return m_tType; }

private:

	CType			m_tType;
};

class CASTExpressionParen : public CASTExpression
{
public:

	CASTExpressionParen( CASTExpression* pExpression )
	: CASTExpression( pExpression->GetType() )
	, m_pExpression( pExpression )
	{
		AddReflection( "Expression", EASTReflectionType_ASTNode, &m_pExpression );
	}

	const char* GetElementName() const { return "ExpressionParen"; }

private:

	CASTExpression* m_pExpression;
	EShaderToken	m_eOperator;
	bool			m_bPre;
};

class CASTExpressionUnary : public CASTExpression
{
public:

	CASTExpressionUnary( EShaderToken eToken, bool bPre, CASTExpression* pExpression )
	: CASTExpression( EvaluateType( pExpression ) )
	, m_pExpression( pExpression )
	, m_eOperator( eToken )
	, m_bPre( bPre )
	{
		AddReflection( "Expression", EASTReflectionType_ASTNode, &m_pExpression );
		AddReflection( "Token", EASTReflectionType_Token, &m_eOperator );
		
		AddCondition( "IsPre", [&](){ return m_bPre; } );
	}

	const char* GetElementName() const { return "UnaryOp"; }

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
	: CASTExpression( EvaluateType( pLeft, pRight ) )
	, m_pLeft( pLeft )
	, m_pRight( pRight )
	, m_eOperator( eToken )
	{
		AddReflection( "Left", EASTReflectionType_ASTNode, &m_pLeft );
		AddReflection( "Right", EASTReflectionType_ASTNode, &m_pRight );
		AddReflection( "Token", EASTReflectionType_Token, &m_eOperator );

		AddCondition( "IsTightlyBound", [&]()
		{ 
			return m_eOperator == EShaderToken_Dot;
		} );
	}

	const char* GetElementName() const { return "BinaryOp"; }

protected:

	CType EvaluateType( CASTExpression* pLeft, CASTExpression* pRight );

private:

	CASTExpression* m_pLeft;
	CASTExpression* m_pRight;
	EShaderToken	m_eOperator;
};

class CASTExpressionTernary : public CASTExpression
{
public:

	CASTExpressionTernary( CASTExpression* pCondition, CASTExpression* pTrue, CASTExpression* pFalse )
	: CASTExpression( EvaluateType( pCondition, pTrue, pFalse ) )
	, m_pCondition( pCondition )
	, m_pTrue( pTrue )
	, m_pFalse( pFalse )
	{
		AddReflection( "Condition", EASTReflectionType_ASTNode, &m_pCondition );
		AddReflection( "TrueBranch", EASTReflectionType_ASTNode, &m_pTrue );
		AddReflection( "FalseBranch", EASTReflectionType_ASTNode, &m_pFalse );
	}

	const char* GetElementName() const { return "TernaryOp"; }

protected:

	CType EvaluateType( CASTExpression* pCondition, CASTExpression* pTrue, CASTExpression* pFalse );

private:

	CASTExpression* m_pCondition;
	CASTExpression* m_pTrue;
	CASTExpression* m_pFalse;
};

class CASTExpressionSwizzleMask : public CASTExpression
{
public:

	CASTExpressionSwizzleMask( const std::string& rtSwizzle )
	: CASTExpression( CType::GetVoidType() )
	, m_tSwizzle( rtSwizzle )
	{
		AddReflection( "Mask", EASTReflectionType_SString, &m_tSwizzle );
	}

	const char* GetElementName() const { return "Swizzle"; }

private:

	std::string m_tSwizzle;
};

class CASTExpressionMemberAccess : public CASTExpression
{
public:

	CASTExpressionMemberAccess( const std::string& rtIdentifier )
	: CASTExpression( CType::GetVoidType() )
	, m_tIdentifier( rtIdentifier )
	{
		AddReflection( "Identifier", EASTReflectionType_SString, &m_tIdentifier );
	}

	const char* GetElementName() const { return "MemberAccess"; }

private:

	std::string m_tIdentifier;
};

#endif //SHADR_AST_EXPRESSION_H