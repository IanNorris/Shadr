#include "CompilationUnit.h"
#include "Tokens.h"
#include "Parser/ParserType.h"
#include "Utility/Error.h"
#include <algorithm>
#include <unordered_map>

void InitialiseBasicTypes( void )
{
	AddTypeDefinition( new CType( "void",  EScalarType_Void ) );

	AddTypeDefinition( new CType( "float",  EScalarType_Float, 0, 0 ) );
	AddTypeDefinition( new CType( "float1", EScalarType_Float, 0, 1 ) );
	AddTypeDefinition( new CType( "float2", EScalarType_Float, 0, 2 ) );
	AddTypeDefinition( new CType( "float3", EScalarType_Float, 0, 3 ) );
	AddTypeDefinition( new CType( "float4", EScalarType_Float, 0, 4 ) );

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

	AddTypeDefinition( new CType( "bool",  EScalarType_Bool, 0, 0 ) );
	AddTypeDefinition( new CType( "bool1", EScalarType_Bool, 0, 1 ) );
	AddTypeDefinition( new CType( "bool2", EScalarType_Bool, 0, 2 ) );
	AddTypeDefinition( new CType( "bool3", EScalarType_Bool, 0, 3 ) );
	AddTypeDefinition( new CType( "bool4", EScalarType_Bool, 0, 4 ) );
}
