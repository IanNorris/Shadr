#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"

CASTVariableDefinition* ParseFunctionParameter( SParseContext& rtContext )
{
	//We're expecting to parse <Type> <Identifier> [ : <Semantic> ]

	CType* pType = ParseType( rtContext );

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

	if( rtContext.sNextToken.eToken == EShaderToken_Colon )
	{
		if( !ConsumeToken( rtContext ) )
		{
			ParserError( rtContext, "Expected end of file" );
		}

		ParserFatal( rtContext, "Encountered semantic, this is not implemented yet." );
	}

	return new CASTVariableDefinition( *pType, tName );
}

CASTPrototype* ParsePrototype( SParseContext& rtContext, CType* pReturnType, const std::string& rtFunctionName )
{
	CASTPrototype* pPrototype = new CASTPrototype( rtFunctionName.c_str(), rtFunctionName.length(), *pReturnType );

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

CASTFunction* ParseFunction( SParseContext& rtContext, CASTPrototype* pPrototype )
{
	if( rtContext.sNextToken.eToken != EShaderToken_Brace_Open )
	{
		ParserError( rtContext, "Expected opening brace ({)");
	}
	ConsumeToken( rtContext );

	CASTBlockStatement* pBlock = ParseBlockStatement( rtContext );

	if( pBlock )
	{
		return new CASTFunction( pPrototype, pBlock );
	}
	
	return NULL;
}
