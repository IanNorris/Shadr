#include "CompilationUnit.h"
#include "AST/AST.h"
#include "Parser.h"

#include <unordered_map>

struct SSemanticDescriptor
{
	const char*		pszName;
	bool			bNumbered;
	bool			bRead;
	bool			bWrite;
	EScalarType		eScalarType;
	int				iMaxVectorSize;
	int				iMaxArrayCount;
	unsigned int	uShaderTypes;
	bool			bDeprecated;
};

SSemanticDescriptor g_atSemanticDescriptors[] = 
{
	//Name							Numbered		Read	Write	Type					Vec		Array	ShaderTypes										Deprecated
	{ "BINORMAL",					true,			true,	false,	EScalarType_Float,			4,		1,	EShaderType_Vertex,								false },
	{ "BLENDINDICES",				true,			true,	false,	EScalarType_UnsignedInt,	4,		1,	EShaderType_Vertex,								false },
	{ "BLENDWEIGHT",				true,			true,	false,	EScalarType_Float,			1,		1,	EShaderType_Vertex,								false },
	{ "COLOR",						true,			true,	true,	EScalarType_Float,			4,		1,	EShaderType_Vertex | EShaderType_Pixel,			false },
	{ "DEPTH",						true,			false,	true,	EScalarType_Float,			1,		1,	EShaderType_Pixel,								false },
	{ "FOG",						false,			false,	true,	EScalarType_Float,			1,		1,	EShaderType_Vertex,								false },
	{ "NORMAL",						true,			true,	false,	EScalarType_Float,			4,		1,	EShaderType_Vertex,								false },
	{ "POSITION",					true,			true,	true,	EScalarType_Float,			4,		1,	EShaderType_Vertex,								false },
	{ "POSITIONT",					false,			true,	false,	EScalarType_Float,			4,		1,	EShaderType_Vertex,								false },
	{ "PSIZE",						true,			true,	true,	EScalarType_Float,			1,		1,	EShaderType_Vertex,								false },
	{ "TANGENT",					true,			true,	false,	EScalarType_Float,			4,		1,	EShaderType_Vertex,								false },
	{ "TEXCOORD",					true,			true,	true,	EScalarType_Float,			4,		1,	EShaderType_Vertex | EShaderType_Pixel,			false },
	{ "TESSFACTOR",					true,			false,	false,	EScalarType_Float,			1,		1,	EShaderType_Vertex,								false },
	{ "VFACE",						false,			true,	false,	EScalarType_Float,			1,		1,	EShaderType_Pixel,								true },
	{ "VPOS",						false,			true,	false,	EScalarType_Float,			2,		1,	EShaderType_Pixel,								true },

	{ "SV_ClipDistance",			true,			true,	true,	EScalarType_Float,			1,		1,	EShaderType_All,								false },
	{ "SV_CullDistance",			true,			true,	true,	EScalarType_Float,			1,		1,	EShaderType_All,								false },
	{ "SV_Coverage",				false,			true,	true,	EScalarType_UnsignedInt,	1,		1,	EShaderType_All,								false },
	{ "SV_Depth",					false,			true,	true,	EScalarType_Float,			1,		1,	EShaderType_All,								false },
	{ "SV_DepthGreaterEqual",		true,			true,	true,	EScalarType_Dummy,			1,		1,	EShaderType_All,								false },
	{ "SV_DepthLessEqual",			true,			true,	true,	EScalarType_Dummy,			1,		1,	EShaderType_All,								false },
	{ "SV_DispatchThreadID",		false,			true,	false,	EScalarType_UnsignedInt,	3,		1,	EShaderType_Compute,							false },
	{ "SV_DomainLocation",			false,			true,	false,	EScalarType_Float,			3,		1,	EShaderType_Domain,								false },
	{ "SV_GroupID",					false,			true,	false,	EScalarType_UnsignedInt,	3,		1,	EShaderType_Compute,							false },
	{ "SV_GroupIndex",				false,			true,	false,	EScalarType_UnsignedInt,	1,		1,	EShaderType_Compute,							false },
	{ "SV_GroupThreadID",			false,			true,	false,	EScalarType_UnsignedInt,	3,		1,	EShaderType_Compute,							false },
	{ "SV_GSInstanceID",			false,			true,	false,	EScalarType_UnsignedInt,	1,		1,	EShaderType_Geometry,							false },
	{ "SV_InnerCoverage",			false,			true,	true,	EScalarType_Dummy,			1,		1,	EShaderType_Pixel,								false },
	{ "SV_InsideTessFactor",		false,			false,	true,	EScalarType_Float,			1,		2,	EShaderType_Hull,								false },
	{ "SV_InsideTessFactor",		false,			true,	false,	EScalarType_Float,			1,		2,	EShaderType_Domain,								false },
	{ "SV_InstanceID",				false,			false,	true,	EScalarType_Dummy,			1,		1,	EShaderType_All,								false },
	{ "SV_IsFrontFace",				false,			false,	true,	EScalarType_Bool,			1,		1,	EShaderType_Geometry,							false },
	{ "SV_OutputControlPointID",	false,			true,	false,	EScalarType_UnsignedInt,	1,		1,	EShaderType_Hull,								false },

	//TODO: Implement interpolators linearNoPerspective, linearNoPerspectiveCentroid
	{ "SV_Position",				false,			true,	true,	EScalarType_Float,			4,		1,	EShaderType_All,								false },

	{ "SV_PrimitiveID",				false,			true,	false,	EScalarType_UnsignedInt,	1,		1,	EShaderType_Tesselation | EShaderType_Pixel,	false },
	{ "SV_PrimitiveID",				false,			false,	true,	EScalarType_UnsignedInt,	1,		1,	EShaderType_Geometry | EShaderType_Pixel,		false },
	{ "SV_RenderTargetArrayIndex",	false,			false,	true,	EScalarType_UnsignedInt,	1,		1,	EShaderType_Geometry,							false },
	{ "SV_RenderTargetArrayIndex",	false,			true,	false,	EScalarType_UnsignedInt,	1,		1,	EShaderType_Pixel,								false },
	{ "SV_SampleIndex",				false,			true,	true,	EScalarType_UnsignedInt,	1,		1,	EShaderType_Pixel,								false },
	{ "SV_StencilRef",				false,			false,	true,	EScalarType_UnsignedInt,	1,		1,	EShaderType_Pixel,								false },

	{ "SV_Target",					true,			false,	true,	EScalarType_Float,			4,		1,	EShaderType_All,								false },

	{ "SV_TessFactor",				false,			false,	true,	EScalarType_Float,			1,		1,	EShaderType_Hull,								false },
	{ "SV_TessFactor",				false,			true,	false,	EScalarType_Float,			1,		1,	EShaderType_Domain,								false },
	{ "SV_VertexID",				false,			true,	false,	EScalarType_UnsignedInt,	1,		1,	EShaderType_Vertex,								false },
	{ "SV_ViewportArrayIndex",		false,			true,	false,	EScalarType_UnsignedInt,	1,		1,	EShaderType_Geometry,							false },
	{ "SV_ViewportArrayIndex",		false,			true,	true,	EScalarType_UnsignedInt,	1,		1,	EShaderType_Pixel,								false },
};

