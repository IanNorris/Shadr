#include "CompilationUnit.h"
#include "AST.h"
#include "Utility/Error.h"
#include "Parser/Parser.h"
#include <minmax.h>

bool CanDoBitwiseOperation( EScalarType eScalarType )
{
	return eScalarType == EScalarType_Int || eScalarType == EScalarType_UnsignedInt || eScalarType == EScalarType_Bool;
}

#define TypeConversion( firstType, secondType, action )											\
	if(		(tLeft.GetScalarType()  == firstType && tRight.GetScalarType() == secondType )		\
		||	(tRight.GetScalarType() == firstType && tLeft.GetScalarType()  == secondType) )		\
	{																							\
		action;																					\
	}

#define TypeConversionNot( firstType, secondType, action )										\
	if(		(tLeft.GetScalarType()  == firstType && tRight.GetScalarType() != secondType )		\
		||	(tRight.GetScalarType() == firstType && tLeft.GetScalarType()  != secondType) )		\
	{																							\
		action;																					\
	}

CType GetBestTypeFromTypePair( SParsePosition& parserPos, CType tLeft, CType tRight, bool bError, bool bAllowWarnings, bool& bSuccess )
{
	unsigned int leftWidth = tLeft.GetVectorWidth();
	unsigned int rightWidth = tRight.GetVectorWidth();

	if( tLeft.GetArrayCount() > 1 || tRight.GetArrayCount() > 1 )
	{
		if( bError )
		{
			Error_Compiler( EError_Error, parserPos.pszFilename, parserPos.uCurrentRow, parserPos.uCurrentCol, "Use of array like a pointer, this is not supported." );
		}
		bSuccess = false;
	}

	int width = leftWidth;

	if( leftWidth != rightWidth )
	{
		if( leftWidth == 1 || rightWidth == 1 )
		{
			width = max( leftWidth, rightWidth );
		}
		else
		{
			if( bError )
			{
				Error_Compiler( EError_Warning, parserPos.pszFilename, parserPos.uCurrentRow, parserPos.uCurrentCol, "Implicit conversion from %s to %s", tLeft.GetTypeName().c_str(), tRight.GetTypeName().c_str() );
			}
			if( !bAllowWarnings )
			{
				bSuccess = false;
			}
		}
	}

	unsigned int leftHeight = tLeft.GetVectorHeight();
	unsigned int rightHeight = tRight.GetVectorHeight();

	int height = leftHeight;

	if( leftHeight != rightHeight )
	{
		if( leftHeight == 1 || rightHeight == 1 )
		{
			height = max( leftHeight, rightHeight );
		}
		else
		{
			if( bError )
			{
				Error_Compiler( EError_Warning, parserPos.pszFilename, parserPos.uCurrentRow, parserPos.uCurrentCol, "Implicit conversion from %s to %s", tLeft.GetTypeName().c_str(), tRight.GetTypeName().c_str() );
			}
			if( !bAllowWarnings )
			{
				bSuccess = false;
			}
		}
	}

	EScalarType eNewScalarType = tLeft.GetScalarType();

	if( tLeft.GetScalarType() != tRight.GetScalarType() )
	{
		//Disallow any implicit conversion to bool
		TypeConversionNot( EScalarType_Bool, EScalarType_Bool, if( bError){ Error_Compiler( EError_Error, parserPos.pszFilename, parserPos.uCurrentRow, parserPos.uCurrentCol, "Implicit conversion to bool is not allowed." ); } bSuccess = false; )
		else TypeConversion( EScalarType_Bool, EScalarType_Bool, eNewScalarType = EScalarType_Bool )
		else TypeConversion( EScalarType_Double, EScalarType_Float, eNewScalarType = EScalarType_Double )
		else TypeConversion( EScalarType_Double, EScalarType_Half, eNewScalarType = EScalarType_Double )
		else TypeConversion( EScalarType_Double, EScalarType_Int, eNewScalarType = EScalarType_Double )
		else TypeConversion( EScalarType_Double, EScalarType_UnsignedInt, eNewScalarType = EScalarType_Double )
		else TypeConversion( EScalarType_Float, EScalarType_Float, eNewScalarType = EScalarType_Float )
		else TypeConversion( EScalarType_Float, EScalarType_Half, eNewScalarType = EScalarType_Float )
		else TypeConversion( EScalarType_Float, EScalarType_Int, eNewScalarType = EScalarType_Float )
		else TypeConversion( EScalarType_Float, EScalarType_UnsignedInt, eNewScalarType = EScalarType_Float )
		else TypeConversion( EScalarType_Int, EScalarType_UnsignedInt, if( bError ) { Error_Compiler( EError_Warning, parserPos.pszFilename, parserPos.uCurrentRow, parserPos.uCurrentCol, "Conversion between unsigned int and int could cause data loss." ); } if( !bAllowWarnings ){ bSuccess = false; } )
		else
		{
			if( bError )
			{
				Error_Compiler( EError_Error, parserPos.pszFilename, parserPos.uCurrentRow, parserPos.uCurrentCol, "Unrecognised type coercion between %s and %s.", tLeft.GetTypeName().c_str(), tRight.GetTypeName().c_str() );
			}
			bSuccess = false;
		}
	}

	//TODO flags
	unsigned int uFlags = 0;
	
	return CType( GetNameFromScalarType( eNewScalarType ), eNewScalarType, uFlags, width, height, 0 );
}

