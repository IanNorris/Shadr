#include "CompilationUnit.h"
#include "Tokens.h"
#include "Parser/ParserType.h"
#include "Utility/Error.h"
#include <algorithm>
#include <unordered_map>

const char* GetNameFromScalarType( EScalarType eScalarType )
{
	switch( eScalarType )
	{
	case EScalarType_Void:
		return "void";

	case EScalarType_Bool:
		return "bool";

	case EScalarType_Half:
		return "half";

	case EScalarType_Float:
		return "float";

	case EScalarType_Double:
		return "double";

	case EScalarType_Int:
		return "int";

	case EScalarType_UnsignedInt:
		return "uint";

	case EScalarType_Dummy:
	case EScalarType_ConstantBuffer:
	case EScalarType_Buffer:
	case EScalarType_Struct:
	case EScalarType_MAX:
	default:
		return "<unknown>";
	}
}

std::unordered_map< std::string, CType* > g_tTypeDatabase;

std::unordered_map< std::string, CSemanticGroup* > g_tSemanticDatabase;

void AddTypeDefinition( CType* pType )
{
	g_tTypeDatabase[ pType->GetTypeName() ] = pType;
}

void InitialiseBasicTypes( void )
{
	AddTypeDefinition( new CType( "void",  EScalarType_Void ) );

	AddTypeDefinition( new CType( "float",  EScalarType_Float, 0, 1 ) );
	AddTypeDefinition( new CType( "float1", EScalarType_Float, 0, 1 ) );
	AddTypeDefinition( new CType( "float2", EScalarType_Float, 0, 2 ) );
	AddTypeDefinition( new CType( "float3", EScalarType_Float, 0, 3 ) );
	AddTypeDefinition( new CType( "float4", EScalarType_Float, 0, 4 ) );

	AddTypeDefinition( new CType( "float2x2",  EScalarType_Float, 0, 2, 2 ) );
	AddTypeDefinition( new CType( "float2x3",  EScalarType_Float, 0, 2, 3 ) );
	AddTypeDefinition( new CType( "float2x4",  EScalarType_Float, 0, 2, 4 ) );

	AddTypeDefinition( new CType( "float3x2",  EScalarType_Float, 0, 3, 2 ) );
	AddTypeDefinition( new CType( "float3x3",  EScalarType_Float, 0, 3, 3 ) );
	AddTypeDefinition( new CType( "float3x4",  EScalarType_Float, 0, 3, 4 ) );

	AddTypeDefinition( new CType( "float4x2",  EScalarType_Float, 0, 4, 2 ) );
	AddTypeDefinition( new CType( "float4x3",  EScalarType_Float, 0, 4, 3 ) );
	AddTypeDefinition( new CType( "float4x4",  EScalarType_Float, 0, 4, 4 ) );

	AddTypeDefinition( new CType( "matrix",  EScalarType_Float, 0, 4, 4 ) );
	AddTypeDefinition( new CType( "matrix4x4",  EScalarType_Float, 0, 4, 4 ) );
	AddTypeDefinition( new CType( "matrix4x3",  EScalarType_Float, 0, 4, 3 ) );

	AddTypeDefinition( new CType( "double",  EScalarType_Double, 0, 1 ) );
	AddTypeDefinition( new CType( "double1", EScalarType_Double, 0, 1 ) );
	AddTypeDefinition( new CType( "double2", EScalarType_Double, 0, 2 ) );
	AddTypeDefinition( new CType( "double3", EScalarType_Double, 0, 3 ) );
	AddTypeDefinition( new CType( "double4", EScalarType_Double, 0, 4 ) );

	AddTypeDefinition( new CType( "int",  EScalarType_Int, 0, 1 ) );
	AddTypeDefinition( new CType( "int1", EScalarType_Int, 0, 1 ) );
	AddTypeDefinition( new CType( "int2", EScalarType_Int, 0, 2 ) );
	AddTypeDefinition( new CType( "int3", EScalarType_Int, 0, 3 ) );
	AddTypeDefinition( new CType( "int4", EScalarType_Int, 0, 4 ) );

	AddTypeDefinition( new CType( "int2x2",  EScalarType_Int, 0, 2, 2 ) );
	AddTypeDefinition( new CType( "int2x3",  EScalarType_Int, 0, 2, 3 ) );
	AddTypeDefinition( new CType( "int2x4",  EScalarType_Int, 0, 2, 4 ) );
	AddTypeDefinition( new CType( "int3x2",  EScalarType_Int, 0, 3, 2 ) );
	AddTypeDefinition( new CType( "int3x3",  EScalarType_Int, 0, 3, 3 ) );
	AddTypeDefinition( new CType( "int3x4",  EScalarType_Int, 0, 3, 4 ) );
	AddTypeDefinition( new CType( "int4x2",  EScalarType_Int, 0, 4, 2 ) );
	AddTypeDefinition( new CType( "int4x3",  EScalarType_Int, 0, 4, 3 ) );
	AddTypeDefinition( new CType( "int4x4",  EScalarType_Int, 0, 4, 4 ) );


	AddTypeDefinition( new CType( "uint",  EScalarType_UnsignedInt, 0, 1 ) );
	AddTypeDefinition( new CType( "uint1", EScalarType_UnsignedInt, 0, 1 ) );
	AddTypeDefinition( new CType( "uint2", EScalarType_UnsignedInt, 0, 2 ) );
	AddTypeDefinition( new CType( "uint3", EScalarType_UnsignedInt, 0, 3 ) );
	AddTypeDefinition( new CType( "uint4", EScalarType_UnsignedInt, 0, 4 ) );

	AddTypeDefinition( new CType( "uint2x2", EScalarType_UnsignedInt, 0, 2, 2 ) );
	AddTypeDefinition( new CType( "uint2x3", EScalarType_UnsignedInt, 0, 2, 3 ) );
	AddTypeDefinition( new CType( "uint2x4", EScalarType_UnsignedInt, 0, 2, 4 ) );
	AddTypeDefinition( new CType( "uint3x2", EScalarType_UnsignedInt, 0, 3, 2 ) );
	AddTypeDefinition( new CType( "uint3x3", EScalarType_UnsignedInt, 0, 3, 3 ) );
	AddTypeDefinition( new CType( "uint3x4", EScalarType_UnsignedInt, 0, 3, 4 ) );
	AddTypeDefinition( new CType( "uint4x2", EScalarType_UnsignedInt, 0, 4, 2 ) );
	AddTypeDefinition( new CType( "uint4x3", EScalarType_UnsignedInt, 0, 4, 3 ) );
	AddTypeDefinition( new CType( "uint4x4", EScalarType_UnsignedInt, 0, 4, 4 ) );

	AddTypeDefinition( new CType( "bool",  EScalarType_Bool, 0, 1 ) );
	AddTypeDefinition( new CType( "bool1", EScalarType_Bool, 0, 1 ) );
	AddTypeDefinition( new CType( "bool2", EScalarType_Bool, 0, 2 ) );
	AddTypeDefinition( new CType( "bool3", EScalarType_Bool, 0, 3 ) );
	AddTypeDefinition( new CType( "bool4", EScalarType_Bool, 0, 4 ) );
}

