#if !defined( SHADR_PARSER_EXPRESSION_H )
#define SHADR_PARSER_EXPRESSION_H

CASTExpression* ParseParenthesisExpression( SParseContext& rtContext, CScope* pParentScope );
CASTExpression* ParseBinaryExpressionRight( SParseContext& rtContext, int iLTRPrecedence, int iRTLPrecedence, CASTExpression* pLeft, CScope* pParentScope );
CASTExpression* ParsePrimary( SParseContext& rtContext, EShaderToken eToken, CScope* pParentScope );
CASTExpression* ParseExpression( SParseContext& rtContext, CScope* pParentScope, int iRTLPrecedence = INT_MAX );
CASTExpression* ParseFunctionCall( SParseContext& rtContext, const std::string& rtFunctionName, CScope* pParentScope );

#endif //SHADR_PARSER_EXPRESSION_H
