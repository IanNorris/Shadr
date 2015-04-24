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

	void Action( CFormatterContext* pContext, const CReflectionObject* pASTNode )
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

	void Action( CFormatterContext* pContext, const CReflectionObject* pASTNode )
	{
		std::string tTemp;

		if( GetValue( "Literal", tTemp ) )
		{
			pContext->tCurrentElement += tTemp;
		}
		
		if( GetValue( "Value", tTemp ) )
		{
			pContext->tCurrentElement += ReflectedValueToString(tTemp, pContext, pASTNode );
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

	void Action( CFormatterContext* pContext, const CReflectionObject* pASTNode )
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

	void Action( CFormatterContext* pContext, const CReflectionObject* pASTNode )
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
		m_pFormatter = ProcessASTFormatter( pElement, "Iterate" );
	}

	void Action( CFormatterContext* pContext, const CReflectionObject* pASTNode )
	{
		std::string tTarget;
		GET( "Target", tTarget );

		std::string tVariableName;
		GET( "Variable", tVariableName );

		const CASTReflectionType* pRefType = ReflectedValueToReflectionType( tTarget, pContext, pASTNode );

		auto pList = pRefType->GetData<std::vector< CReflectionObject* >>();

		pContext->atNodeVariables.CreateValue( tVariableName );

		for( auto pItem : (*pList) )
		{
			pContext->atNodeVariables.SetValue( tVariableName, pItem );

			m_pFormatter->Action( pContext, pItem );
		}

		pContext->atNodeVariables.DestroyValue( tVariableName );
	}

private:

	CASTFormatter* m_pFormatter;
};

class CASTFormatterCommandPrintChild : public CASTFormatterCommand
{
public:

	virtual const char* GetName() const { return "PrintChild"; }

	void Initialise( TiXmlElement* pElement )
	{
	}

	void Action( CFormatterContext* pContext, const CReflectionObject* pASTNode )
	{
		const char* pszUseOverride = nullptr;
		std::string tUseOverride;
		if( GetValue( "Use", tUseOverride ) )
		{
			pszUseOverride = tUseOverride.c_str();
		}
		

		const CReflectionObject* pObject = GetReflectedObjectFromKey( "Target", pContext, pASTNode );
		Assert( pObject, "Unable to find object for Target" );

		ExecuteFormatter( pContext, pObject, pszUseOverride );
	}
};

class CASTFormatterCommandSemiColon : public CASTFormatterCommand
{
public:

	virtual const char* GetName() const { return "SemiColon"; }

	void Initialise( TiXmlElement* pElement )
	{
	}

	void Action( CFormatterContext* pContext, const CReflectionObject* pASTNode )
	{
		pContext->tCurrentElement += ";";
	}
};

class CASTFormatterCommandInsertOriginalNewlines : public CASTFormatterCommand
{
public:

	virtual const char* GetName() const { return "InsertOriginalNewlines"; }

	void Initialise( TiXmlElement* pElement )
	{
	}

	void Action( CFormatterContext* pContext, const CReflectionObject* pASTNode )
	{
		pContext->tCurrentElement += "\n";
	}
};

class CASTFormatterCommandCondition : public CASTFormatterCommand
{
public:

	virtual const char* GetName() const { return "Condition"; }

	CASTFormatterCommandCondition()
	: CASTFormatterCommand()
	, m_pFormatter( nullptr )
	{}

	void Initialise( TiXmlElement* pElement )
	{
		m_pFormatter = ProcessASTFormatter( pElement, "Iterate" );
	}

	void Action( CFormatterContext* pContext, const CReflectionObject* pASTNode )
	{
		bool bExpect = true;

		std::string tConditionalPath;
		if( GetValue( "IfTrue", tConditionalPath ) )
		{
			
		}
		else if( GetValue( "IfFalse", tConditionalPath ) )
		{
			bExpect = false;
		}

		if( IsReflectedConditionTrue( tConditionalPath, pContext, pASTNode ) == bExpect )
		{
			m_pFormatter->Action( pContext, pASTNode );
		}
	}

private:

	CASTFormatter* m_pFormatter;
};

#endif //SHADR_FORMATTER_COMMANDS_H