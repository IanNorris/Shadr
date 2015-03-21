#if !defined( SHADR_TYPE_H )
#define SHADR_TYPE_H

enum EScalarType
{
	EScalarType_NotScalar,
	EScalarType_NotEvaluated,

	EScalarType_Bool,

	EScalarType_Half,
	EScalarType_Float,
	EScalarType_Double,

	EScalarType_Int,
	EScalarType_UnsignedInt,

	EScalarType_MAX,
};

class CType
{
public:

	CType( EScalarType eType )
	: m_eType( eType )
	{}

	EScalarType GetScalarType( ) { return m_eType; }

	static const CType& GetNotScalar()
	{
		static CType s_tNotScalar( EScalarType_NotScalar );
		return s_tNotScalar;
	}

	static const CType& GetNotEvaluated()
	{
		static CType s_tNotEvaluated( EScalarType_NotEvaluated );
		return s_tNotEvaluated;
	}

	void SetScalarType( EScalarType eType ) { m_eType = eType; }

private:

	EScalarType m_eType;
};

#endif //SHADR_TYPE_H