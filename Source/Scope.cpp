#include <stdio.h>
#include <stdlib.h>

#include "Tokens.h"
#include "Utility/Error.h"
#include "Utility/Utility.h"
#include "CompilationUnit.h"
#include "AST/AST.h"

void CScope::AddPrototype( SParseContext& rtContext, CASTPrototype* pPrototype )
{
	m_pPrototypes.insert( std::pair< std::string, CASTPrototype* >( pPrototype->GetFunctionName(), pPrototype ) );
}

void CScope::AddFunction( SParseContext& rtContext, CASTFunction* pFunction )
{
	m_pFunctions.insert( std::pair< std::string, CASTFunction* >( pFunction->GetPrototype()->GetFunctionName(), pFunction ) );
}
