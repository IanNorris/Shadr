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

void Optimise( CASTBase* pNode, CASTScope* pScope, CASTBase* pParent, std::vector< std::pair< CASTExpression*, CASTExpression* > >& toReplaceParent )
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

	std::vector< std::pair< CASTExpression*, CASTExpression* > > toReplace;

	auto tChildren = pNode->GetChildren();
	for( auto& pChild : tChildren )
	{
		Optimise( pChild, pScope, pNode, toReplace );
	}

	if( !toReplace.empty() )
	{
		//If we updated any nodes, we need to push those down
		auto pExpr = dynamic_cast<CASTExpression*>(pNode);
		if( pExpr )
		{
			pNode = CASTExpression::ReplaceAndCloneSingle( pExpr, toReplace, 1 /*Only recurse one level deep*/ );
			toReplaceParent.push_back( std::pair< CASTExpression*, CASTExpression* >( pExpr, (CASTExpression*)pNode ) );
		}

		//If we updated any nodes, we need to push those down
		auto pExprStatement = dynamic_cast<CASTExpressionStatement*>(pNode);
		if( pExprStatement )
		{
			for( auto& tExpPair : toReplace )
			{
				if( pExprStatement->GetExpression() == tExpPair.first )
				{
					pExprStatement->SetExpression( tExpPair.second );
					break;
				}
			}
		}
	}

	auto pFunctionCall = dynamic_cast<CASTExpressionFunctionCall*>(pNode);
	if( pFunctionCall && pFunctionCall->IsInline() )
	{
		Assert( pParent, "How did we get a function call on its own with no parent?" );

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
					auto tChildren = pFunctionBody->GetChildren();

					Assert( tChildren.size() == 2, "Function implementation does not have two children: prototype and child, it has %u", tChildren.size() );

					CASTPrototype* pPrototype = dynamic_cast<CASTPrototype*>(tChildren[0]);
					CASTBlockStatement* pBody = dynamic_cast<CASTBlockStatement*>(tChildren[1]);
					if( pPrototype && pBody )
					{
						auto tBodyChildren = pBody->GetChildren();

						//For now we'll only inline functions that have a single expression in the return.
						if( tBodyChildren.size() == 1 )
						{
							auto pReturnStatement = dynamic_cast<CASTReturnStatement*>(tBodyChildren[0]);
							if( pReturnStatement )
							{
								CASTExpression* pReturnExpression = dynamic_cast<CASTExpression*>(pReturnStatement->GetChildren()[0]);
								if( pReturnExpression )
								{
									std::vector< std::pair< CASTExpression*, CASTExpression* > > tExpressionReplacements;

									auto& atParameters = pPrototype->GetParameters();

									int iParamIndex = 0;
									for( auto pParameter : atParameters )
									{
										auto& atVariables = pParameter->GetVariables();
										Assert( atVariables.size() == 1, "Multiple variables defined per symbol, this shouldn't be possible with a function definition" );

										for( auto pVariable : atVariables )
										{
											tExpressionReplacements.push_back( std::pair< CASTExpression*, CASTExpression* >( pVariable, parameterExpressions[ iParamIndex ] ) );
										}

										iParamIndex++;
									}

									toReplaceParent.push_back( std::pair< CASTExpression*, CASTExpression* >( pFunctionCall, CASTExpression::ReplaceAndCloneSingle( pReturnExpression, tExpressionReplacements, (unsigned int)-1 ) ) );
								}
							}
						}
					}

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
