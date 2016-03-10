#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"

CASTVariableDefinition* ParseFunctionParameter( SParseContext& rtContext )
{
	//We're expecting to parse <Type> <Identifier> [ : <Semantic> ]

	CType* pType = ParseType( rtContext );

	if( pType == nullptr )
	{
		return NULL;
	}

	if( pType->GetScalarType() == EScalarType_Void )
	{
		return NULL;
	}

	std::string tName;

	if( rtContext.sNextToken.eToken != EShaderToken_Identifier )
	{
		ParserError( rtContext, "Expected identifier" );
		tName = "!dummy";
	}
	else
	{
		tName = std::string( rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength );
	}

	if( !ConsumeToken( rtContext ) )
	{
		ParserError( rtContext, "Expected end of file" );
	}

	SSemantic* pSemantic = nullptr;

	if( rtContext.sNextToken.eToken == EShaderToken_Colon )
	{
		if( !ConsumeToken( rtContext ) )
		{
			ParserError( rtContext, "Expected end of file" );
		}

		pSemantic = ParseSemantic( rtContext, pType );

		if( pSemantic )
		{
			AddSemantic( pSemantic );
		}
	}

	pType->SetSemantic( pSemantic );

	return new CASTVariableDefinition( rtContext, *pType, tName, true );
}

CASTPrototype* ParsePrototype( SParseContext& rtContext, CType* pReturnType, const std::string& rtFunctionName, CScope* pParentScope )
{
	CASTPrototype* pPrototype = new CASTPrototype( rtContext, rtFunctionName.c_str(), rtFunctionName.length(), *pReturnType, pParentScope, (pReturnType->GetFlags() & ETypeFlag_Intrinsic) != 0, (pReturnType->GetFlags() & ETypeFlag_Inline) != 0 );
	pParentScope->AddPrototype( rtContext, pPrototype );

	if( rtContext.sNextToken.eToken != EShaderToken_Parenthesis_Close )
	{
		bool bContinue;
		do
		{
			bContinue = false;

			CASTVariableDefinition* pParameter = ParseFunctionParameter( rtContext );
			if( pParameter )
			{
				pPrototype->AddParameter( pParameter );

				pPrototype->GetScope().AddVariable( rtContext, pParameter->GetVariables()[0] );
			}
			else
			{
				//ParseFunctionParameter will return NULL if it encounters the void type,
				//but we need to validate that we didn't get any more parameters before it.
				//If there was any kind of parsing error it would have already reported that
				//to the user.

				if( !pPrototype->GetParameters().empty() )
				{
					ParserError( rtContext, "Malformed parameter list for function %s", rtFunctionName.c_str() );
				}

				break;
			}

			if( rtContext.sNextToken.eToken == EShaderToken_Comma )
			{
				if( !ConsumeToken( rtContext ) )
				{
					ParserError( rtContext, "Expected end of file" );
				}

				bContinue = true;
			}
		}
		while( bContinue );

		if( rtContext.sNextToken.eToken != EShaderToken_Parenthesis_Close )
		{
			ParserError( rtContext, "Expected end of function prototype" );
		}
	}

	if( !ConsumeToken( rtContext ) )
	{
		ParserError( rtContext, "Expected end of file" );
	}

	return pPrototype;
}

CASTFunction* ParseFunction( SParseContext& rtContext, CASTPrototype* pPrototype, CScope* pGlobalScope )
{
	if( rtContext.sNextToken.eToken != EShaderToken_Brace_Open )
	{
		ParserError( rtContext, "Expected opening brace ({)");
	}
	ConsumeToken( rtContext );

	CASTBlockStatement* pBlock = ParseBlockStatement( rtContext, pGlobalScope );

	if( pBlock )
	{
		CASTFunction* pFunction = new CASTFunction( rtContext, pPrototype, pBlock );
		pGlobalScope->GetParentScope()->AddFunction( rtContext, pFunction );
		return pFunction;
	}
	
	return NULL;
}
