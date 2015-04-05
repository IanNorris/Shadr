#if !defined( SHADR_PARSER_H )
#define SHADR_PARSER_H

#include "Utility/Utility.h"
#include "Utility/Error.h"

#define ParserFatal( context, description, ... ) Error_Compiler( EError_Fatal, context.uCurrentRow, context.uCurrentCol, description, __VA_ARGS__ )
#define ParserError( context, description, ... ) Error_Compiler( EError_Error, context.uCurrentRow, context.uCurrentCol, description, __VA_ARGS__ )
#define ParserWarning( context, description, ... ) Error_Compiler( EError_Warning, context.uCurrentRow, context.uCurrentCol, description, __VA_ARGS__ )
#define ParserInfo( context, description, ... ) Error_Compiler( EError_Information, context.uCurrentRow, context.uCurrentCol, description, __VA_ARGS__ )
#define ParserDebug( context, description, ... ) Error_Compiler( EError_Debug, context.uCurrentRow, context.uCurrentCol, description, __VA_ARGS__ )

void ParseBuffer( const char* pszFilename, const std::string& tBuffer, CCompilationUnit* pCU );
void ParseFile( const char* pszFilename, CCompilationUnit* pCU );

void PushParseContext( SParseContext& rtContext );
SParseContext PopParseContext( void );

#include "ParserType.h"
#include "ParserExpression.h"
#include "ParserFunction.h"
#include "ParserProgram.h"

#endif //SHADR_PARSER_H
