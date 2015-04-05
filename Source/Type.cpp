#include "LLVM.h"
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

llvm::Type::TypeID CType::GetTypeID( EScalarType eType )
{
	switch( eType )
	{
		case EScalarType_Void:
			return llvm::Type::VoidTyID;

		case EScalarType_Bool:
			return llvm::Type::IntegerTyID;

		case EScalarType_Half:
			return llvm::Type::HalfTyID;

		case EScalarType_Float:
			return llvm::Type::FloatTyID;

		case EScalarType_Double:
			return llvm::Type::DoubleTyID;

		case EScalarType_Int:
			return llvm::Type::IntegerTyID;

		case EScalarType_UnsignedInt:
			return llvm::Type::IntegerTyID;

		case EScalarType_ConstantBuffer:
			return llvm::Type::StructTyID;

		case EScalarType_Buffer:
			return llvm::Type::ArrayTyID;

		case EScalarType_Struct:
			return llvm::Type::StructTyID;

		default:
			Assert( 0, "Unknown type %d", eType );
			return llvm::Type::VoidTyID;
	}
}

unsigned int CType::GetBitWidth( EScalarType eType )
{
	switch( eType )
	{
		case EScalarType_Bool:
			return 1;

		case EScalarType_Int:
			return 32;

		case EScalarType_UnsignedInt:
			return 32;

		default:
			return 0;
	}
}

bool CType::GetTypeSigned( EScalarType eType )
{
	switch( eType )
	{
		case EScalarType_Bool:
			return false;

		case EScalarType_UnsignedInt:
			return false;

		default:
			return true;
	}
}

llvm::Type* CType::CreateLLVMType()
{
	llvm::Type* pBasicType = GetLLVMBasicType();

	if( m_uVectorWidth )
	{
		llvm::Type* pVectorWidth = llvm::VectorType::get( pBasicType, m_uVectorWidth );

		if( m_uVectorHeight )
		{
			return llvm::VectorType::get( pVectorWidth, m_uVectorHeight );
		}
		else
		{
			return pVectorWidth;
		}
	}
	else
	{
		Assert( m_uVectorHeight == 0, "Vector height specified but no width" );

		return pBasicType;
	}
}

llvm::Type* CType::GetLLVMType()
{
	if( !m_pCachedType )
	{
		m_pCachedType = CreateLLVMType();
	}

	return m_pCachedType;
}

llvm::Type* CType::GetLLVMBasicType()
{
	switch( m_eType )
	{
		case EScalarType_Void:
			return llvm::Type::getVoidTy( llvm::getGlobalContext() );

		case EScalarType_Bool:
			return llvm::Type::getIntNTy( llvm::getGlobalContext(), 1 );

		case EScalarType_Half:
			return llvm::Type::getHalfTy( llvm::getGlobalContext() );

		case EScalarType_Float:
			return llvm::Type::getFloatTy( llvm::getGlobalContext() );

		case EScalarType_Double:
			return llvm::Type::getDoubleTy( llvm::getGlobalContext() );

		case EScalarType_Int:
			return llvm::Type::getIntNTy( llvm::getGlobalContext(), 32 );

		case EScalarType_UnsignedInt:
			return llvm::Type::getIntNTy( llvm::getGlobalContext(), 32 );

		case EScalarType_ConstantBuffer:
			return GetLLVMStructType();

		case EScalarType_Buffer:
			Assert( 0 , "Not implemented" );
			return llvm::Type::getVoidTy( llvm::getGlobalContext() );

		case EScalarType_Struct:
			return GetLLVMStructType();

		default:
			Assert( 0, "Unknown type %d", m_eType );
			return NULL;
	}
}

llvm::StructType* CType::GetLLVMStructType()
{
	/*llvm::Type** pTypes = new llvm::Type*[ m_tChildren.count() ];

	for( unsigned int uType = 0; uType < m_tChildren.count(); uType++ )
	{
		pTypes[ uType ] = m_tChildren[ uType ].pType;
	}

	llvm::StructType* pStruct = llvm::StructType::create( )

	delete[] pTypes;*/

	Assert( 0, "Not implemented" );

	return NULL;
}