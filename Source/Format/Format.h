#if !defined( SHADR_FORMAT_H )
#define SHADR_FORMAT_H

typedef std::unordered_map< std::string, std::string > TDictionary;

class CFormatter;
class CReflectionObject;

class CFormatterBucket
{
public:

	std::string& GetBody() { return m_tBody; }

	CFormatterBucket* GetBucket( unsigned int uBucket )
	{
		auto tIter = atNumberedBuckets.find( uBucket );
		if( tIter == atNumberedBuckets.end() )
		{
			atNumberedBuckets[ uBucket ] = new CFormatterBucket();
		}

		return atNumberedBuckets[ uBucket ];
	}

	CFormatterBucket* GetBucket( const std::string& rtBucket )
	{
		auto tIter = atNamedBuckets.find( rtBucket );
		if( tIter == atNamedBuckets.end() )
		{
			atNamedBuckets[ rtBucket ] = new CFormatterBucket();
		}

		return atNamedBuckets[ rtBucket ];
	}

private:

	std::unordered_map< std::string, CFormatterBucket* > atNamedBuckets; 
	std::unordered_map< unsigned int, CFormatterBucket* > atNumberedBuckets; 

	std::string m_tBody;
};

template< class T >
class CValueStack
{
public:

	void CreateValue( const std::string& rtVariable )
	{
		atTempVariableStack[ rtVariable ].push_back( T() );
	}

	void DestroyValue( const std::string& rtVariable )
	{
		atTempVariableStack[ rtVariable ].pop_back();
	}

	bool GetValue( const std::string& rtVariable, T& rtValueOut )
	{
		auto tIter = atTempVariableStack.find( rtVariable );

		if( tIter == atTempVariableStack.end() )
		{
			return false;
		}
		else
		{
			rtValueOut = (*tIter).second.back();
			return true;
		}
	}

	void SetValue( const std::string& rtVariable, T& rtValue )
	{
		auto tIter = atTempVariableStack.find( rtVariable );

		if( tIter == atTempVariableStack.end() )
		{
			Error_Linker( EError_Error, "Variable %s has not been created.\n", rtVariable.c_str() );
			return;
		}
		else
		{
			(*tIter).second.back() = rtValue;
		}
	}

private:

	std::unordered_map< std::string, std::vector< const T > > atTempVariableStack; 
};

class CASTBase;

class CFormatterContext
{
public:

	CFormatterContext( CFormatter* _pFormatter )
	: pFormatter( _pFormatter )
	, uCurrentIndent( 0 )
	{}

	CValueStack<std::string> atStringVariables;
	CValueStack<CReflectionObject*> atNodeVariables;

	std::string tCurrentElement;

	CFormatter* pFormatter;
	unsigned int uCurrentIndent;
	CFormatterBucket tBuckets;
};

class CFormatterStore
{
public:

	void SetValue( const std::string& tKey, const std::string& tValue )
	{
		m_tValues[ tKey ] = tValue;
	}

	bool GetValue( const std::string& tKey, std::string& rtValueOut ) const
	{
		auto tIter = m_tValues.find( tKey );
		if( tIter != m_tValues.end() )
		{
			rtValueOut = (*tIter).second;
			return true;
		}

		return false;
	}

	TDictionary& GetDictionary() { return m_tValues; }

private:

	TDictionary m_tValues;
};

class CASTBase;
class TiXmlElement;

class CASTFormatterCommand : public CFormatterStore
{
public:

	virtual const char* GetName() const = 0;

	virtual void Initialise( TiXmlElement* pElement ) = 0;

	virtual void Action( CFormatterContext* pContext, const CReflectionObject* pASTNode ) = 0;

	std::string GetReflectedStringFromKey( const std::string& rtKey, CFormatterContext* pContext, const CReflectionObject* pASTNode ) const;
	const CReflectionObject* GetReflectedObjectFromKey( const std::string& rtKey, CFormatterContext* pContext, const CReflectionObject* pASTNode ) const;
};

class CASTFormatter : public CFormatterStore
{
public:

	CASTFormatter( const char* pszName )
	: m_tName( pszName )
	{}

	void AddCommand( CASTFormatterCommand* pCommand )
	{
		m_apCommands.push_back( pCommand );
	}

	void Action( CFormatterContext* pContext, const CReflectionObject* pASTNode )
	{
		for( auto pCommand : m_apCommands )
		{
			pCommand->Action( pContext, pASTNode );
		}
	}

private:

	std::string m_tName;

	std::vector< CASTFormatterCommand* > m_apCommands;
};

typedef std::unordered_map< std::string, CASTFormatter* > TASTMap;

class CFormatter : public CFormatterStore
{
public:
	void AddASTType( const char* pszName, CASTFormatter* pFormatter )
	{
		m_tASTTypes[ pszName ] = pFormatter;
	}

	CASTFormatter* GetASTType( const std::string& rtName )
	{
		auto tIter = m_tASTTypes.find( rtName );

		if( tIter != m_tASTTypes.end() )
		{
			return (*tIter).second;
		}
		else
		{
			Error_Linker( EError_Error, "No formatter specified for AST type %s", rtName.c_str() );
			return nullptr;
		}		
	}

	const std::string& GetIndent()
	{
		return m_tIndent;
	}

private:

	TASTMap m_tASTTypes;

	std::string m_tIndent;
};

class CASTReflectionType;

void ProcessNode( CFormatterStore* pStore, TiXmlElement* pElement );
CASTFormatter* ProcessASTFormatter( TiXmlElement* pElement, const char* pszName );
void ExecuteFormatter( CFormatterContext* pContext, const CReflectionObject* pASTNode, const char* pszOverrideFormatter );
CFormatter* GetFormatter( const std::string& rtFormatName );
bool IsReflectedConditionTrue( const std::string& rtReflectionPath, CFormatterContext* pContext, const CReflectionObject* pReflectionObject );
const CASTReflectionType* ReflectedValueToReflectionType( const std::string& rtReflectionPath, CFormatterContext* pContext, const CReflectionObject* pReflectionObject, bool bRoot = true );
const CReflectionObject* ReflectedValueToReflectionObject( const std::string& rtReflectionPath, CFormatterContext* pContext, const CReflectionObject* pReflectionObject, bool bRoot = true );
std::string ReflectedValueToString( const std::string& rtReflectionPath, CFormatterContext* pContext, const CReflectionObject* pReflectionObject, bool bRoot = true );
bool ReflectedCondition( const std::string& rtReflectionPath, CFormatterContext* pContext, const CReflectionObject* pReflectionObject, bool bRoot = true );

void InitialiseFormats();

#include "FormatterCommands.h"

#endif //SHADR_FORMAT_H