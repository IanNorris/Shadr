#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"

#include <unordered_map>



CType* ParseTypeDefinition( SParseContext& rtContext )
{

	//Add type

	//AddTypeDefinition( pType );

	return NULL;
}

CType* ParseType( SParseContext& rtContext )
{
	SParseContext tContextCopy = rtContext;

	unsigned int uTypeFlags = 0;
	EScalarType eType = EScalarType_MAX;
	std::string tTypeName;
	CType* pParent = nullptr;

	bool bFinishEatingModifiers = false;
	do
	{
		bool bAteSomething = true;

		switch( rtContext.sNextToken.eToken )
		{
			case EShaderToken_Const:
				uTypeFlags |= ETypeFlag_Const;
				break;

			case EShaderToken_Modifier_Precision_Full:
				uTypeFlags |= ETypeFlag_Precision_Full;
				break;

			case EShaderToken_Modifier_Precision_Half:
				uTypeFlags |= ETypeFlag_Precision_Half;
				break;

			case EShaderToken_Modifier_Precision_UNorm:
				uTypeFlags |= ETypeFlag_UNorm;
				break;

			case EShaderToken_Modifier_Precision_SNorm:
				uTypeFlags |= ETypeFlag_SNorm;
				break;

			case EShaderToken_In:
				//Token is syntactic sugar only
				break;

			case EShaderToken_Out:
				uTypeFlags |= ETypeFlag_Reference;
				break;

			case EShaderToken_Intrinsic:
				uTypeFlags |= ETypeFlag_Intrinsic;
				break;

			case EShaderToken_Inline:
				uTypeFlags |= ETypeFlag_Inline;
				break;

			default:
				bAteSomething = false;
				bFinishEatingModifiers = true;
		}

		if( bAteSomething )
		{
			if( !ConsumeToken( rtContext ) )
			{
				return nullptr;
			}
		}
	}
	while( !bFinishEatingModifiers );

	if( rtContext.sNextToken.eToken == EShaderToken_Identifier )
	{
		tTypeName = std::string( rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength );

		pParent = GetType( tTypeName );
		if( !pParent )
		{
			ParserError( rtContext, "Type %s is not defined", tTypeName.c_str() );
			return NULL;
		}

		ConsumeToken( rtContext );
	}
	else
	{
		ParserError( rtContext, "Expected type" );
		rtContext = tContextCopy;
		return NULL;
	}

	if( rtContext.sNextToken.eToken == EShaderToken_Binary_Bitwise_And )
	{
		uTypeFlags |= ETypeFlag_Reference;

		ConsumeToken( rtContext );
	}

	CType* pType = new CType( pParent->GetTypeName(), pParent->GetScalarType(), uTypeFlags, pParent->GetVectorWidth(), pParent->GetVectorHeight(), pParent->GetArrayCount(), pParent );
	return pType;
}
