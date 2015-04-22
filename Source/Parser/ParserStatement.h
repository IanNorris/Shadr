#if !defined( SHADR_PARSER_STATEMENT_H )
#define SHADR_PARSER_STATEMENT_H

CASTVariableDefinition* ParseVariableDefinition( SParseContext& rtContext, CScope* pParentScope );
CASTStatement* ParseStatement( SParseContext& rtContext, CScope* pParentScope );
CASTBlockStatement* ParseBlockStatement( SParseContext& rtContext, CScope* pParentScope );
CASTIfStatement* ParseIfStatement( SParseContext& rtContext, CScope* pParentScope );
CASTWhileStatement* ParseWhileStatement( SParseContext& rtContext, CScope* pParentScope );

#endif //SHADR_PARSER_STATEMENT_H
