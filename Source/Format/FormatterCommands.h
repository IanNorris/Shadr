#if !defined( SHADR_FORMATTER_COMMANDS_H )
#define SHADR_FORMATTER_COMMANDS_H

class CASTFormatterCommandNewline : public CASTFormatterCommand
{
public:
	void Initialise( TiXmlElement* pElement )
	{
	
	}

	void Action( CFormatterContext* pContext, CASTBase* pASTNode )
	{
		pContext->tCurrentElement.push_back( '\n' );

		for( unsigned int uIndent = 0; uIndent < pContext->uCurrentIndent; uIndent++ )
		{
			pContext->tCurrentElement += pContext->pFormatter->GetIndent();
		}
	}
};

class CASTFormatterCommandPrint : public CASTFormatterCommand
{
public:
	void Initialise( TiXmlElement* pElement )
	{
	
	}

	void Action( CFormatterContext* pContext, CASTBase* pASTNode )
	{
		std::string tTemp;

		if( GetValue( "Literal", tTemp ) )
		{
			pContext->tCurrentElement += tTemp;
		}
		
		if( GetValue( "Value", tTemp ) )
		{
			Assert( 0, "Not implemented" );
			pContext->tCurrentElement += tTemp;
		}

		for( unsigned int uIndent = 0; uIndent < pContext->uCurrentIndent; uIndent++ )
		{
			pContext->tCurrentElement += pContext->pFormatter->GetIndent();
		}
	}
};

class CASTFormatterCommandAddIndent : public CASTFormatterCommand
{
public:
	void Initialise( TiXmlElement* pElement )
	{
	
	}

	void Action( CFormatterContext* pContext, CASTBase* pASTNode )
	{
		pContext->uCurrentIndent++;
	}
};

class CASTFormatterCommandRemoveIndent : public CASTFormatterCommand
{
public:
	void Initialise( TiXmlElement* pElement )
	{
	
	}

	void Action( CFormatterContext* pContext, CASTBase* pASTNode )
	{
		pContext->uCurrentIndent--;
	}
};

class CASTFormatterCommandIterate : public CASTFormatterCommand, public CASTFormatter
{
public:

	CASTFormatterCommandIterate()
	: CASTFormatterCommand()
	, m_pFormatter( nullptr )
	{}

	void Initialise( TiXmlElement* pElement )
	{
		m_pFormatter = ProcessASTFormatter( pElement );
	}

	void Action( CFormatterContext* pContext, CASTBase* pASTNode )
	{
		m_pFormatter->Action( pContext, pASTNode );
	}

private:

	CASTFormatter* m_pFormatter;
};

class CASTFormatterCommandPrintChild : public CASTFormatterCommand, public CASTFormatter
{
public:

	void Initialise( TiXmlElement* pElement )
	{
	}

	void Action( CFormatterContext* pContext, CASTBase* pASTNode )
	{
		
	}
};

class CASTFormatterCommandSemiColon : public CASTFormatterCommand, public CASTFormatter
{
public:

	void Initialise( TiXmlElement* pElement )
	{
	}

	void Action( CFormatterContext* pContext, CASTBase* pASTNode )
	{
		pContext->tCurrentElement += ";";
	}
};

class CASTFormatterCommandInsertOriginalNewlines : public CASTFormatterCommand, public CASTFormatter
{
public:

	void Initialise( TiXmlElement* pElement )
	{
	}

	void Action( CFormatterContext* pContext, CASTBase* pASTNode )
	{
		pContext->tCurrentElement += "\n";
	}
};

#endif //SHADR_FORMATTER_COMMANDS_H