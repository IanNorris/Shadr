#if !defined( SHADR_AST_H )
#define SHADR_AST_H

#include "Tokens.h"
#include "Type.h"
#include "Scope.h"
#include "Utility/Error.h"


class CASTScope
{
public:

	CASTScope( CScope* pScope )
	: m_tScope( pScope )
	{}

	CScope& GetScope() { return m_tScope; }
	const CScope& GetScope() const { return m_tScope; }

private:
	
	CScope							m_tScope;
};


class CASTBase : public CReflectionObject
{
public:
	CASTBase( const SParsePosition& rtParsePosition )
	: m_tParserPosition( rtParsePosition )
	{
	}

	virtual std::vector< CASTBase* > GetChildren( void ) = 0;

	SParsePosition& GetParserPosition(){ return m_tParserPosition; }

private:

	SParsePosition m_tParserPosition;
};

class CASTProgram : public CASTBase, public CASTScope
{
public:

	CASTProgram( const SParsePosition& rtParsePosition, CScope* pParentScope )
	: CASTBase( rtParsePosition )
	, CASTScope( pParentScope )
	{
		AddReflection( "Elements", EASTReflectionType_ASTNodeArray, &m_apElements );
	}

	const char* GetElementName() const { return "Program"; }

	void AddElement( CASTBase* pElement ) { m_apElements.push_back( pElement ); }

	std::vector< CASTBase* > GetChildren( void ) { return m_apElements; }

private:

	std::vector< CASTBase* > m_apElements;
};

#include "ASTExpression.h"
#include "ASTStatement.h"
#include "ASTNumeric.h"
#include "ASTFunction.h"

#endif //SHADR_AST_H