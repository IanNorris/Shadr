#if !defined( SHADR_AST_STATEMENT_H )
#define SHADR_AST_STATEMENT_H

#include "Tokens.h"

enum EAnnotation
{
	EAnnotation_SideEffect, //!< The statement has a side-effect that may not be obvious to the compiler
	EAnnotation_Unroll,		//!< This statement should be unrolled to a maximum of (param0) iterations
	EAnnotation_ForceInline,//!< Force this function to be inlined in all cases (even when optimisations are disabled)
};

class CASTAnnotation : public CASTBase
{
public: 

	CASTAnnotation( const SParsePosition& rtParsePosition, EAnnotation eAnnotation, const std::vector<CASTExpression*>& rtParameters )
	: CASTBase( rtParsePosition )
	, m_eAnnotation( eAnnotation )
	, m_apParameters( rtParameters )
	{}

	EAnnotation GetType() { return m_eAnnotation; }

	const char* GetElementName() const { return "Annotation"; }

	virtual std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.resize( m_apParameters.size() );
		for( auto& rpAnnotation : m_apParameters )
		{
			tChildren.push_back( rpAnnotation );
		}
		return tChildren;
	}

private:

	EAnnotation m_eAnnotation;

	std::vector<CASTExpression*> m_apParameters;
};

class CASTAnnotationGroup : public CASTBase
{
public: 

	CASTAnnotationGroup( const SParsePosition& rtParsePosition )
	: CASTBase( rtParsePosition )
	{}

	std::vector<CASTAnnotation*>& GetAnnotations() { return m_apAnnotations; }

	const char* GetElementName() const { return "AnnotationGroup"; }

	virtual std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.reserve( m_apAnnotations.size() );
		for( auto& rpAnnotation : m_apAnnotations )
		{
			tChildren.push_back( rpAnnotation );
		}

		return tChildren;
	}

private:

	std::vector<CASTAnnotation*> m_apAnnotations;
};

class CASTAnnotationSupport
{
public:

	void AddAnnotation( CASTAnnotationGroup* pAnnotation )
	{
		m_tAnnotations.push_back( pAnnotation );
	}

	CASTAnnotation* GetAnnotation( EAnnotation eAnnotation )
	{
		for( auto& rpAnnotationGroups : m_tAnnotations )
		{
			auto& rpAnnotationGroup = rpAnnotationGroups->GetAnnotations();
			for( auto& rpAnnotation : rpAnnotationGroup )
			{
				if( rpAnnotation->GetType() == eAnnotation )
				{
					return rpAnnotation;
				}
			}
		}

		return nullptr;
	}

private:

	std::vector< CASTAnnotationGroup* > m_tAnnotations;
};

class CASTStatement : public CASTBase, public CASTAnnotationSupport
{
public:

	CASTStatement( const SParsePosition& rtParsePosition )
	: CASTBase( rtParsePosition )
	{}
};

class CASTNopStatement : public CASTStatement
{
public:

	const char* GetElementName() const { return "Nop"; }

	CASTNopStatement( const SParsePosition& rtParsePosition )
	: CASTStatement( rtParsePosition )
	{}

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		return tChildren;
	}
};

class CASTReturnStatement : public CASTStatement
{
public:

	CASTReturnStatement( const SParsePosition& rtParsePosition, CASTExpression* pExpression )
	: CASTStatement( rtParsePosition )
	, m_pExpression( pExpression )
	{
		AddReflection( "Expression", EASTReflectionType_ASTNode, &m_pExpression );
	}

	const char* GetElementName() const { return "Return"; }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.push_back( m_pExpression );
		return tChildren;
	}

private:

	CASTExpression* m_pExpression;
};

class CASTExpressionStatement : public CASTStatement
{
public:

	CASTExpressionStatement( const SParsePosition& rtParsePosition, CASTExpression* pExpression, bool bChild )
	: CASTStatement( rtParsePosition )
	, m_pExpression( pExpression )
	, m_bChild( bChild )
	{
		AddReflection( "Expression", EASTReflectionType_ASTNode, &m_pExpression );

		AddCondition( "IsChild", [&]() { return m_bChild; } );
	}

