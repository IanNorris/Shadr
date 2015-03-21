#if !defined( SHADR_AST_H )
#define SHADR_AST_H

#include "Tokens.h"

class CASTBase
{
public:

	CASTBase( EShaderToken eToken ) : m_eToken( eToken ){}
	virtual ~CASTBase() {}

	virtual llvm::Value* GenerateCode() = 0;

private:

	EShaderToken m_eToken;
};

#include "Expression.h"
#include "Numeric.h"

#endif //SHADR_AST_H