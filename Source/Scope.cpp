#include <stdio.h>
#include <stdlib.h>

#include "Tokens.h"
#include "Utility/Error.h"
#include "Utility/Utility.h"
#include "CompilationUnit.h"
#include "AST/AST.h"

void CScope::AddPrototype( SParseContext& rtContext, CASTPrototype* pPrototype )
{
	auto tIter = m_pPrototypes.find( pPrototype->GetFunctionName() );
	m_pPrototypes.insert( std::pair< std::string, CASTPrototype* >( pPrototype->GetFunctionName(), pPrototype ) );
}