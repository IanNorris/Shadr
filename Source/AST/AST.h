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

class CASTBase
{
public:

	CASTBase()
	{}

	virtual ~CASTBase() {}

	//virtual llvm::Value* GenerateCode( CModule* pModule ) = 0;
	
};

class CASTProgram : public CASTBase, public CASTScope
{
public:

	CASTProgram()
	: CASTBase()
	, CASTScope( NULL )
	{}

	void AddElement( CASTBase* pElement ) { m_apElements.push_back( pElement ); }

private:

	std::vector< CASTBase* > m_apElements;
};

#include "ASTExpression.h"
#include "ASTStatement.h"
#include "ASTNumeric.h"
#include "ASTFunction.h"

#endif //SHADR_AST_H