#if !defined( SHADR_TYPE_H )
#define SHADR_TYPE_H

#include "Reflection.h"

void InitialiseBasicTypes( void );

class CType;
class CScope;
class CASTAnnotationGroup;
class CASTPrototype;

inline bool operator==(const CType& lhs, const CType& rhs);

enum EShaderType
{
	EShaderType_Vertex		= 1 << 0,
	EShaderType_Pixel		= 1 << 1,
	EShaderType_Geometry	= 1 << 2,
	EShaderType_Hull		= 1 << 3,
	EShaderType_Domain		= 1 << 4,
	EShaderType_Compute		= 1 << 5,

	EShaderType_Tesselation	= EShaderType_Geometry | EShaderType_Hull | EShaderType_Domain,
	EShaderType_All			= EShaderType_Vertex | EShaderType_Pixel | EShaderType_Geometry | EShaderType_Hull | EShaderType_Domain | EShaderType_Compute,
};

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

struct SSemantic
{
public:

	SSemantic()
	: pType( nullptr )
	, iSemanticIndex( -1 )
	, iIsOut( -1 )
	{}

	//TODO: update operator== below	

	std::string tSemanticName;
	std::string tNamedSemanticIndex;
	int			iSemanticIndex;
	
	int			iIsOut; //Negative if unset

	CType* pType;
};

inline bool operator==(const SSemantic& lhs, const SSemantic& rhs)
{
	bool bIsEqual = true;
	
	bIsEqual &= lhs.pType == rhs.pType;
	bIsEqual &= (lhs.tSemanticName.compare( rhs.tSemanticName ) == 0);
	bIsEqual &= (lhs.tNamedSemanticIndex.compare( rhs.tNamedSemanticIndex ) == 0);
	bIsEqual &= lhs.iIsOut == rhs.iIsOut || lhs.iIsOut == -1 || rhs.iIsOut == -1;

	return true;
}

//A semantic group is for example all semantics based off TEXCOORD.
//So the group would contain TEXCOORD0, TEXCOORD1, TEXCOORD2 and TEXCOORD(main)
//but an actual semantic is any one of those.
struct SSemanticGroup
{
	struct SEntry
	{
		SEntry() : iSemanticIndex( -1 ), tNamedSemanticIndex( "" ) {}
		SEntry( int index ) : iSemanticIndex( index ), tNamedSemanticIndex( "" ) {}
		SEntry( std::string name ) : iSemanticIndex( -1 ), tNamedSemanticIndex( name ) {}

		int iSemanticIndex;
		std::string tNamedSemanticIndex;

		std::vector< SSemantic* > attachedSemantics;
	};

	std::vector< SEntry > tEntries;
};

class CRegister
{

};

class CType : public CReflectionObject
{
public:

	CType( const std::string& rtTypeName, EScalarType eType, unsigned int uFlags = 0, unsigned int uVectorWidth = 1, unsigned int uVectorHeight = 1, unsigned int uArrayCount = 0, CType* pParentType = nullptr, SSemantic* pSemantic = nullptr, CRegister* pRegister = nullptr )
	: m_tName( rtTypeName )
	, m_pAnnotation( nullptr )
	, m_pScope( nullptr )
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

	const unsigned int GetFlags() { return m_uFlags; }

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

		//Do not do a deep comparison of types

		return true;
	}

	unsigned int GetVectorWidth() { return m_uVectorWidth; }
	unsigned int GetVectorHeight() { return m_uVectorHeight; }
	unsigned int GetArrayCount() { return m_uArrayCount; }

	void SetSemantic( SSemantic* pSemantic )
	{
		Assert( m_pSemantic == nullptr, "Semantic is already set" );

		m_pSemantic = pSemantic;
	}

	void SetAnnotation( CASTAnnotationGroup* pAnnotation )
	{
		m_pAnnotation = pAnnotation;
	}

	void CreateScope( CScope* pParentScope );

	CScope* GetScope()
	{
		return m_pScope;
	};

private:

	std::string		m_tName;

	CASTAnnotationGroup* m_pAnnotation;
	CScope*			m_pScope;
	CType*			m_pParentType;
	SSemantic*		m_pSemantic;
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

void AddSemantic( SSemantic* pSemantic );
void AddTypeDefinition( CType* pType );
CType* GetType( const std::string& rtName );

#endif //SHADR_TYPE_H