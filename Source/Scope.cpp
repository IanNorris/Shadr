#include <stdio.h>
#include <stdlib.h>

#include "Tokens.h"
#include "Utility/Error.h"
#include "Utility/Utility.h"
#include "CompilationUnit.h"
#include "AST/AST.h"

void CScope::AddVariable( SParseContext& rtContext, CASTVariable* pVariable )
{
	auto tIter = m_pVariables.find( pVariable->GetName() );
	if( tIter != m_pVariables.end() )
	{
		if( (*tIter).second->GetType().GetScalarType() != EScalarType_Dummy )
		{
			Error_Compiler( EError_Error, rtContext.pszFilename, rtContext.uCurrentRow, rtContext.uCurrentCol, "Identifier %s is already defined in this scope.", pVariable->GetName().c_str() );
		}
	}

	m_pVariables[ pVariable->GetName() ] = pVariable;
}

void CScope::AddPrototype( SParseContext& rtContext, CASTPrototype* pPrototype )
{
	m_pPrototypes.insert( std::pair< std::string, CASTPrototype* >( pPrototype->GetFunctionName(), pPrototype ) );
}

void CScope::AddFunction( SParseContext& rtContext, CASTFunction* pFunction )
{
	m_pFunctions.insert( std::pair< std::string, CASTFunction* >( pFunction->GetPrototype()->GetFunctionName(), pFunction ) );
}