SSemantic* ParseSemantic( SParseContext& rtContext, CType* pType )
{
	if (rtContext.sNextToken.eToken != EShaderToken_Identifier)
	{
		ParserError(rtContext, "Expected identifier." );
		return NULL;
	}

	std::string tIdentifier = std::string( rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength );

	if( !ConsumeToken( rtContext ) )
	{
		ParserError( rtContext, "Unexpected end of program." );
		return NULL;
	}

	SSemantic* pNewSemantic = new SSemantic();
	pNewSemantic->pType;
	pNewSemantic->tPartialName = tIdentifier;

	return pNewSemantic;
}

CType* ParseTypeDefinition( SParseContext& rtContext )
{
	if(		rtContext.sNextToken.eToken == EShaderToken_Struct
		||	rtContext.sNextToken.eToken == EShaderToken_Constant_Buffer )
	{
		EScalarType eScalarType = rtContext.sNextToken.eToken == EShaderToken_Struct ? EScalarType_Struct : EScalarType_ConstantBuffer;

		if( !ConsumeToken( rtContext ) )
		{
			ParserError( rtContext, "Unexpected end of program." );
			return NULL;
		}

		std::string tTypeName(rtContext.sNextToken.pszToken, rtContext.sNextToken.uLength);

		if (rtContext.sNextToken.eToken != EShaderToken_Identifier)
		{
			ParserError(rtContext, "Expected identifier." );
			return NULL;
		}

		if( !ConsumeToken( rtContext ) )
		{
			ParserError( rtContext, "Unexpected end of program." );
			return NULL;
		}

		if( rtContext.sNextToken.eToken != EShaderToken_Brace_Open )
		{
			ParserError(rtContext, "Expected an opening brace." );
			return NULL;
		}

		if( !ConsumeToken( rtContext ) )
		{
			ParserError( rtContext, "Unexpected end of program." );
			return NULL;
		}

		CType* pNewType = new CType( tTypeName, eScalarType );
		AddTypeDefinition(pNewType);

		while( CASTVariableDefinition* pVariableDef = ParseFunctionParameter( rtContext ) )
		{
			for( auto& pVar : pVariableDef->GetVariables() )
			{
				pNewType->AddChild( pVar->GetName(), &pVar->GetType() );
			}

			if (rtContext.sNextToken.eToken != EShaderToken_SemiColon)
			{
				ParserError(rtContext, "Expected a semi-colon.");
				return pNewType;
			}

			if (!ConsumeToken(rtContext))
			{
				ParserError(rtContext, "Unexpected end of program.");
				return pNewType;
			}
		}

		if (rtContext.sNextToken.eToken != EShaderToken_Brace_Close)
		{
			ParserError(rtContext, "Expected a closing brace." );
			return pNewType;
		}

		if (!ConsumeToken(rtContext))
		{
			ParserError(rtContext, "Unexpected end of program.");
			return pNewType;
		}

		if (rtContext.sNextToken.eToken != EShaderToken_SemiColon)
		{
			ParserError(rtContext, "Expected a semi-colon.");
			return pNewType;
		}

		if (!ConsumeToken(rtContext))
		{
			ParserError(rtContext, "Unexpected end of program.");
			return pNewType;
		}

		return pNewType;
	}
	else
	{
		return NULL;
	}
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
