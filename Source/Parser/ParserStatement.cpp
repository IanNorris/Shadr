#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"

CASTVariableDefinition* ParseVariableDefinition( SParseContext& rtContext, CScope* pParentScope )
{
	//We're expecting to parse <Type> <Identifier> [ = <Expression> ]) [ (, <Identifier> [ = <Expression> ]))+ ]

	CType* pType = ParseType( rtContext );

	if( pType == nullptr || pType->GetScalarType() == EScalarType_Void )
	{
		return NULL;
	}

	CASTVariableDefinition* pVariableDef = nullptr;

	bool bContinue = false;
	do
	{
		SVariable* pVar = nullptr;

		std::string tName;
		bool bCommaAcceptable = false;

		SParseContext tContextPeek = rtContext;

		if( rtContext.sNextToken.eToken != EShaderToken_Identifier )
		{
			return NULL;
		}
		else
		{
			tName = std::string( rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength );

			if( pVariableDef )
			{
				pVar = new SVariable();
				pVar->tName = tName;
				pVar->pType = pType;

				pVariableDef->GetVariables().push_back( pVar );
			}
			else
			{
				pVariableDef = new CASTVariableDefinition( *pType, tName, false );
				pVar = pVariableDef->GetVariables()[0];
			}

			pParentScope->AddVariable( rtContext, pVariableDef->GetVariables().back() );

			ConsumeToken( rtContext );
		}

		if( rtContext.sNextToken.eToken == EShaderToken_Assign )
		{
			//Restore context to previous position, we can consume the entire token
			rtContext = tContextPeek;
			
			rtContext.SetFlag( EParseFlag_RejectComma );

			CASTExpression* pExpression = ParseExpression( rtContext, pParentScope );
			if( pExpression )
			{
				pVar->pAssignment = new CASTExpressionStatement( pExpression, true );

				pVariableDef->GetAssignments().push_back( pVar->pAssignment );
			}
			else
			{
				ParserError( rtContext, "Expected expression");
			}

			rtContext.UnsetFlag( EParseFlag_RejectComma );
		}

		if( bContinue = (rtContext.sNextToken.eToken == EShaderToken_Comma) )
		{
			ConsumeToken( rtContext );
		}

	} while( bContinue );

	if( rtContext.sNextToken.eToken == EShaderToken_SemiColon )
	{
		ConsumeToken( rtContext );
	}
	else
	{
		ParserError( rtContext, "Expected semi-colon (;)");
	}

	return pVariableDef;
}

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
	else if( rtContext.sNextToken.eToken == EShaderToken_While )
	{
		ConsumeToken( rtContext );

		return ParseWhileStatement( rtContext, pParentScope );
	}
	else if( rtContext.sNextToken.eToken == EShaderToken_Do )
	{
		ConsumeToken( rtContext );

		return ParseDoWhileStatement( rtContext, pParentScope );
	}
	else if( rtContext.sNextToken.eToken == EShaderToken_For )
	{
		ConsumeToken( rtContext );

		return ParseForStatement( rtContext, pParentScope );
	}
	else if( rtContext.sNextToken.eToken == EShaderToken_SemiColon )
	{
		ConsumeToken( rtContext );

		return new CASTNopStatement();
	}

	rtContext = tContextCopy;

	CASTExpression* pExpression = ParseExpression( rtContext, pParentScope );
	if( pExpression )
	{
		if( rtContext.sNextToken.eToken == EShaderToken_SemiColon )
		{
			ConsumeToken( rtContext );
		}
		else
		{
			ParserError( rtContext, "Expected semi-colon (;)");
		}

		return new CASTExpressionStatement( pExpression, false );
	}

	rtContext = tContextCopy;

	CASTVariableDefinition* pDefinition = ParseVariableDefinition( rtContext, pParentScope );
	if( pDefinition )
	{
		return pDefinition;
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

CASTWhileStatement* ParseWhileStatement( SParseContext& rtContext, CScope* pParentScope )
{
	if( rtContext.sNextToken.eToken != EShaderToken_Parenthesis_Open )
	{
		ParserError( rtContext, "Expected open parenthesis");
	}
	ConsumeToken( rtContext );

	CASTExpression* pCondition = ParseExpression( rtContext, pParentScope );
	if( !pCondition )
	{
		return NULL;
	}

	if( rtContext.sNextToken.eToken != EShaderToken_Parenthesis_Close )
	{
		ParserError( rtContext, "Expected closing parenthesis");
	}
	ConsumeToken( rtContext );

	CASTStatement* pStatement = ParseStatement( rtContext, pParentScope );

	if( !pStatement )
	{
		return NULL;
	}

	return new CASTWhileStatement( pCondition, pStatement );
}

CASTDoWhileStatement* ParseDoWhileStatement( SParseContext& rtContext, CScope* pParentScope )
{
	CASTStatement* pStatement = ParseStatement( rtContext, pParentScope );

	if( !pStatement )
	{
		return NULL;
	}

	if( rtContext.sNextToken.eToken != EShaderToken_While )
	{
		ParserError( rtContext, "Expected 'while'");
	}
	ConsumeToken( rtContext );

	if( rtContext.sNextToken.eToken != EShaderToken_Parenthesis_Open )
	{
		ParserError( rtContext, "Expected open parenthesis");
	}
	ConsumeToken( rtContext );

	CASTExpression* pCondition = ParseExpression( rtContext, pParentScope );
	if( !pCondition )
	{
		return NULL;
	}

	if( rtContext.sNextToken.eToken != EShaderToken_Parenthesis_Close )
	{
		ParserError( rtContext, "Expected closing parenthesis");
	}
	ConsumeToken( rtContext );

	if( rtContext.sNextToken.eToken != EShaderToken_SemiColon )
	{
		ParserError( rtContext, "Expected semi-colon (;)");
	}
	ConsumeToken( rtContext );

	return new CASTDoWhileStatement( pCondition, pStatement );
}

CASTForStatement* ParseForStatement( SParseContext& rtContext, CScope* pParentScope )
{
	CScope* pForScope = new CScope( pParentScope );

	if( rtContext.sNextToken.eToken != EShaderToken_Parenthesis_Open )
	{
		ParserError( rtContext, "Expected open parenthesis");
	}
	ConsumeToken( rtContext );

	CASTStatement* pInitialStatement = ParseStatement( rtContext, pForScope );
	if( !pInitialStatement )
	{
		ParserError( rtContext, "Expected statement");
		return NULL;
	}

	CASTExpression* pCondition = ParseExpression( rtContext, pForScope );
	if( !pCondition )
	{
		return NULL;
	}

	if( rtContext.sNextToken.eToken != EShaderToken_SemiColon )
	{
		ParserError( rtContext, "Expected semi-colon (;)");
	}
	ConsumeToken( rtContext );

	CASTExpression* pIteration = ParseExpression( rtContext, pForScope );
	if( !pIteration )
	{
		return NULL;
	}

	if( rtContext.sNextToken.eToken != EShaderToken_Parenthesis_Close )
	{
		ParserError( rtContext, "Expected closing parenthesis");
	}
	ConsumeToken( rtContext );

	CASTStatement* pStatement = ParseStatement( rtContext, pForScope );

	if( !pStatement )
	{
		return NULL;
	}

	return new CASTForStatement( pInitialStatement, pCondition, pIteration, pStatement, pForScope );
}