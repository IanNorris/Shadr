#if !defined( SHADR_PARSER_EXPRESSION_H )
#define SHADR_PARSER_EXPRESSION_H

CASTExpression* ParseParenthesisExpression( SParseContext& rtContext, CScope* pParentScope );
CASTExpression* ParseBinaryExpressionRight( SParseContext& rtContext, int iLeftPrecedence, CASTExpression* pLeft, CScope* pParentScope );
CASTExpression* ParsePrimary( SParseContext& rtContext, CScope* pParentScope );
CASTExpression* ParseExpression( SParseContext& rtContext, CScope* pParentScope );

#endif //SHADR_PARSER_EXPRESSION_H
