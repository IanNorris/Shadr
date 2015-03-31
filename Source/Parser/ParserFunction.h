#if !defined( SHADR_PARSER_FUNCTION_H )
#define SHADR_PARSER_FUNCTION_H

CASTVariableDefinition* ParseFunctionParameter( SParseContext& rtContext );
CASTPrototype* ParsePrototype( SParseContext& rtContext, CType* pReturnType, const std::string& rtFunctionName );
CASTFunction* ParseFunction( SParseContext& rtContext, CASTPrototype* pPrototype );

#endif //SHADR_PARSER_FUNCTION_H
