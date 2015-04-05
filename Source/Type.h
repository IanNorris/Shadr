#if !defined( SHADR_TYPE_H )
#define SHADR_TYPE_H

void InitialiseBasicTypes( void );

enum EScalarType
{
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
};

class CSemantic
{
	
};

class CRegister
{

};

class CType
{
public:

	CType( const std::string& rtTypeName, EScalarType eType, unsigned int uFlags = 0, unsigned int uVectorWidth = 0, unsigned int uVectorHeight = 0, unsigned int uArrayCount = 0, CType* pParentType = nullptr, CSemantic* pSemantic = nullptr, CRegister* pRegister = nullptr )
	: m_tName( rtTypeName )
	, m_pParentType( pParentType )
	, m_pSemantic( pSemantic )
	, m_pRegister( pRegister )
	, m_eType( eType )
	, m_uFlags( uFlags )
	, m_uVectorWidth( uVectorWidth )
	, m_uVectorHeight( uVectorHeight )
	, m_uArrayCount( uArrayCount )
	{}

	EScalarType GetScalarType( ) { return m_eType; }

	const std::string& GetTypeName() { return m_tName; }

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

	static const CType& GetVoidType()
	{
		static CType s_tVoid( "void", EScalarType_Void );
		return s_tVoid;
	}

	void SetScalarType( EScalarType eType ) { m_eType = eType; }

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

#endif //SHADR_TYPE_H