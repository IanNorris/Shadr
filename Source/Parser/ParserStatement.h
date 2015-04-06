#if !defined( SHADR_PARSER_STATEMENT_H )
#define SHADR_PARSER_STATEMENT_H

CASTStatement* ParseStatement( SParseContext& rtContext );
CASTBlockStatement* ParseBlockStatement( SParseContext& rtContext );

#endif //SHADR_PARSER_STATEMENT_H
