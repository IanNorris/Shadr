#if !defined( SHADR_PARSER_H )
#define SHADR_PARSER_H

#include "Utility/Utility.h"
#include "Utility/Error.h"

#if defined( _DEBUG )
#define DEBUG_ONLY( ... ) __VA_ARGS__
#else
#define DEBUG_ONLY( ... )
#endif

#define ParserFatal( context, description, ... ) ReportErrorOnAmbiguousToken( context ); Error_Compiler( EError_Fatal, context.pszFilename, context.uCurrentRow, context.uCurrentCol, description DEBUG_ONLY(" (%s(%d))"), __VA_ARGS__ DEBUG_ONLY(, __FILE__, __LINE__) )
#define ParserError( context, description, ... ) ReportErrorOnAmbiguousToken( context ); Error_Compiler( EError_Error, context.pszFilename, context.uCurrentRow, context.uCurrentCol, description DEBUG_ONLY(" (%s(%d))"), __VA_ARGS__ DEBUG_ONLY(, __FILE__, __LINE__) )
#define ParserWarning( context, description, ... ) ReportErrorOnAmbiguousToken( context ); Error_Compiler( EError_Warning, context.pszFilename, context.uCurrentRow, context.uCurrentCol, description DEBUG_ONLY(" (%s(%d))"), __VA_ARGS__ DEBUG_ONLY(, __FILE__, __LINE__) )
#define ParserInfo( context, description, ... ) ReportErrorOnAmbiguousToken( context ); Error_Compiler( EError_Information, context.pszFilename, context.uCurrentRow, context.uCurrentCol, description DEBUG_ONLY(" (%s(%d))"), __VA_ARGS__ DEBUG_ONLY(, __FILE__, __LINE__) )
#define ParserDebug( context, description, ... ) ReportErrorOnAmbiguousToken( context ); Error_Compiler( EError_Debug, context.pszFilename, context.uCurrentRow, context.uCurrentCol, description DEBUG_ONLY(" (%s(%d))"), __VA_ARGS__ DEBUG_ONLY(, __FILE__, __LINE__) )

CASTProgram* ParseBuffer( const char* pszFilename, const std::string& tBuffer, CCompilationUnit* pCU, CScope* pParentScope );
CASTProgram* ParseFile( const char* pszFilename, CCompilationUnit* pCU, CScope* pParentScope );

void PushParseContext( SParseContext& rtContext );
SParseContext PopParseContext( void );

#include "ParserType.h"
#include "ParserExpression.h"
#include "ParserStatement.h"
#include "ParserFunction.h"
#include "ParserProgram.h"

#endif //SHADR_PARSER_H
