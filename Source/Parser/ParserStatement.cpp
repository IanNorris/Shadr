#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"

#define AnnotationCheckParamCount( expectedCountMin, expectedCountMax ) if( apParameters.size() < expectedCountMin || apParameters.size() > expectedCountMax ) { ParserError( rtContext, "%s annotation requires between %u and %u parameters, given %u", std::string(sAnnotation.pszToken,sAnnotation.uLength).c_str(), expectedCountMin, expectedCountMax, apParameters.size() ); }

CASTAnnotationGroup* ParseAnnotation( SParseContext& rtContext, CScope* pParentScope )
{
	if( rtContext.sNextToken.eToken != EShaderToken_Square_Open )
	{
		return NULL;
	}

	if( !ConsumeToken( rtContext ) )
	{
		ParserError( rtContext, "Unexpected end of file." );
		return NULL;
	}

	CASTAnnotationGroup* pNewGroup = new CASTAnnotationGroup( rtContext );

	std::vector<CASTExpression*> apParameters;
	
	while( true )
	{
		apParameters.clear();

		bool bFirst = true;

		EShaderToken eAnnotationToken = rtContext.sNextToken.eToken;
		SPossibleToken sAnnotation = rtContext.sNextToken;

		if( !ConsumeToken( rtContext ) )
		{
			ParserError( rtContext, "Unexpected end of file" );
			return pNewGroup;
		}

		if( rtContext.sNextToken.eToken == EShaderToken_Parenthesis_Open )
		{
			if( !ConsumeToken( rtContext ) )
			{
				ParserError( rtContext, "Unexpected end of file" );
			}

			while( true )
			{
				if( rtContext.sNextToken.eToken == EShaderToken_Parenthesis_Close )
				{
					break;
				}

				if( !bFirst )
				{
					if( rtContext.sNextToken.eToken == EShaderToken_Comma )
					{
						if( !ConsumeToken( rtContext ) )
						{
							ParserError( rtContext, "Unexpected end of file" );
						}
					}
					else
					{
						ParserError( rtContext, "Expected comma." );
					}
				}
				bFirst = false;

				CASTExpression* pExpression = ParseExpression( rtContext, pParentScope, INT_MAX, false, true );
				if( pExpression )
				{
					apParameters.push_back( pExpression );
				}
				else
				{
					ParserError( rtContext, "Expected expression." );
					break;
				}
			}

			if( !ConsumeToken( rtContext ) )
			{
				ParserError( rtContext, "Unexpected end of file" );
			}
		}

		switch( eAnnotationToken )
		{
			case EShaderToken_Annotation_SideEffect:
				AnnotationCheckParamCount( 0, 0 );
				pNewGroup->GetChildren().push_back( new CASTAnnotation( rtContext, EAnnotation_SideEffect, apParameters ) );
				break;

			case EShaderToken_Annotation_Unroll:
				AnnotationCheckParamCount( 0, 1 );
				pNewGroup->GetChildren().push_back( new CASTAnnotation( rtContext, EAnnotation_Unroll, apParameters ) );
				break;

			case EShaderToken_Annotation_ForceInline:
				AnnotationCheckParamCount( 0, 0 );
				pNewGroup->GetChildren().push_back( new CASTAnnotation( rtContext, EAnnotation_ForceInline, apParameters ) );
				break;
		}

		if( rtContext.sNextToken.eToken != EShaderToken_Comma )
		{
			if( rtContext.sNextToken.eToken != EShaderToken_Square_Close )
			{
				ParserError( rtContext, "Unexpected end of file." );
				return pNewGroup;
			}

			if( !ConsumeToken( rtContext ) )
			{
				ParserError( rtContext, "Unexpected end of file." );
				return pNewGroup;
			}

			//There may be many annotations strung together [like][this]
			if( rtContext.sNextToken.eToken == EShaderToken_Square_Open )
			{
				if( !ConsumeToken( rtContext ) )
				{
					ParserError( rtContext, "Unexpected end of file." );
					return pNewGroup;
				}
			}
			else
			{
				break;
			}
		}
	}

	return pNewGroup;

}

