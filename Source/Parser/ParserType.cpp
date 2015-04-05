#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"

#include <unordered_map>

std::unordered_map< std::string, CType* > g_tTypeDatabase;

void AddTypeDefinition( CType* pType )
{
	g_tTypeDatabase[ pType->GetTypeName() ] = pType;
}

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

			default:
				bFinishEatingModifiers = true;
		}
	}
	while( !bFinishEatingModifiers && ConsumeToken(rtContext ) );

	if( rtContext.sNextToken.eToken == EShaderToken_Identifier )
	{
		tTypeName = std::string( rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength );

		auto tIter = g_tTypeDatabase.find( tTypeName );
		if( tIter != g_tTypeDatabase.end() )
		{
			pParent = tIter->second;
		}
		else
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

	CType* pType = new CType( pParent->GetTypeName(), pParent->GetScalarType(), uTypeFlags, 0, 0, 0, pParent );
	return pType;
}
