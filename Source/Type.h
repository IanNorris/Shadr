#if !defined( SHADR_TYPE_H )
#define SHADR_TYPE_H

#include "Reflection.h"

void InitialiseBasicTypes( void );

class CType;

inline bool operator==(const CType& lhs, const CType& rhs);

enum EScalarType
{
	EScalarType_Dummy,

	EScalarType_Void,

	EScalarType_Bool,

	EScalarType_Half,
	EScalarType_Float,
	EScalarType_Double,

	EScalarType_Int,
	EScalarType_UnsignedInt,

	EScalarType_ConstantBuffer,
	EScalarType_Buffer,
	EScalarType_Struct,

	EScalarType_MAX,
};

enum ETypeFlag
{
	ETypeFlag_Const				= 1 << 0,
	ETypeFlag_Precision_Full	= 1 << 1,
	ETypeFlag_Precision_Half	= 1 << 2,
	ETypeFlag_UNorm				= 1 << 3,
	ETypeFlag_SNorm				= 1 << 4,
	ETypeFlag_Reference			= 1 << 5,
	ETypeFlag_Intrinsic			= 1 << 6,
	ETypeFlag_Inline			= 1 << 7,
};

const char* GetNameFromScalarType( EScalarType eScalarType );

class CSemantic
{
	//TODO: update operator== below	
};

inline bool operator==(const CSemantic& lhs, const CSemantic& rhs)
{
	return true;
}

class CRegister
{

};

class CType : public CReflectionObject
{
public:

	CType( const std::string& rtTypeName, EScalarType eType, unsigned int uFlags = 0, unsigned int uVectorWidth = 1, unsigned int uVectorHeight = 1, unsigned int uArrayCount = 0, CType* pParentType = nullptr, CSemantic* pSemantic = nullptr, CRegister* pRegister = nullptr )
	: m_tName( rtTypeName )
	, m_pParentType( pParentType )
	, m_pSemantic( pSemantic )
	, m_pRegister( pRegister )
	, m_eType( eType )
	, m_uFlags( uFlags )
	, m_uVectorWidth( uVectorWidth )
	, m_uVectorHeight( uVectorHeight )
	, m_uArrayCount( uArrayCount )
	{
		Assert( uVectorWidth >= 1, "Vector width set to zero" );
		Assert( uVectorHeight >= 1, "Vector height set to zero" );

		AddReflection( "Name", EASTReflectionType_SString, &m_tName );
		AddReflection( "VectorWidth", EASTReflectionType_UInt, &m_uVectorWidth );
		AddReflection( "VectorHeight", EASTReflectionType_UInt, &m_uVectorHeight );
		AddReflection( "ArrayCount", EASTReflectionType_UInt, &m_uArrayCount );
		
		AddCondition( "IsConst", [&](){ return (m_uFlags & ETypeFlag_Const) != 0; } );
	}

	const char* GetElementName() const { return "Type"; }

	EScalarType GetScalarType( ) { return m_eType; }

	const std::string& GetTypeName();

	static const CType& GetFloatType()
	{
		static CType s_tFloat( "float", EScalarType_Float, 0 );
		return s_tFloat;
	}

	static const CType& GetConstFloatType()
	{
		static CType s_tFloat( "float", EScalarType_Float, ETypeFlag_Const );
		return s_tFloat;
	}

	static const CType& GetConstDoubleType()
	{
		static CType s_tDouble( "double", EScalarType_Double, ETypeFlag_Const );
		return s_tDouble;
	}

	static const CType& GetConstIntType()
	{
		static CType s_tInt( "int", EScalarType_Int, ETypeFlag_Const );
		return s_tInt;
	}

	static const CType& GetConstUIntType()
	{
		static CType s_tUInt( "uint", EScalarType_UnsignedInt, ETypeFlag_Const );
		return s_tUInt;
	}

	static const CType& GetBoolType()
	{
		static CType s_tBool( "bool", EScalarType_Bool, 0 );
		return s_tBool;
	}

	static const CType& GetVoidType()
	{
		static CType s_tVoid( "void", EScalarType_Void );
		return s_tVoid;
	}

	void SetScalarType( EScalarType eType ) { m_eType = eType; }

	bool CompareTo( const CType& other ) const
	{
		unsigned int compareFlagMask = ETypeFlag_Const | ETypeFlag_Precision_Full | ETypeFlag_Precision_Half | ETypeFlag_UNorm | ETypeFlag_SNorm;

		if(		m_eType != other.m_eType
			||	m_uFlags != other.m_uFlags
			||	m_uVectorWidth != other.m_uVectorWidth
			||	m_uVectorHeight != other.m_uVectorHeight
			||	m_uArrayCount != other.m_uArrayCount
			||  (m_uFlags & compareFlagMask) != (other.m_uFlags & compareFlagMask) )
		{
			return false;
		}

		if( !(*m_pSemantic == *other.m_pSemantic) )
		{
			return false;
		}

		if( m_tChildren.size() != other.m_tChildren.size() )
		{
			return false;
		}

		size_t uChildCount = m_tChildren.size();
		for( size_t uChild = 0; uChild < uChildCount; uChild++ )
		{
			if( !(*m_tChildren[ uChild ].pType == *other.m_tChildren[ uChild ].pType) )
			{
				return false;
			}
		}

		return true;
	}

	unsigned int GetVectorWidth() { return m_uVectorWidth; }
	unsigned int GetVectorHeight() { return m_uVectorHeight; }
	unsigned int GetArrayCount() { return m_uArrayCount; }

private:

	struct SChild
	{
		std::string tName;
		CType*		pType;
	};

	std::vector< SChild > m_tChildren;

	std::string		m_tName;

	CType*			m_pParentType;
	CSemantic*		m_pSemantic;
	CRegister*		m_pRegister;
	EScalarType		m_eType;
	unsigned int	m_uFlags;
	unsigned int	m_uVectorWidth;
	unsigned int	m_uVectorHeight;
	unsigned int	m_uArrayCount;
};

inline bool operator==(const CType& lhs, const CType& rhs)
{
	return lhs.CompareTo( rhs );
}

void AddTypeDefinition( CType* pType );
CType* GetType( const std::string& rtName );

#endif //SHADR_TYPE_H