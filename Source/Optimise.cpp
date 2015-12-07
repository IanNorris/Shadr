#include <stdio.h>
#include <stdlib.h>

#include "Tokens.h"
#include "Utility/Error.h"
#include "Utility/Utility.h"
#include "CompilationUnit.h"
#include "AST/AST.h"

bool FindReferences( CASTBase* pNode, CASTBase* pNodeToFind )
{
	if( !pNode )
	{
		return false;
	}

	auto pFunctionCall = dynamic_cast<CASTExpressionFunctionCall*>(pNode);
	if( pFunctionCall )
	{
		if(		pFunctionCall->GetPrototype() == pNodeToFind
			||	pFunctionCall->GetFunctionBody() == pNodeToFind )
		{
			return true;
		}
	}

	auto tChildren = pNode->GetChildren();
	for( auto& pChild : tChildren )
	{
		if( pChild == pNodeToFind )
		{
			return true;
		}

		if( FindReferences( pChild, pNodeToFind ) )
		{
			return true;
		}
	}

	return false;
}

bool IdentifySideEffects( CASTBase* pNode, CASTScope* pScope )
{
	if( !pNode )
	{
		return false;
	}

	CASTScope* pNewScope = dynamic_cast<CASTScope*>(pNode);

	if( pNewScope != nullptr )
	{
		pScope = pNewScope;
	}

	pNode->SetHasSideEffect( false );

	bool bChildHadSideEffects = false;
	auto tChildren = pNode->GetChildren();
	for( auto& pChild : tChildren )
	{
		if( IdentifySideEffects( pChild, pScope ) )
		{
			bChildHadSideEffects = true;
		}
	}

	//We may have explicitly tagged a node as a side effect.
	//This is useful for identifying side effects that the compiler can't
	//necessarily see (such as into intrinsics)
	auto pAnnotations = dynamic_cast<CASTAnnotationSupport*>(pNode);
	bool bHasSideEffects = false;
	if( pAnnotations && pAnnotations->GetAnnotation(EAnnotation_SideEffect) )
	{
		bHasSideEffects = true;
	}

	pNode->SetHasSideEffect( bHasSideEffects || bChildHadSideEffects );

	return pNode->HasSideEffect();
}

void Optimise( CASTBase* pNode, CASTScope* pScope )
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
		Optimise( pChild, pScope );
	}

	auto pFunctionCall = dynamic_cast<CASTExpressionFunctionCall*>(pNode);
	if( pFunctionCall && pFunctionCall->IsInline() )
	{
		CASTFunction* pFunctionBody = pFunctionCall->GetFunctionBody();
		if( pFunctionBody )
		{
			//Check if the function is recursive. If it is, we can't inline
			if( !FindReferences( pFunctionBody, pFunctionBody ) )
			{
				//Check if the input parameters have side effects, if they do, we need to be careful we don't
				//modify the order of any operations. 

				//Check each parameter, if we don't modify the parameters, we can rewrite the AST
				//so that the input parameters use the input expressions directly.

				//If the entire function lives in the return, or can be pushed into a single expression
				//then we can insert it directly into any expression referencing it.
			}
			else
			{
				Error_Compiler( 
					EError_Warning, 
					pFunctionBody->GetParserPosition().pszFilename,
					pFunctionBody->GetParserPosition().uCurrentRow,
					pFunctionBody->GetParserPosition().uCurrentCol,
					"Function \"%s\" is flagged for inlining yet cannot (yet) be inlined as it is recursive.",
					pFunctionBody->GetPrototype()->GetFunctionName().c_str()
				);
			}
		}
	}
}
