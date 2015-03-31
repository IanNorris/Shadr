#include "LLVM.h"
#include "AST.h"
#include "Utility/Error.h"

llvm::Value* CASTExpressionBinary::GenerateCode( CModule* pModule )
{
	CModule::TBuilder& rtBuilder = pModule->GetIRBuilder();

	llvm::Value* pLeft = m_pLeft->GenerateCode( pModule );
	llvm::Value* pRight = m_pRight->GenerateCode( pModule );

	Assert( pLeft && pRight, "Code generation failed at %s operator", GetTokenName( m_eOperator ) );

	EScalarType eType = GetType().GetScalarType();

	

	if( eType == EScalarType_Float )
	{
		switch( m_eOperator )
		{
			case EShaderToken_Binary_Bitwise_Shift_Left:
			case EShaderToken_Binary_Bitwise_Shift_Right:
			case EShaderToken_Binary_Bitwise_Or:
			case EShaderToken_Binary_Bitwise_And:
			case EShaderToken_Binary_Bitwise_Lazy_Or:
			case EShaderToken_Binary_Bitwise_Lazy_And:
				Assert( 0, "Bitwise operators are not valid on floating point values" );
				return NULL;

			case EShaderToken_Binary_Operator_Plus:
				return rtBuilder.CreateFAdd( pLeft, pRight );

			case EShaderToken_Binary_Operator_Minus:
				return rtBuilder.CreateFSub( pLeft, pRight );

			case EShaderToken_Binary_Operator_Divide:
				return rtBuilder.CreateFDiv( pLeft, pRight );

			case EShaderToken_Binary_Operator_Multiply:
				return rtBuilder.CreateFMul( pLeft, pRight );

			case EShaderToken_Binary_Operator_Modulo:
				Assert( 0, "Modulo operator is not valid on floating point values" );
				return NULL;

			case EShaderToken_Binary_Comparison_LT:
				GetType().SetScalarType( EScalarType_Bool );
				return rtBuilder.CreateFCmpOLT( pLeft, pRight );

			case EShaderToken_Binary_Comparison_GT:
				GetType().SetScalarType( EScalarType_Bool );
				return rtBuilder.CreateFCmpOGT( pLeft, pRight );

			case EShaderToken_Binary_Comparison_LTE:
				GetType().SetScalarType( EScalarType_Bool );
				return rtBuilder.CreateFCmpOLE( pLeft, pRight );

			case EShaderToken_Binary_Comparison_GTE:
				GetType().SetScalarType( EScalarType_Bool );
				return rtBuilder.CreateFCmpOGE( pLeft, pRight );

			case EShaderToken_Binary_Comparison_Equal:
				GetType().SetScalarType( EScalarType_Bool );
				return rtBuilder.CreateFCmpOEQ( pLeft, pRight );

			case EShaderToken_Binary_Comparison_NotEqual:
				GetType().SetScalarType( EScalarType_Bool );
				return rtBuilder.CreateFCmpONE( pLeft, pRight );
		}
	}
	else
	{
		Assert( 0, "Unsupported type" );
	}
	

	Assert( 0, "Unknown binary operator" );

	return NULL;
}

CType CASTExpressionBinary::EvaluateType( CASTExpression* pLeft, CASTExpression* pRight )
{
	return CType::GetConstFloatType();
}