#if !defined( SHADR_PARSER_EXPRESSION_H )
#define SHADR_PARSER_EXPRESSION_H

CASTExpression* ParseParenthesisExpression( SParseContext& rtContext );
CASTExpression* ParseBinaryExpressionRight( SParseContext& rtContext, int iLeftPrecedence, CASTExpression* pLeft );
CASTExpression* ParsePrimary( SParseContext& rtContext );
CASTExpression* ParseExpression( SParseContext& rtContext );

#endif //SHADR_PARSER_EXPRESSION_H
