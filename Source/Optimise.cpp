#include <stdio.h>
#include <stdlib.h>

#include "Tokens.h"
#include "Utility/Error.h"
#include "Utility/Utility.h"
#include "CompilationUnit.h"
#include "AST/AST.h"

bool DoesExpressionModify( CASTBase* pNode )
{
	if( !pNode )
	{
		return false;
	}

	//Looking for a variable
	auto pExpression = dynamic_cast<CASTExpression*>(pNode);
	if( pExpression && pExpression->DoesModifyLeft() )
	{
		return true;
	}

	auto pAnnotation = dynamic_cast<CASTAnnotationSupport*>(pNode);
	if( pAnnotation && pAnnotation->GetAnnotation(EAnnotation_SideEffect) )
	{
		return true;
	}

	auto tChildren = pNode->GetChildren();
	for( auto& pChild : tChildren )
	{
		if( DoesExpressionModify( pChild ) )
		{
			return true;
		}
	}

	return false;
}

bool FindReferences( CASTBase* pNode, CASTBase* pNodeToFind, bool bRequireModification )
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
			Assert( !bRequireModification, "It makes no sense to call FindReferences with a function and checking for modifications" );
			return true;
		}
	}

	//Looking for a variable
	if( bRequireModification && dynamic_cast<CASTVariable*>(pNodeToFind) )
	{
		auto pExpression = dynamic_cast<CASTExpression*>(pNode);
		if( pExpression && pExpression->DoesModifyLeft() )
		{
			auto& children = pExpression->GetChildren();
			if( !children.empty() )
			{
				auto pVariableRef = dynamic_cast<CASTVariableReference*>(children[0]);
				if( pVariableRef )
				{
					if( pVariableRef->GetVariable() == pNodeToFind )
					{
						return true;
					}
				}
			}
		}
	}

	auto tChildren = pNode->GetChildren();
	for( auto& pChild : tChildren )
	{
		if( pChild == pNodeToFind )
		{
			if( bRequireModification )
			{
				CASTExpression* pExpr = dynamic_cast<CASTExpression*>(pNode);
				if( pExpr )
				{
					if( pExpr->DoesModifyLeft() )
					{
						return true;
					}
				}
			}
			else
			{
				return true;
			}
		}

		if( FindReferences( pChild, pNodeToFind, bRequireModification ) )
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

	CASTExpression* pExpr = dynamic_cast<CASTExpression*>(pNode);
	if( pExpr )
	{
		if( pExpr->DoesModifyLeft() )
		{
			bHasSideEffects = true;
		}
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
			//Check if the function is recursive. If it is, we can't inline at the moment
			if( !FindReferences( pFunctionBody, pFunctionBody, false ) )
			{
				bool bAnyInputExpressionModified = false;
				auto& parameterExpressions = pFunctionCall->GetParameters();
				for( auto& parameter : parameterExpressions )
				{
					if( DoesExpressionModify( parameter ) )
					{
						bAnyInputExpressionModified = true;
					}
				}

				bool bAnyParameterModified = false;
				auto& parameters = pFunctionCall->GetPrototype()->GetParameters();
				for( auto& parameter : parameters )
				{
					auto& variables = parameter->GetVariables();
					for( auto& variable : variables )
					{
						if( FindReferences( pFunctionBody, variable, true ) )
						{
							bAnyParameterModified = true;
							break;
						}
					}
				}

				//TODO: Does it access a global?
				bool bAnyGlobalModified = false;

				if( !pFunctionBody->HasSideEffect() && !bAnyParameterModified && !bAnyInputExpressionModified && !bAnyGlobalModified )
				{
					//Function passed all tests and is safe to inline
				}

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
