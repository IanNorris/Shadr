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
		DEBUG_STOP

		pContext->tCurrentElement += pContext->pFormatter->GetNewline();
		pContext->uNeededIndent = pContext->uCurrentIndent;
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
		DEBUG_STOP

		std::string tTemp;

		pContext->PrepareNewline();

		if( GetValue( "Literal", tTemp ) )
		{
			pContext->tCurrentElement += tTemp;
		}
		else if( GetValue( "Value", tTemp ) )
		{
			pContext->tCurrentElement += ReflectedValueToString(tTemp, pContext, pASTNode );
		}
		else
		{
			Error_Linker( EError_Error, GetCurrentFilename(), "Neither Literal or Value specified for Print command.\n" );
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
		DEBUG_STOP

		pContext->uCurrentIndent++;
		pContext->uNeededIndent++;

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
		DEBUG_STOP

		pContext->uCurrentIndent--;

		if( pContext->uNeededIndent )
		{
			pContext->uNeededIndent--;
		}
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
		DEBUG_STOP

		std::string tTarget;
		GET( "Target", tTarget );

		std::string tVariableName;
		GET( "Variable", tVariableName );

		Assert( tVariableName.length() >= 2, "User variables must be at least one character long.", tVariableName.c_str() );
		Assert( tVariableName[0] == '$', "User variables must start with a $, %s does not.", tVariableName.c_str() );

		const CASTReflectionType* pRefType = ReflectedValueToReflectionType( tTarget, pContext, pASTNode );

		auto pList = pRefType->GetData<std::vector< CReflectionObject* >>();

		pContext->atNodeVariables.CreateValue( tVariableName );
		pContext->atStringVariables.CreateValue( "IsFirst(" + tVariableName + ")" );
		pContext->atStringVariables.CreateValue( "IsLast(" + tVariableName + ")" );

		size_t uItemCount = (*pList).size();
		size_t uItem = 0;

		for( auto pItem : (*pList) )
		{
			pContext->atNodeVariables.SetValue( tVariableName, pItem );
			pContext->atStringVariables.SetValue( "IsFirst(" + tVariableName + ")", (uItem == 0) ? std::string("true") : std::string("false") );
			pContext->atStringVariables.SetValue( "IsLast(" + tVariableName + ")", (uItem == (uItemCount - 1)) ? std::string("true") : std::string("false") );

			m_pFormatter->Action( pContext, pItem );

			uItem++;
		}

		pContext->atNodeVariables.DestroyValue( tVariableName );
		pContext->atStringVariables.DestroyValue( "IsFirst(" + tVariableName + ")" );
		pContext->atStringVariables.DestroyValue( "IsLast(" + tVariableName + ")" );
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
		DEBUG_STOP

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
		DEBUG_STOP

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
		DEBUG_STOP

		pContext->PrepareNewline();

		pContext->tCurrentElement += pContext->pFormatter->GetNewline();
		pContext->uNeededIndent = pContext->uCurrentIndent;
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
		DEBUG_STOP

		bool bExpect = true;

		std::string tConditionalPath;
		if( GetValue( "IfTrue", tConditionalPath ) )
		{
			
		}
		else if( GetValue( "IfFalse", tConditionalPath ) )
		{
			bExpect = false;
		}
		else
		{
			Error_Linker( EError_Error, GetCurrentFilename(), "Neither IfTrue or IfFalse specified for Condition" );
			return;
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