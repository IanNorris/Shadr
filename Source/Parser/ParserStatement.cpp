#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"

CASTStatement* ParseStatement( SParseContext& rtContext, CScope* pParentScope )
{
	SParseContext tContextCopy = rtContext;

	//Is it a block?
	if( rtContext.sNextToken.eToken == EShaderToken_Brace_Open )
	{
		ConsumeToken( rtContext );

		return ParseBlockStatement( rtContext, pParentScope );
	}
	else if( rtContext.sNextToken.eToken == EShaderToken_Return )
	{
		ConsumeToken( rtContext );

		CASTExpression* pExpression = ParseExpression( rtContext, pParentScope );


		if( rtContext.sNextToken.eToken == EShaderToken_SemiColon )
		{
			ConsumeToken( rtContext );
		}
		else
		{
			ParserError( rtContext, "Expected semi-colon (;)");
		}

		if( pExpression )
		{
			return new CASTReturnStatement( pExpression );
		}
		else
		{
			return NULL;
		}
	}
	else if( rtContext.sNextToken.eToken == EShaderToken_If )
	{
		ConsumeToken( rtContext );

		return ParseIfStatement( rtContext, pParentScope );
	}

	return NULL;
}

CASTBlockStatement* ParseBlockStatement( SParseContext& rtContext, CScope* pParentScope )
{
	CASTBlockStatement* pBlock = new CASTBlockStatement( pParentScope );

	while( rtContext.sNextToken.eToken != EShaderToken_Brace_Close )
	{
		CASTStatement* pStatement = ParseStatement( rtContext, &pBlock->GetScope() );

		if( pStatement )
		{
			pBlock->AddStatement( pStatement );
		}
	}
	ConsumeToken( rtContext ); //Consume the brace

	return pBlock;
}

CASTIfStatement* ParseIfStatement( SParseContext& rtContext, CScope* pParentScope )
{
	if( rtContext.sNextToken.eToken != EShaderToken_Parenthesis_Open )
		{
			ParserError( rtContext, "Expected open parenthesis (");
		}
		ConsumeToken( rtContext );

		CASTExpression* pCondition = ParseExpression( rtContext, pParentScope );
		if( !pCondition )
		{
			return NULL;
		}

		if( rtContext.sNextToken.eToken != EShaderToken_Parenthesis_Close )
		{
			ParserError( rtContext, "Expected open parenthesis (");
		}
		ConsumeToken( rtContext );

		CASTStatement* pStatement = ParseStatement( rtContext, pParentScope );

		if( !pStatement )
		{
			return NULL;
		}

		CASTStatement* pElseStatement = NULL;

		if( rtContext.sNextToken.eToken == EShaderToken_Else )
		{
			ConsumeToken( rtContext );

			pElseStatement = ParseStatement( rtContext, pParentScope );
		}

		return new CASTIfStatement( pCondition, pStatement, pElseStatement );
}
