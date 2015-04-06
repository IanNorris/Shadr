#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"

CASTStatement* ParseStatement( SParseContext& rtContext )
{
	SParseContext tContextCopy = rtContext;

	//Is it a block?
	if( rtContext.sNextToken.eToken == EShaderToken_Brace_Open )
	{
		ConsumeToken( rtContext );

		return ParseBlockStatement( rtContext );
	}
	else if( rtContext.sNextToken.eToken == EShaderToken_Return )
	{
		ConsumeToken( rtContext );

		CASTExpression* pExpression = ParseExpression( rtContext );
		if( pExpression )
		{
			return new CASTReturnStatement( pExpression );
		}
		else
		{
		
		}
	}

	return NULL;
}

CASTBlockStatement* ParseBlockStatement( SParseContext& rtContext )
{
	CASTBlockStatement* pBlock = new CASTBlockStatement();

	while( rtContext.sNextToken.eToken != EShaderToken_Brace_Close )
	{
		CASTStatement* pStatement = ParseStatement( rtContext );

		if( pStatement )
		{
			pBlock->AddStatement( pStatement );
		}
	}

	return pBlock;
}