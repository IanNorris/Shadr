#include <stdio.h>
#include <stdlib.h>

#include "Tokens.h"
#include "Utility/Error.h"
#include "Utility/Utility.h"
#include "CompilationUnit.h"
#include "AST/AST.h"

void Reconcile( CASTBase* pNode, CASTScope* pScope )
{
	if( !pNode )
	{
		return;
	}

	CASTScope* pNewScope = dynamic_cast<CASTScope*>(pNode);

	if( pNewScope != nullptr )
	{
		pScope = pNewScope;
	}

	auto tChildren = pNode->GetChildren();
	for( auto& pChild : tChildren )
	{
		Reconcile( pChild, pScope );
	}
	
	auto pFunctionCall = dynamic_cast<CASTExpressionFunctionCall*>(pNode);
	if( pFunctionCall )
	{
		if( pFunctionCall->FindMatchingFunction( &pScope->GetScope() ) )
		{
			pFunctionCall->FindMatchingFunctionBody( &pScope->GetScope() );
		}
	}

	auto pExpression = dynamic_cast<CASTExpression*>(pNode);
	if( pExpression )
	{
		pExpression->EvaluateType();
	}
}
