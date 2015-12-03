#include "CompilationUnit.h"
#include "Tokens.h"
#include "Parser/ParserType.h"
#include "Utility/Error.h"
#include <algorithm>
#include <unordered_map>

std::unordered_map< std::string, CType* > g_tTypeDatabase;

void AddTypeDefinition( CType* pType )
{
	g_tTypeDatabase[ pType->GetTypeName() ] = pType;
}

void InitialiseBasicTypes( void )
{
	AddTypeDefinition( new CType( "void",  EScalarType_Void ) );

	AddTypeDefinition( new CType( "float",  EScalarType_Float, 0, 0 ) );
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

	AddTypeDefinition( new CType( "double",  EScalarType_Double, 0, 0 ) );
	AddTypeDefinition( new CType( "double1", EScalarType_Double, 0, 1 ) );
	AddTypeDefinition( new CType( "double2", EScalarType_Double, 0, 2 ) );
	AddTypeDefinition( new CType( "double3", EScalarType_Double, 0, 3 ) );
	AddTypeDefinition( new CType( "double4", EScalarType_Double, 0, 4 ) );

	AddTypeDefinition( new CType( "int",  EScalarType_Int, 0, 0 ) );
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


	AddTypeDefinition( new CType( "uint",  EScalarType_UnsignedInt, 0, 0 ) );
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

	AddTypeDefinition( new CType( "bool",  EScalarType_Bool, 0, 0 ) );
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