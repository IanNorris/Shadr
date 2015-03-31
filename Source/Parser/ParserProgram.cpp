#include "LLVM.h"
#include "AST/AST.h"
#include "Parser.h"

CASTBlock* ParseProgram( SParseContext& rtContext )
{
	CASTBlock* pProgram = new CASTBlock();

	do 
	{
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

			CASTPrototype* pPrototype = ParsePrototype( rtContext, pReturnType, tTargetName );

			//We have a function body
			if( rtContext.sNextToken.eToken == EShaderToken_Brace_Open )
			{
				CASTFunction* pFunction = ParseFunction( rtContext, pPrototype );

				if( pFunction )
				{
					pProgram->Add( pFunction );
				}
				else
				{
					ParserError( rtContext, "Unexpected end of function" );
				}
			}
			else if( rtContext.sNextToken.eToken == EShaderToken_SemiColon )
			{
				pProgram->Add( pPrototype );
			}
		}

		ConsumeToken( rtContext );
	}
	while( rtContext.sNextToken.eToken != EShaderToken_Invalid );

	return pProgram;
}
