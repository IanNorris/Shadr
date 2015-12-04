#include <stdio.h>
#include <stdlib.h>

#include "Tokens.h"
#include "Utility/Error.h"
#include "Utility/Utility.h"
#include "CompilationUnit.h"
#include "AST/AST.h"

void IdentifySideEffects( CASTBase* pNode, CASTScope* pScope )
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
		IdentifySideEffects( pChild, pScope );
	}

	auto pFunctionCall = dynamic_cast<CASTExpressionFunctionCall*>(pNode);
	if( pFunctionCall )
	{
		CASTPrototype* pPrototype = pScope->GetScope().FindPrototype( pFunctionCall->GetFunctionName() );

		if( !pPrototype )
		{
			Error_Compiler( 
				EError_Error, 
				pFunctionCall->GetParserPosition().pszFilename, 
				pFunctionCall->GetParserPosition().uCurrentRow, 
				pFunctionCall->GetParserPosition().uCurrentCol, 
				"Function \"%s\" was not defined.", 
				pFunctionCall->GetFunctionName().c_str()
			);

			return;
		}

		if( pPrototype->GetParameters().size() != pFunctionCall->GetParameters().size() )
		{
			Error_Compiler( 
				EError_Error, 
				pFunctionCall->GetParserPosition().pszFilename, 
				pFunctionCall->GetParserPosition().uCurrentRow, 
				pFunctionCall->GetParserPosition().uCurrentCol, 
				"Function \"%s\" expects %d parameters, %d provided.", 
				pFunctionCall->GetFunctionName().c_str(), 
				pPrototype->GetParameters().size(), 
				pFunctionCall->GetParameters().size()
			);
		}
	}
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
	if( pFunctionCall )
	{
		CASTPrototype* pPrototype = pScope->GetScope().FindPrototype( pFunctionCall->GetFunctionName() );

		if( !pPrototype )
		{
			Error_Compiler( 
				EError_Error, 
				pFunctionCall->GetParserPosition().pszFilename, 
				pFunctionCall->GetParserPosition().uCurrentRow, 
				pFunctionCall->GetParserPosition().uCurrentCol, 
				"Function \"%s\" was not defined.", 
				pFunctionCall->GetFunctionName().c_str()
			);

			return;
		}

		if( pPrototype->GetParameters().size() != pFunctionCall->GetParameters().size() )
		{
			Error_Compiler( 
				EError_Error, 
				pFunctionCall->GetParserPosition().pszFilename, 
				pFunctionCall->GetParserPosition().uCurrentRow, 
				pFunctionCall->GetParserPosition().uCurrentCol, 
				"Function \"%s\" expects %d parameters, %d provided.", 
				pFunctionCall->GetFunctionName().c_str(), 
				pPrototype->GetParameters().size(), 
				pFunctionCall->GetParameters().size()
			);
		}
	}
}
