#include "CompilationUnit.h"
#include "AST.h"
#include "Utility/Error.h"

CType CASTExpressionUnary::EvaluateType( CASTExpression* pExpression )
{
	return CType::GetConstFloatType();
}

CType CASTExpressionBinary::EvaluateType( CASTExpression* pLeft, CASTExpression* pRight )
{
	return CType::GetConstFloatType();
}

CType CASTExpressionTernary::EvaluateType( CASTExpression* pCondition, CASTExpression* pTrue, CASTExpression* pFalse )
{
	return CType::GetConstFloatType();
}