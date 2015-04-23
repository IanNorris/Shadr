#if !defined( SHADR_FORMAT_H )
#define SHADR_FORMAT_H

typedef std::unordered_map< std::string, std::string > TDictionary;

class CFormatter;

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

class CFormatterContext
{
public:

	CFormatterContext()
	: pFormatter( nullptr )
	, uCurrentIndent( 0 )
	{}

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

	bool GetValue( const std::string& tKey, std::string& rtValueOut )
	{
		auto tIter = m_tValues.find( tKey );
		if( tIter != m_tValues.end() )
		{
			rtValueOut = m_tValues[ tKey ];
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
	virtual void Initialise( TiXmlElement* pElement ) = 0;

	virtual void Action( CFormatterContext* pContext, CASTBase* pASTNode ) = 0;
};

class CASTFormatter : public CFormatterStore
{
public:

	void AddCommand( CASTFormatterCommand* pCommand )
	{
		m_apCommands.push_back( pCommand );
	}

	void Action( CFormatterContext* pContext, CASTBase* pASTNode )
	{
		for( auto pCommand : m_apCommands )
		{
			pCommand->Action( pContext, pASTNode );
		}
	}

private:

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
		Assert( tIter != m_tASTTypes.end(), "No formatter specified for AST type %s", rtName.c_str() );

		return (*tIter).second;
	}

	const std::string& GetIndent()
	{
		return m_tIndent;
	}

private:

	TASTMap m_tASTTypes;

	std::string m_tIndent;
};

void ProcessNode( CFormatterStore* pStore, TiXmlElement* pElement );
CASTFormatter* ProcessASTFormatter( TiXmlElement* pElement );

void InitialiseFormats();

#include "FormatterCommands.h"

#endif //SHADR_FORMAT_H