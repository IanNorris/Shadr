#if !defined( SHADR_AST_STATEMENT_H )
#define SHADR_AST_STATEMENT_H

#include "Tokens.h"

class CASTStatement : public CASTBase
{
public:

	CASTStatement()
	: CASTBase()
	{}
};

class CASTNopStatement : public CASTStatement
{
public:

	const char* GetElementName() const { return "Nop"; }

	CASTNopStatement()
	: CASTStatement()
	{}
};

class CASTReturnStatement : public CASTStatement
{
public:

	CASTReturnStatement( CASTExpression* pExpression )
	: CASTStatement()
	, m_pExpression( pExpression )
	{
		AddReflection( "Expression", EASTReflectionType_ASTNode, &m_pExpression );
	}

	const char* GetElementName() const { return "Return"; }

private:

	CASTExpression* m_pExpression;
};

class CASTExpressionStatement : public CASTStatement
{
public:

	CASTExpressionStatement( CASTExpression* pExpression, bool bChild )
	: CASTStatement()
	, m_pExpression( pExpression )
	, m_bChild( bChild )
	{
		AddReflection( "Expression", EASTReflectionType_ASTNode, &m_pExpression );

		AddCondition( "IsChild", [&]() { return m_bChild; } );
	}

	const char* GetElementName() const { return "ExpressionStatement"; }

private:

	CASTExpression* m_pExpression;
	bool			m_bChild;
};

class CASTIfStatement : public CASTStatement
{
public:

	CASTIfStatement( CASTExpression* pCondition, CASTStatement* pStatement, CASTStatement* pElseStatement )
	: CASTStatement()
	, m_pCondition( pCondition )
	, m_pStatement( pStatement )
	, m_pElseStatement( pElseStatement )
	{
		AddReflection( "Condition", EASTReflectionType_ASTNode, &m_pCondition );
		AddReflection( "Statement", EASTReflectionType_ASTNode, &m_pStatement );
		AddReflection( "ElseStatement", EASTReflectionType_ASTNode, &m_pElseStatement );

		AddCondition( "HasElse", [&]() { return (m_pElseStatement != nullptr); } );
	}

	const char* GetElementName() const { return "If"; }

private:

	CASTExpression* m_pCondition;
	CASTStatement* m_pStatement;
	CASTStatement* m_pElseStatement;
};

class CASTWhileStatement : public CASTStatement
{
public:

	CASTWhileStatement( CASTExpression* pCondition, CASTStatement* pStatement )
	: CASTStatement()
	, m_pCondition( pCondition )
	, m_pStatement( pStatement )
	{
		AddReflection( "Condition", EASTReflectionType_ASTNode, &m_pCondition );
		AddReflection( "Statement", EASTReflectionType_ASTNode, &m_pStatement );
	}

	const char* GetElementName() const { return "While"; }

private:

	CASTExpression* m_pCondition;
	CASTStatement* m_pStatement;
};

class CASTDoWhileStatement : public CASTStatement
{
public:

	CASTDoWhileStatement( CASTExpression* pCondition, CASTStatement* pStatement )
	: CASTStatement()
	, m_pCondition( pCondition )
	, m_pStatement( pStatement )
	{
		AddReflection( "Condition", EASTReflectionType_ASTNode, &m_pCondition );
		AddReflection( "Statement", EASTReflectionType_ASTNode, &m_pStatement );
	}

	const char* GetElementName() const { return "DoWhile"; }

private:

	CASTExpression* m_pCondition;
	CASTStatement* m_pStatement;
};

class CASTForStatement : public CASTStatement
{
public:

	CASTForStatement( CASTStatement* pInitialStatement, CASTExpression* pCondition, CASTExpression* pIterationExpression, CASTStatement* pBodyStatement, CScope* pForScope )
	: CASTStatement()
	, m_pScope( pForScope )
	, m_pInitialStatement( pInitialStatement )
	, m_pCondition( pCondition )
	, m_pIterationExpression( pIterationExpression )
	, m_pBody( pBodyStatement )
	{
		AddReflection( "InitialStatement", EASTReflectionType_ASTNode, &m_pInitialStatement );
		AddReflection( "Condition", EASTReflectionType_ASTNode, &m_pCondition );
		AddReflection( "IterationExpression", EASTReflectionType_ASTNode, &m_pIterationExpression );
		AddReflection( "Body", EASTReflectionType_ASTNode, &m_pBody );
	}

	const char* GetElementName() const { return "For"; }

private:

	CScope* m_pScope;
	CASTStatement* m_pInitialStatement;
	CASTExpression* m_pCondition;
	CASTExpression* m_pIterationExpression;
	CASTStatement* m_pBody;
};

class CASTBlockStatement : public CASTStatement, public CASTScope
{
public:

	CASTBlockStatement( CScope* pParentScope )
	: CASTStatement()
	, CASTScope( pParentScope )
	{
		AddReflection( "Statements", EASTReflectionType_ASTNodeArray, &m_apStatements );
	}

	const char* GetElementName() const { return "Block"; }

	void AddStatement( CASTStatement* pStatement ) { m_apStatements.push_back( pStatement ); }

private:

	std::vector< CASTStatement* >	m_apStatements;
};

class CASTVariableDefinition : public CASTStatement
{
public:

	CASTVariableDefinition( const CType& rtType, const std::string& rtName, bool bIsChild )
	: CASTStatement()
	, m_tType( rtType )
	, m_pType( &m_tType )
	, m_bChild( bIsChild )
	{
		SVariable* pVariable = new SVariable();
		pVariable->tName = rtName;
		pVariable->pType = &m_tType;

		m_tVariables.push_back( pVariable );

		AddReflection( "Type", EASTReflectionType_Type, &m_pType );
		AddReflection( "Variables", EASTReflectionType_ASTNodeArray, &m_tVariables );
		AddReflection( "Assignments", EASTReflectionType_ASTNodeArray, &m_tAssignments );

		AddCondition( "IsChild", [&]() { return m_bChild; } );
	}

	const char* GetElementName() const { return "VariableDef"; }

	const CType& GetType() const { return m_tType; }
	CType& GetType() { return m_tType; }

	std::vector<SVariable*>& GetVariables() { return m_tVariables; }
	std::vector<CASTStatement*>& GetAssignments() { return m_tAssignments; }

private:

	CType*		m_pType;
	CType		m_tType;

	std::vector<SVariable*>	m_tVariables;
	std::vector<CASTStatement*> m_tAssignments;

	bool		m_bChild;
};

#endif //SHADR_AST_STATEMENT_H