CASTVariableDefinition* ParseVariableDefinition( SParseContext& rtContext, CScope* pParentScope )
{
	//We're expecting to parse [Annotation] <Type> <Identifier> [ = <Expression> ]) [ (, <Identifier> [ = <Expression> ]))+ ]

	CType* pType = ParseType( rtContext );

	if( pType == nullptr || pType->GetScalarType() == EScalarType_Void )
	{
		return NULL;
	}

	CASTVariableDefinition* pVariableDef = nullptr;

	bool bContinue = false;
	do
	{
		CASTVariable* pVar = nullptr;

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
				pVar = new CASTVariable( rtContext, tName, *pType, nullptr );

				pVariableDef->GetVariables().push_back( pVar );
			}
			else
			{
				pVariableDef = new CASTVariableDefinition( rtContext, *pType, tName, false );
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
				pVar->SetExpressionStatement( new CASTExpressionStatement( rtContext, pExpression, true ) );

				pVariableDef->GetAssignments().push_back( pVar->GetAssignment() );
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

CASTStatement* ApplyAnnotation( CASTStatement* pStatement, CASTAnnotationGroup* pAnnotation )
{
	if( pStatement && pAnnotation )
	{
		pStatement->AddAnnotation( pAnnotation );
	}

	return pStatement;
}

CASTStatement* ParseStatement( SParseContext& rtContext, CScope* pParentScope )
{
	SParseContext tContextCopy = rtContext;

	CASTAnnotationGroup* pAnnotation = ParseAnnotation( rtContext, pParentScope );
	if( !pAnnotation )
	{
		rtContext = tContextCopy;
	}

	//Is it a block?
	if( rtContext.sNextToken.eToken == EShaderToken_Brace_Open )
	{
		ConsumeToken( rtContext );

		return ApplyAnnotation( ParseBlockStatement( rtContext, pParentScope ), pAnnotation );
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
			return ApplyAnnotation( new CASTReturnStatement( rtContext, pExpression ), pAnnotation );
		}
		else
		{
			return NULL;
		}
	}
	else if( rtContext.sNextToken.eToken == EShaderToken_If )
	{
		ConsumeToken( rtContext );

		return ApplyAnnotation( ParseIfStatement( rtContext, pParentScope ), pAnnotation );
	}
	else if( rtContext.sNextToken.eToken == EShaderToken_While )
	{
		ConsumeToken( rtContext );

		return ApplyAnnotation( ParseWhileStatement( rtContext, pParentScope ), pAnnotation );
	}
	else if( rtContext.sNextToken.eToken == EShaderToken_Do )
	{
		ConsumeToken( rtContext );

		return ApplyAnnotation( ParseDoWhileStatement( rtContext, pParentScope ), pAnnotation );
	}
	else if( rtContext.sNextToken.eToken == EShaderToken_For )
	{
		ConsumeToken( rtContext );

		return ApplyAnnotation( ParseForStatement( rtContext, pParentScope ), pAnnotation );
	}
	else if( rtContext.sNextToken.eToken == EShaderToken_SemiColon )
	{
		ConsumeToken( rtContext );

		return ApplyAnnotation( new CASTNopStatement( rtContext ), pAnnotation );
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

		return ApplyAnnotation( new CASTExpressionStatement( rtContext, pExpression, false ), pAnnotation );
	}

	rtContext = tContextCopy;

	CASTVariableDefinition* pDefinition = ParseVariableDefinition( rtContext, pParentScope );
	if( pDefinition )
	{
		return ApplyAnnotation( pDefinition, pAnnotation );
	}

	return NULL;
}

CASTBlockStatement* ParseBlockStatement( SParseContext& rtContext, CScope* pParentScope )
{
	CASTBlockStatement* pBlock = new CASTBlockStatement( rtContext, pParentScope );

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

	return new CASTIfStatement( rtContext, pCondition, pStatement, pElseStatement );
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

	return new CASTWhileStatement( rtContext, pCondition, pStatement );
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

	return new CASTDoWhileStatement( rtContext, pCondition, pStatement );
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

	return new CASTForStatement( rtContext, pInitialStatement, pCondition, pIteration, pStatement, pForScope );
}