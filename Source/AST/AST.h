#if !defined( SHADR_AST_H )
#define SHADR_AST_H

#include "Tokens.h"
#include "Type.h"

class CASTBase
{
public:

	CASTBase( const CType& rtType )
	: m_tType( rtType )
	{}

	virtual ~CASTBase() {}

	virtual llvm::Value* GenerateCode( CModule* pModule ) = 0;
	const CType& GetType() const { return m_tType; }
	CType& GetType() { return m_tType; }

private:

	CType		m_tType;
};

#include "ASTExpression.h"
#include "ASTNumeric.h"
#include "ASTFunction.h"

#endif //SHADR_AST_H