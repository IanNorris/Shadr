#if !defined( SHADR_AST_STATEMENT_H )
#define SHADR_AST_STATEMENT_H

#include "Tokens.h"

class CASTStatement : public CASTBase
{
public:

	CASTStatement()
	: CASTBase( CType::GetVoidType() )
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

class CASTBlockStatement : public CASTStatement
{
public:

	CASTBlockStatement()
	: CASTStatement()
	{}

	void AddStatement( CASTStatement* pStatement ) { m_apStatements.push_back( pStatement ); }

private:

	std::vector< CASTStatement* > m_apStatements;
};

#endif //SHADR_AST_STATEMENT_H