CType* GetType( const std::string& rtName )
{
	auto tIter = g_tTypeDatabase.find( rtName );
	if( tIter != g_tTypeDatabase.end() )
	{
		return tIter->second;
	}

	return NULL;
}

const std::string& CType::GetTypeName()
{
	char szTemp[ 32 ] = {0};
	if( m_uVectorWidth > 1 )
	{
		if( m_uVectorHeight > 1 )
		{
			sprintf_s( szTemp, "%dx%d", m_uVectorWidth, m_uVectorHeight );
		}
		else
		{
			sprintf_s( szTemp, "%d", m_uVectorWidth );
		}
	}

	switch( m_eType )
	{
		case EScalarType_Dummy:
			m_tName = "int";
			m_tName += szTemp;
			break;

		case EScalarType_Void:
			m_tName = "void";
			Assert( m_uVectorWidth == 1 && m_uVectorHeight == 1, "Void type cannot be a vector or matrix" );
			break;

		case EScalarType_Bool:
			m_tName = "bool";
			m_tName += szTemp;
			break;

		case EScalarType_Half:
			m_tName = "half";
			m_tName += szTemp;
			break;

		case EScalarType_Float:
			m_tName = "float";
			m_tName += szTemp;
			break;

		case EScalarType_Double:
			m_tName = "double";
			m_tName += szTemp;
			break;

		case EScalarType_Int:
			m_tName = "int";
			m_tName += szTemp;
			break;

		case EScalarType_UnsignedInt:
			m_tName = "uint";
			m_tName += szTemp;
			break;

		case EScalarType_ConstantBuffer:
		case EScalarType_Buffer:
		case EScalarType_Struct:
			//TODO: Leave these alone for now
			Assert( m_uVectorWidth == 1 && m_uVectorHeight == 1, "Complex types cannot be vectors or matrices" );
			break;

		case EScalarType_MAX:
		default:
			Assert( 0, "Unrecognised type" );
	}

	return m_tName;
}