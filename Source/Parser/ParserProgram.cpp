#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"

CASTProgram* ParseProgram( SParseContext& rtContext, CScope* pParentScope )
{
	CASTProgram* pProgram = new CASTProgram( rtContext, pParentScope );

	do 
	{
		SParseContext tContextCopy = rtContext;
		CASTAnnotationGroup* pAnnotation = ParseAnnotation( rtContext, pParentScope );
		if( !pAnnotation )
		{
			rtContext = tContextCopy;
		}

		SParseContext tPostAnnotationCopy = rtContext;

		if( ParseTypeDefinition( rtContext ) )
		{
			//Nothing to do here as it is done internally to the function on success.
		}
		else
		{
			rtContext = tPostAnnotationCopy;

			CType* pReturnType = ParseType( rtContext );

			//If there's no return type, it can't be a function
			if( !pReturnType )
			{
				ParserError( rtContext, "Expected global variable, type or function definition" );

				return NULL;
			}

			//Target name could be a function or global variable at this point
			std::string tTargetName( rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength );

			if( !ConsumeToken( rtContext ) )
			{
				ParserError( rtContext, "Unexpected end of file" );
			}

			//It's a prototype!
			if( rtContext.sNextToken.eToken == EShaderToken_Parenthesis_Open )
			{
				if( !ConsumeToken( rtContext ) )
				{
					ParserError( rtContext, "Unexpected end of file" );
				}

				CASTPrototype* pPrototype = ParsePrototype( rtContext, pReturnType, tTargetName, &pProgram->GetScope() );

				//We have a function body
				if( rtContext.sNextToken.eToken == EShaderToken_Brace_Open )
				{
					CASTFunction* pFunction = ParseFunction( rtContext, pPrototype, &pPrototype->GetScope() );

					if( pFunction )
					{
						if( pAnnotation )
						{
							pFunction->AddAnnotation( pAnnotation );
						}

						pProgram->AddElement( pFunction );
					}
					else
					{
						ParserError( rtContext, "Unexpected end of function" );
					}
				}
				else if( rtContext.sNextToken.eToken == EShaderToken_SemiColon )
				{
					if( pAnnotation )
					{
						pPrototype->AddAnnotation( pAnnotation );
					}

					pProgram->AddElement( pPrototype );

					if( !ConsumeToken( rtContext ) )
					{
						//EOF!
						return pProgram;
					}
				}
			}
			else
			{
				//Need to backtrack as variable definitions will consume the type themselves.
				rtContext = tPostAnnotationCopy;

				CASTVariableDefinition* pVariableDefinition = ParseVariableDefinition( rtContext, &pProgram->GetScope() );
				if( pVariableDefinition )
				{
					if( pAnnotation )
					{
						pVariableDefinition->AddAnnotation( pAnnotation );
					}

					pProgram->AddElement( pVariableDefinition );
				}
				else
				{
					ParserError( rtContext, "Expected global variable, type or function definition" );
					break;
				}
			}
		}
	}
	while( rtContext.sNextToken.eToken != EShaderToken_Invalid );

	return pProgram;
}
