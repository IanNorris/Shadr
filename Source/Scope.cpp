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
	/*if( tIter != m_pPrototypes.end() )
	{
		auto& pOther = (*tIter).second;
	}*/

	m_pPrototypes[ pPrototype->GetFunctionName() ] = pPrototype;
}