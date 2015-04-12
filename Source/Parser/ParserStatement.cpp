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
				SVariable tVar;
				tVar.tName = tName;
				tVar.pType = pType;

				pVariableDef->GetVariables().push_back( tVar );
			}
			else
			{
				pVariableDef = new CASTVariableDefinition( *pType, tName );
			}

			pParentScope->AddVariable( rtContext, &pVariableDef->GetVariables().back() );

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
				pVariableDef->GetAssignments().push_back( new CASTExpressionStatement( pExpression ) );
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

		return new CASTExpressionStatement( pExpression );
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