bool CanCoerceType( const CType& from, const CType& to, bool bAllowWarnings )
{
	SParsePosition tDummy;
	bool bSuccess = true;
	GetBestTypeFromTypePair( tDummy, from, to, false, bAllowWarnings, bSuccess );

	return bSuccess;
}

CType CASTExpressionUnary::EvaluateType( CASTExpression* pExpression )
{
	switch( m_eOperator )
	{
	case EShaderToken_Unary_Not:
	case EShaderToken_Unary_Bitwise_Not:
		return CType::GetBoolType(); 

	default:
		return pExpression->GetType();
	}
}

CType CASTExpressionBinary::EvaluateType( CASTExpression* pLeft, CASTExpression* pRight )
{
	switch( m_eOperator )
	{
	case EShaderToken_Binary_Bitwise_Shift_Left:
	case EShaderToken_Binary_Bitwise_Shift_Right:
	case EShaderToken_Binary_Bitwise_Or:
	case EShaderToken_Binary_Bitwise_And:
	case EShaderToken_Binary_Bitwise_Lazy_Or:
	case EShaderToken_Binary_Bitwise_Lazy_And:
		{
			if( !CanDoBitwiseOperation( pLeft->GetType().GetScalarType() ) )
			{
				Error_Compiler( EError_Error, GetParserPosition().pszFilename, GetParserPosition().uCurrentRow, GetParserPosition().uCurrentCol, "%s cannot be used for a bitwise operation", pLeft->GetType().GetTypeName().c_str() );
			}
			else if( !CanDoBitwiseOperation( pRight->GetType().GetScalarType() ) )
			{
				Error_Compiler( EError_Error, GetParserPosition().pszFilename, GetParserPosition().uCurrentRow, GetParserPosition().uCurrentCol, "%s cannot be used for a bitwise operation", pRight->GetType().GetTypeName().c_str() );
			}

			bool bSuccess = false;
			return GetBestTypeFromTypePair( GetParserPosition(), pLeft->GetType(), pRight->GetType(), true, false, bSuccess );
	}
	
	case EShaderToken_Binary_Operator_Plus:
	case EShaderToken_Binary_Operator_Minus:
	case EShaderToken_Binary_Operator_Divide:
	case EShaderToken_Binary_Operator_Multiply:
	case EShaderToken_Binary_Operator_Modulo:
		{
			bool bSuccess = false;
			return GetBestTypeFromTypePair( GetParserPosition(), pLeft->GetType(), pRight->GetType(), true, false, bSuccess );
		}

	case EShaderToken_Binary_Comparison_LT:
	case EShaderToken_Binary_Comparison_GT:
	case EShaderToken_Binary_Comparison_LTE:
	case EShaderToken_Binary_Comparison_GTE:
	case EShaderToken_Binary_Comparison_Equal:
	case EShaderToken_Binary_Comparison_NotEqual:
		return CType::GetBoolType();

	case EShaderToken_Assign:
	case EShaderToken_Assign_Add:
	case EShaderToken_Assign_Subtract:
	case EShaderToken_Assign_Divide:
	case EShaderToken_Assign_Multiply:
	case EShaderToken_Assign_Modulo:
		{
			bool bSuccess = false;
			return GetBestTypeFromTypePair( GetParserPosition(), pLeft->GetType(), pRight->GetType(), true, false, bSuccess );
		}

	case EShaderToken_Assign_Shift_Left:
	case EShaderToken_Assign_Shift_Right:

	case EShaderToken_Unary_Not:
	case EShaderToken_Unary_Bitwise_Not:
		return CType::GetBoolType(); 

	case EShaderToken_Dot:
		{
			auto pSwizzle = dynamic_cast<CASTExpressionSwizzleMask*>(pRight);
			if( pSwizzle )
			{
				return CType( GetNameFromScalarType( pLeft->GetType().GetScalarType() ), pLeft->GetType().GetScalarType(), 0, pSwizzle->GetSwizzleSize() );
			}
			else
			{
				Error_Compiler( EError_Error, GetParserPosition().pszFilename, GetParserPosition().uCurrentRow, GetParserPosition().uCurrentCol, "Unrecognised RHS of dot operator %s", pRight->GetElementName() );
			}
		}

	default:
		Error_Compiler( EError_Error, GetParserPosition().pszFilename, GetParserPosition().uCurrentRow, GetParserPosition().uCurrentCol, "Unrecognised binary operator %d", m_eOperator );
		break;
	}

	return CType::GetConstFloatType();
}

CType CASTExpressionTernary::EvaluateType( CASTExpression* pCondition, CASTExpression* pTrue, CASTExpression* pFalse )
{
	bool bSuccess = false;
	return GetBestTypeFromTypePair( GetParserPosition(), pTrue->GetType(), pFalse->GetType(), true, false, bSuccess );
}