	const char* GetElementName() const { return "ExpressionStatement"; }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.push_back( m_pExpression );
		return tChildren;
	}

private:

	CASTExpression* m_pExpression;
	bool			m_bChild;
};

class CASTIfStatement : public CASTStatement
{
public:

	CASTIfStatement( const SParsePosition& rtParsePosition, CASTExpression* pCondition, CASTStatement* pStatement, CASTStatement* pElseStatement )
	: CASTStatement( rtParsePosition )
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

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.push_back( m_pCondition );
		tChildren.push_back( m_pStatement );
		tChildren.push_back( m_pElseStatement );
		return tChildren;
	}

private:

	CASTExpression* m_pCondition;
	CASTStatement* m_pStatement;
	CASTStatement* m_pElseStatement;
};

class CASTWhileStatement : public CASTStatement
{
public:

	CASTWhileStatement( const SParsePosition& rtParsePosition, CASTExpression* pCondition, CASTStatement* pStatement )
	: CASTStatement( rtParsePosition )
	, m_pCondition( pCondition )
	, m_pStatement( pStatement )
	{
		AddReflection( "Condition", EASTReflectionType_ASTNode, &m_pCondition );
		AddReflection( "Statement", EASTReflectionType_ASTNode, &m_pStatement );
	}

	const char* GetElementName() const { return "While"; }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.push_back( m_pCondition );
		tChildren.push_back( m_pStatement );
		return tChildren;
	}

private:

	CASTExpression* m_pCondition;
	CASTStatement* m_pStatement;
};

class CASTDoWhileStatement : public CASTStatement
{
public:

	CASTDoWhileStatement( const SParsePosition& rtParsePosition, CASTExpression* pCondition, CASTStatement* pStatement )
	: CASTStatement( rtParsePosition )
	, m_pCondition( pCondition )
	, m_pStatement( pStatement )
	{
		AddReflection( "Condition", EASTReflectionType_ASTNode, &m_pCondition );
		AddReflection( "Statement", EASTReflectionType_ASTNode, &m_pStatement );
	}

	const char* GetElementName() const { return "DoWhile"; }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.push_back( m_pCondition );
		tChildren.push_back( m_pStatement );
		return tChildren;
	}

private:

	CASTExpression* m_pCondition;
	CASTStatement* m_pStatement;
};

class CASTForStatement : public CASTStatement
{
public:

	CASTForStatement( const SParsePosition& rtParsePosition, CASTStatement* pInitialStatement, CASTExpression* pCondition, CASTExpression* pIterationExpression, CASTStatement* pBodyStatement, CScope* pForScope )
	: CASTStatement( rtParsePosition )
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

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		tChildren.push_back( m_pInitialStatement );
		tChildren.push_back( m_pCondition );
		tChildren.push_back( m_pIterationExpression );
		tChildren.push_back( m_pBody );
		return tChildren;
	}

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

	CASTBlockStatement( const SParsePosition& rtParsePosition, CScope* pParentScope )
	: CASTStatement( rtParsePosition )
	, CASTScope( pParentScope )
	{
		AddReflection( "Statements", EASTReflectionType_ASTNodeArray, &m_apStatements );
	}

	const char* GetElementName() const { return "Block"; }

	void AddStatement( CASTStatement* pStatement ) { m_apStatements.push_back( pStatement ); }

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		for( auto& child : m_apStatements )
		{
			tChildren.push_back( child );
		}
		return tChildren;
	}

private:

	std::vector< CASTStatement* >	m_apStatements;
};

class CASTVariableDefinition : public CASTStatement
{
public:

	CASTVariableDefinition( const SParsePosition& rtParsePosition, const CType& rtType, const std::string& rtName, bool bIsChild )
	: CASTStatement( rtParsePosition )
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

	std::vector< CASTBase* > GetChildren( void )
	{
		std::vector< CASTBase* > tChildren;
		for( auto& child : m_tAssignments )
		{
			tChildren.push_back( child );
		}
		return tChildren;
	}

private:

	CType*		m_pType;
	CType		m_tType;

	std::vector<SVariable*>	m_tVariables;
	std::vector<CASTStatement*> m_tAssignments;

	bool		m_bChild;
};

#endif //SHADR_AST_STATEMENT_H
