#if !defined( SHADR_PARSER_H )
#define SHADR_PARSER_H

void ParseBuffer( const char* pszFilename, const std::string& tBuffer, CModule* pModule );
void ParseFile( const char* pszFilename, CModule* pModule );

#include "ParserExpression.h"
#include "ParserFunction.h"
#include "ParserProgram.h"

#endif //SHADR_PARSER_H
