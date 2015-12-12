#if !defined( SHADR_AST_EXPRESSION_H )
#define SHADR_AST_EXPRESSION_H

class CASTExpression : public CASTBase
{
public:

	CASTExpression( const SParsePosition& rtParsePosition, const CType& rtType, bool bModifiesLeft )
	: CASTBase( rtParsePosition )
	, m_tType( rtType )
	, m_bModifiesLeft( bModifiesLeft )
	{}

	const CType& GetType() const { return m_tType; }
	CType& GetType() { return m_tType; }

	virtual void EvaluateType() = 0;

	//Does the operator modify its L-value?
	bool DoesModifyLeft() { return m_bModifiesLeft; }

protected:

	CType			m_tType;

	bool			m_bModifiesLeft;
};

class CASTExpressionParen : public CASTExpression
{
public:

	CASTExpressionParen( const SParsePosition& rtParsePosition, CASTExpression* pExpression )
	: CASTExpression( rtParsePosition, CType::GetVoidType(), false )
	, m_pExpression( pExpression )
	{
		AddReflection( "Expression", EASTReflectionType_ASTNode, &m_pExpression );
	}

	const char* GetElementName() const { return "ExpressionParen"; }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.push_back( m_pExpression );
		return tChildren;
	}

	void EvaluateType() override
	{
		m_tType = m_pExpression->GetType();
	}

private:

	CASTExpression* m_pExpression;
	EShaderToken	m_eOperator;
	bool			m_bPre;
};

class CASTExpressionUnary : public CASTExpression
{
public:

	CASTExpressionUnary( const SParsePosition& rtParsePosition, EShaderToken eToken, bool bPre, CASTExpression* pExpression )
	: CASTExpression( rtParsePosition, EvaluateType( pExpression ), TokenModifiesLeft( eToken ) )
	, m_pExpression( pExpression )
	, m_eOperator( eToken )
	, m_bPre( bPre )
	{
		AddReflection( "Expression", EASTReflectionType_ASTNode, &m_pExpression );
		AddReflection( "Token", EASTReflectionType_Token, &m_eOperator );
		
		AddCondition( "IsPre", [&](){ return m_bPre; } );
	}

	const char* GetElementName() const { return "UnaryOp"; }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.push_back( m_pExpression );
		return tChildren;
	}

	void EvaluateType() override
	{
		m_tType = EvaluateType( m_pExpression );
	}

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

	CASTExpressionBinary( const SParsePosition& rtParsePosition, EShaderToken eToken, CASTExpression* pLeft, CASTExpression* pRight )
	: CASTExpression( rtParsePosition, CType::GetVoidType(), TokenModifiesLeft( eToken ) )
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

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.push_back( m_pLeft );
		tChildren.push_back( m_pRight );
		return tChildren;
	}

	void EvaluateType() override
	{
		m_tType = EvaluateType( m_pLeft, m_pRight );
	}

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

	CASTExpressionTernary( const SParsePosition& rtParsePosition, CASTExpression* pCondition, CASTExpression* pTrue, CASTExpression* pFalse )
	: CASTExpression( rtParsePosition, CType::GetVoidType(), false )
	, m_pCondition( pCondition )
	, m_pTrue( pTrue )
	, m_pFalse( pFalse )
	{
		AddReflection( "Condition", EASTReflectionType_ASTNode, &m_pCondition );
		AddReflection( "TrueBranch", EASTReflectionType_ASTNode, &m_pTrue );
		AddReflection( "FalseBranch", EASTReflectionType_ASTNode, &m_pFalse );
	}

	const char* GetElementName() const { return "TernaryOp"; }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.push_back( m_pCondition );
		tChildren.push_back( m_pTrue );
		tChildren.push_back( m_pFalse );
		return tChildren;
	}

	void EvaluateType() override
	{
		m_tType = EvaluateType( m_pCondition, m_pTrue, m_pFalse );
	}

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

	CASTExpressionSwizzleMask( const SParsePosition& rtParsePosition, const std::string& rtSwizzle )
	: CASTExpression( rtParsePosition, CType::GetVoidType(), false )
	, m_tSwizzle( rtSwizzle )
	{
		AddReflection( "Mask", EASTReflectionType_SString, &m_tSwizzle );
	}

	const char* GetElementName() const { return "Swizzle"; }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		return tChildren;
	}

	void EvaluateType() override
	{
		m_tType = CType::GetVoidType();
	}

	const char* GetSwizzle()
	{
		return m_tSwizzle.c_str();
	}

	unsigned int GetSwizzleSize()
	{
		return m_tSwizzle.size();
	}

private:

	std::string m_tSwizzle;
};

class CASTExpressionMemberAccess : public CASTExpression
{
public:

	CASTExpressionMemberAccess( const SParsePosition& rtParsePosition, const std::string& rtIdentifier )
	: CASTExpression( rtParsePosition, CType::GetVoidType(), false )
	, m_tIdentifier( rtIdentifier )
	{
		AddReflection( "Identifier", EASTReflectionType_SString, &m_tIdentifier );
	}

	const char* GetElementName() const { return "MemberAccess"; }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		return tChildren;
	}

	void EvaluateType() override
	{
		m_tType = CType::GetVoidType();
	}

private:

	std::string m_tIdentifier;
};

class CASTExpressionStatement;

class CASTVariable : public CASTExpression
{
public:

	CASTVariable( const SParsePosition& rtParsePosition, const std::string& rtName, const CType& tType, CASTExpressionStatement* pExpressionStatement )
	: CASTExpression( rtParsePosition, tType, false ) 
	, m_tName( rtName )
	, m_pAssignment( pExpressionStatement )
	, m_uFlags( 0 )
	{
		AddReflection( "Type", EASTReflectionType_Type, &m_tType );
		AddReflection( "Name", EASTReflectionType_SString, &m_tName );
		AddReflection( "Assignment", EASTReflectionType_ASTNode, &m_pAssignment );

		AddCondition( "HasAssignment", [&]() { return m_pAssignment != nullptr; } );
	}

	const char* GetElementName() const { return "Variable"; }

	static CASTVariable* CreateDummyVariable( const SParsePosition& rtParsePosition, const std::string& rtName )
	{
		CASTVariable* pVariable = new CASTVariable( rtParsePosition, rtName, CType( "", EScalarType_Dummy ), nullptr );
		return pVariable;
	}

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		return tChildren;
	}

	const std::string& GetName() const { return m_tName; }

	void EvaluateType() override
	{

	}

	void SetExpressionStatement( CASTExpressionStatement* pAssignment )
	{
		m_pAssignment = pAssignment;
	}

	CASTExpressionStatement* GetAssignment()
	{
		return m_pAssignment;
	}

private:

	std::string m_tName;

	CASTExpressionStatement* m_pAssignment;

	unsigned int m_uFlags;
};

#endif //SHADR_AST_EXPRESSION_H