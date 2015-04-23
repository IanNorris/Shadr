#if !defined( SHADR_FORMATTER_COMMANDS_H )
#define SHADR_FORMATTER_COMMANDS_H

#include "AST/AST.h"

#define GET( name, target )																											\
	if( !GetValue( name, target ) )																									\
	{																																\
		Error_Linker( EError_Error, "Failed to get attribute %s for formatter %s while formatting AST node %s.\n", (const char*)name, GetName(), pASTNode->GetElementName() );	\
		return;																														\
	}

class CASTFormatterCommandNewline : public CASTFormatterCommand
{
public:

	virtual const char* GetName() const { return "NewLine"; }

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

	virtual const char* GetName() const { return "Print"; }

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

	virtual const char* GetName() const { return "AddIndent"; }

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

	virtual const char* GetName() const { return "RemoveIndent"; }

	void Initialise( TiXmlElement* pElement )
	{
	
	}

	void Action( CFormatterContext* pContext, CASTBase* pASTNode )
	{
		pContext->uCurrentIndent--;
	}
};

class CASTFormatterCommandIterate : public CASTFormatterCommand
{
public:

	virtual const char* GetName() const { return "Iterate"; }

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
		std::string tTarget;
		GET( "Target", tTarget );

		auto pList = pASTNode->GetReflectedData<std::vector< CASTBase* >>( tTarget );

		for( auto pItem : (*pList) )
		{
			m_pFormatter->Action( pContext, pItem );
		}	
	}

private:

	CASTFormatter* m_pFormatter;

	std::vector< CASTFormatterCommand* > m_apCommands;
};

class CASTFormatterCommandPrintChild : public CASTFormatterCommand, public CASTFormatter
{
public:

	virtual const char* GetName() const { return "PrintChild"; }

	void Initialise( TiXmlElement* pElement )
	{
	}

	void Action( CFormatterContext* pContext, CASTBase* pASTNode )
	{
		ExecuteFormatter( pContext, pASTNode );
	}
};

class CASTFormatterCommandSemiColon : public CASTFormatterCommand, public CASTFormatter
{
public:

	virtual const char* GetName() const { return "SemiColon"; }

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

	virtual const char* GetName() const { return "InsertOriginalNewlines"; }

	void Initialise( TiXmlElement* pElement )
	{
	}

	void Action( CFormatterContext* pContext, CASTBase* pASTNode )
	{
		pContext->tCurrentElement += "\n";
	}
};

#endif //SHADR_FORMATTER_COMMANDS_H