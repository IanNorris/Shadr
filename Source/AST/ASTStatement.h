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
	{}

private:

	CASTExpression* m_pExpression;
};

class CASTExpressionStatement : public CASTStatement
{
public:

	CASTExpressionStatement( CASTExpression* pExpression )
	: CASTStatement()
	, m_pExpression( pExpression )
	{}

private:

	CASTExpression* m_pExpression;
};

class CASTIfStatement : public CASTStatement
{
public:

	CASTIfStatement( CASTExpression* pCondition, CASTStatement* pStatement, CASTStatement* pElseStatement )
	: CASTStatement()
	, m_pCondition( pCondition )
	, m_pStatement( pStatement )
	, m_pElseStatement( pElseStatement )
	{}

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
	{}

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
	{}

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
	{}

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
	{}

	void AddStatement( CASTStatement* pStatement ) { m_apStatements.push_back( pStatement ); }

private:

	std::vector< CASTStatement* >	m_apStatements;
};

class CASTVariableDefinition : public CASTStatement
{
public:

	CASTVariableDefinition( const CType& rtType, const std::string& rtName )
	: CASTStatement()
	, m_tType( rtType )
	{
		SVariable tVariable;
		tVariable.tName = rtName;
		tVariable.pType = &m_tType;

		m_tVariables.push_back( tVariable );
	}

	const CType& GetType() const { return m_tType; }
	CType& GetType() { return m_tType; }

	std::vector<SVariable>& GetVariables() { return m_tVariables; }
	std::vector<CASTStatement*>& GetAssignments() { return m_tAssignments; }

	/*llvm::Value* GenerateCode( CModule* pModule )
	{
		Assert( 0, "Attempting to call GenerateCode on an unsupported AST node (CASTVariableDefinition)." );
		return NULL;
	}*/

private:

	CType		m_tType;

	std::vector<SVariable>	m_tVariables;
	std::vector<CASTStatement*> m_tAssignments;
};

#endif //SHADR_AST_STATEMENT_H
