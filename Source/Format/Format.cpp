#include <stdio.h>
#include <stdlib.h>

#include "Tokens.h"
#include "Utility/Error.h"
#include "Utility/Utility.h"

#include <functional>
#include "dirent.h"

#include <TinyXML.h>

#include "Format.h"
#include "AST/AST.h"
#include "ASTFormatters_Statement.h"

std::unordered_map< std::string, CFormatter* > g_apFormats;

void ProcessNode( CFormatterStore* pStore, TiXmlElement* pElement )
{
	TiXmlAttribute* pAttrib = pElement->FirstAttribute();

	while( pAttrib )
	{
		pStore->GetDictionary()[ pAttrib->Name() ] = pAttrib->Value();

		pAttrib = pAttrib->Next();
	}
}

CASTFormatter* ProcessASTFormatter( TiXmlElement* pElement, const char* pszName )
{
	CASTFormatter* pFormatter = new CASTFormatter( pszName );

	ProcessNode( pFormatter, pElement );

	TiXmlElement* pCommandElement = pElement->FirstChildElement();
	while( pCommandElement )
	{
		const char* pszCommandName = pCommandElement->Value();

		CASTFormatterCommand* pCommand = nullptr;

		if( _stricmp( pszCommandName, "Newline" ) == 0 )
		{
			pCommand = new CASTFormatterCommandNewline();
		}
		else if( _stricmp( pszCommandName, "Print" ) == 0 )
		{
			pCommand = new CASTFormatterCommandPrint();
		}
		else if( _stricmp( pszCommandName, "AddIndent" ) == 0 )
		{
			pCommand = new CASTFormatterCommandAddIndent();
		}
		else if( _stricmp( pszCommandName, "RemoveIndent" ) == 0 )
		{
			pCommand = new CASTFormatterCommandRemoveIndent();
		}
		else if( _stricmp( pszCommandName, "Iterate" ) == 0 )
		{
			pCommand = new CASTFormatterCommandIterate();
		}
		else if( _stricmp( pszCommandName, "PrintChild" ) == 0 )
		{
			pCommand = new CASTFormatterCommandPrintChild();
		}
		else if( _stricmp( pszCommandName, "SemiColon" ) == 0 )
		{
			pCommand = new CASTFormatterCommandSemiColon();
		}
		else if( _stricmp( pszCommandName, "InsertOriginalNewlines" ) == 0 )
		{
			pCommand = new CASTFormatterCommandInsertOriginalNewlines();
		}
		else if( _stricmp( pszCommandName, "Condition" ) == 0 )
		{
			pCommand = new CASTFormatterCommandCondition();
		}
		else 
		{
			Assert( 0, "Unrecognised command type %s", pszCommandName );
		}
		pCommand->Initialise( pCommandElement );

		ProcessNode( pCommand, pCommandElement );

		pFormatter->AddCommand( pCommand );

		pCommandElement = pCommandElement->NextSiblingElement();
	}

	return pFormatter;
}

CFormatter* InitialiseFormat( const std::string& rtFormatFilename )
{
	CFormatter* pFormat = new CFormatter();

	TiXmlDocument tDoc;
	if( !tDoc.LoadFile( rtFormatFilename.c_str() ) )
	{
		fprintf( stderr, "Unable to parse XML document %s.\n", rtFormatFilename.c_str() );
		return nullptr;
	}

	TiXmlElement* pRoot = tDoc.FirstChildElement( "ShaderSyntax" );
	if( !pRoot )
	{
		fprintf( stderr, "Format file %s has no root node 'ShaderSyntax'.\n", rtFormatFilename.c_str() );
		return nullptr;
	}

	ProcessNode( pFormat, pRoot );

	TiXmlElement* pASTFormatter = pRoot->FirstChildElement();
	while( pASTFormatter )
	{
		pFormat->AddASTType( pASTFormatter->Value(), ProcessASTFormatter( pASTFormatter, pASTFormatter->Value() ) );

		pASTFormatter = pASTFormatter->NextSiblingElement();
	}

	pFormat->Initialise();

	return pFormat;
}

void InitialiseFormats()
{
	std::string tPath = "Formats";

	DIR* pDir = opendir( tPath.c_str() );
	dirent* pEnt;
	if( pDir )
	{
		while( (pEnt = readdir( pDir )) != NULL )
		{
			//We only want files that don't start with a dot
			if(  pEnt->d_name[0] != '.' )
			{
				std::string tFileContent;

				std::string tFilename = tPath;
				tFilename += "/";
				tFilename += pEnt->d_name;

				if( !ReadFile( tFileContent, tFilename.c_str() ))
				{
					fprintf( stderr, "\tUnable to open file %s\n", tFilename.c_str() );
					continue;
				}

				std::string tFilenameOnly = pEnt->d_name;

				std::string tNameWithoutExt = tFilenameOnly.substr( 0, tFilenameOnly.find_last_of( '.' ) );

				CFormatter* pFormat = InitialiseFormat( tFilename );

				if( pFormat )
				{
					g_apFormats[ tNameWithoutExt ] = pFormat;
				}
			}
		}

		closedir( pDir );
	}
}

void ExecuteFormatter( CFormatterContext* pContext, const CReflectionObject* pASTNode, const char* pszOverrideFormatter )
{
	Assert( pASTNode, "AST node is null" );

	const char* pszName = pASTNode->GetElementName();

	CASTFormatter* pFormatter = pszOverrideFormatter ? pContext->pFormatter->GetASTType( pszOverrideFormatter ) : pContext->pFormatter->GetASTType( pszName );
	if( pFormatter )
	{
		pFormatter->Action( pContext, pASTNode );
	}
	else
	{
		fprintf( stderr, "Unable to find formatter for type %s.\n", pszName );
	}
}

CFormatter* GetFormatter( const std::string& rtFormatName )
{
	auto tIter = g_apFormats.find( rtFormatName );
	if( tIter != g_apFormats.end() )
	{
		return (*tIter).second;
	}
	else
	{
		Assert( 0, "No formatter found for format %s.", rtFormatName.c_str() );
		return nullptr;
	}
}

bool IsReflectedConditionTrue( const std::string& rtReflectionPath, CFormatterContext* pContext, const CReflectionObject* pReflectionObject, bool bRoot )
{
	std::string::size_type uFirstDot = rtReflectionPath.find_first_of( '.' );
	std::string tToFirstObject = rtReflectionPath.substr( 0, uFirstDot );
	std::string tRestOfPath = uFirstDot != std::string::npos ? rtReflectionPath.substr( uFirstDot + 1 ) : std::string();

	CReflectionObject* pTestNode = nullptr;
	std::string tTestString;
	if( bRoot && pContext->atNodeVariables.GetValue( tToFirstObject, pTestNode ) )
	{
		return IsReflectedConditionTrue( tRestOfPath, pContext, pTestNode, false );
	}
	if( bRoot && pContext->atStringVariables.GetValue( tToFirstObject, tTestString ) )
	{
		return tTestString.compare( "true" ) == 0;
	}

	if( tRestOfPath.empty() )
	{
		return pReflectionObject->GetConditionOutcome( tToFirstObject );
	}
	else
	{
		const CASTReflectionType* pReflectionType = pReflectionType = pReflectionObject->GetReflectionType( tToFirstObject );

		if( !pReflectionType )
		{
			Error_Linker( EError_Error, "Unable to find value %s within object %s.\n", tToFirstObject.c_str(), pReflectionObject->GetElementName() );

			return nullptr;
		}

		switch( pReflectionType->GetType() )
		{
			case EASTReflectionType_Type:
			case EASTReflectionType_ASTNode:
				return IsReflectedConditionTrue( tRestOfPath, pContext, *pReflectionType->GetData<CReflectionObject*>(), false );

			case EASTReflectionType_Variable:
			case EASTReflectionType_TypeChild:
			case EASTReflectionType_TypeScalar:
			case EASTReflectionType_TypeSemantic:
			case EASTReflectionType_TypeRegister:		

			default:
				Error_Linker( EError_Error, "Type does not support introspection.\n" );
				return nullptr;
		}
	}

	

	
}

const CASTReflectionType* ReflectedValueToReflectionType( const std::string& rtReflectionPath, CFormatterContext* pContext, const CReflectionObject* pReflectionObject, bool bRoot )
{
	std::string::size_type uFirstDot = rtReflectionPath.find_first_of( '.' );
	std::string tToFirstObject = rtReflectionPath.substr( 0, uFirstDot );
	std::string tRestOfPath = uFirstDot != std::string::npos ? rtReflectionPath.substr( uFirstDot + 1 ) : std::string();

	const CASTReflectionType* pReflectionType = pReflectionType = pReflectionObject->GetReflectionType( tToFirstObject );

	if( !pReflectionType )
	{
		Error_Linker( EError_Error, "Unable to find value %s within object %s.\n", tToFirstObject.c_str(), pReflectionObject->GetElementName() );

		return nullptr;
	}

	switch( pReflectionType->GetType() )
	{
		case EASTReflectionType_Type:
		case EASTReflectionType_ASTNode:
			if( tRestOfPath.empty() )
			{
				return pReflectionType;
			}
			else
			{
				return ReflectedValueToReflectionType( tRestOfPath, pContext, *pReflectionType->GetData<CReflectionObject*>(), false );
			}

		case EASTReflectionType_Variable:
		case EASTReflectionType_TypeChild:
		case EASTReflectionType_TypeScalar:
		case EASTReflectionType_TypeSemantic:
		case EASTReflectionType_TypeRegister:		

		default:
			return pReflectionType;
	}
}

const CReflectionObject* ReflectedValueToReflectionObject( const std::string& rtReflectionPath, CFormatterContext* pContext, const CReflectionObject* pReflectionObject, bool bRoot )
{
	std::string::size_type uFirstDot = rtReflectionPath.find_first_of( '.' );
	std::string tToFirstObject = rtReflectionPath.substr( 0, uFirstDot );
	std::string tRestOfPath = uFirstDot != std::string::npos ? rtReflectionPath.substr( uFirstDot + 1 ) : std::string();

	const CASTReflectionType* pReflectionType = nullptr;

	CReflectionObject* pTestNode = nullptr;
	std::string tTestString;
	if( bRoot && pContext->atNodeVariables.GetValue( tToFirstObject, pTestNode ) )
	{
		if( tRestOfPath.empty() )
		{
			return pTestNode;
		}
		else
		{
			return ReflectedValueToReflectionObject( tRestOfPath, pContext, pTestNode, false );
		}
	}
	else
	{
		pReflectionType = pReflectionObject->GetReflectionType( tToFirstObject );
	}

	if( !pReflectionType )
	{
		Error_Linker( EError_Error, "Unable to find value %s within object %s.\n", tToFirstObject.c_str(), pReflectionObject->GetElementName() );

		return nullptr;
	}

	switch( pReflectionType->GetType() )
	{
		case EASTReflectionType_Type:
		case EASTReflectionType_ASTNode:
		case EASTReflectionType_Variable:
			if( tRestOfPath.empty() )
			{
				return *pReflectionType->GetData<CReflectionObject*>();
			}
			else
			{
				return ReflectedValueToReflectionObject( tRestOfPath, pContext, *pReflectionType->GetData<CReflectionObject*>(), false );
			}

		
		case EASTReflectionType_TypeChild:
		case EASTReflectionType_TypeScalar:
		case EASTReflectionType_TypeSemantic:
		case EASTReflectionType_TypeRegister:		

		default:
			Assert( 0, "Type is invalid, expected reflection object." );
			return nullptr;
	}
}

std::string ReflectedValueToString( const std::string& rtReflectionPath, CFormatterContext* pContext, const CReflectionObject* pReflectionObject, bool bRoot )
{
	std::string::size_type uFirstDot = rtReflectionPath.find_first_of( '.' );
	std::string tToFirstObject = rtReflectionPath.substr( 0, uFirstDot );
	std::string tRestOfPath = uFirstDot != std::string::npos ? rtReflectionPath.substr( uFirstDot + 1 ) : std::string();

	const CASTReflectionType* pReflectionType = nullptr;

	CReflectionObject* pTestNode = nullptr;
	std::string tTestString;
	if( bRoot && pContext->atNodeVariables.GetValue( tToFirstObject, pTestNode ) )
	{
		return ReflectedValueToString( tRestOfPath, pContext, pTestNode, false );
	}
	else if( bRoot && pContext->atStringVariables.GetValue( tToFirstObject, tTestString ) )
	{
		return tTestString;
	}
	else
	{
		pReflectionType = pReflectionObject->GetReflectionType( tToFirstObject );
	}

	if( !pReflectionType )
	{
		Error_Linker( EError_Error, "Unable to find value %s within object %s.\n", tToFirstObject.c_str(), pReflectionObject->GetElementName() );

		return "";
	}

	switch( pReflectionType->GetType() )
	{
		case EASTReflectionType_Token:
			return GetTokenString( *pReflectionType->GetData<EShaderToken>() );

		case EASTReflectionType_Type:
		case EASTReflectionType_ASTNode:
		case EASTReflectionType_Variable:
			Assert( !tRestOfPath.empty(), "Cannot print a node object" );
			return ReflectedValueToString( tRestOfPath, pContext, *pReflectionType->GetData<CReflectionObject*>(), false );

		case EASTReflectionType_TypeChild:
		case EASTReflectionType_TypeScalar:
		case EASTReflectionType_TypeSemantic:
		case EASTReflectionType_TypeRegister:
		
		
		case EASTReflectionType_CString:
			return std::string( pReflectionType->GetData<const char>() );

		case EASTReflectionType_SString:
			return *(pReflectionType->GetData<std::string>());

		case EASTReflectionType_ASTNodeArray:
			Assert( 0, "Arrays cannot be printed, you should iterate" );
			return "";

		case EASTReflectionType_Double:
			{
				char szBuffer[ 128 ] = {0};
				sprintf_s( szBuffer, 128, "%lf", *(pReflectionType->GetData<double>()) );
				
				//Trim trailing zeroes
				size_t uDotPos = (size_t)-1;
				bool encounteredExponent = false;
				size_t uLen = strlen(szBuffer);
				for( size_t uPos = 0; uPos < uLen; uPos++ )
				{
					if( szBuffer[uPos] == '.' )
					{
						uDotPos = uPos;
					}
					else if( szBuffer[uPos] == 'e' )
					{
						encounteredExponent = true;
					}
				}
				if( !encounteredExponent )
				{
					size_t uMinLength = uDotPos != (size_t)-1 ? uDotPos : 2;
					while( uLen > uMinLength && szBuffer[uLen-2] != '.' && szBuffer[uLen-1] == '0' )
					{
						szBuffer[uLen-1] = '\0';
						uLen--;
					}
				}

				return szBuffer;
			}

		case EASTReflectionType_UInt:
			{
				char szBuffer[ 128 ] = {0};
				sprintf_s( szBuffer, 128, "%llu", *(pReflectionType->GetData<uint64_t>()) );
				return szBuffer;
			}

		case EASTReflectionType_Int:
			{
				char szBuffer[ 128 ] = {0};
				sprintf_s( szBuffer, 128, "%lld", *(pReflectionType->GetData<int64_t>()) );
				return szBuffer;
			}

		case EASTReflectionType_Bool:
			return *(pReflectionType->GetData<bool>()) ? "true" : "false";
		

		default:
			Assert( 0, "Unknown type." );
			return "";
	}
}

bool ReflectedCondition( const std::string& rtReflectionPath, CFormatterContext* pContext, const CReflectionObject* pReflectionObject, bool bRoot )
{
	return false;
}

std::string CASTFormatterCommand::GetReflectedStringFromKey( const std::string& rtKey, CFormatterContext* pContext, const CReflectionObject* pASTNode ) const
{
	std::string tTarget;
	if( !GetValue( rtKey, tTarget ) )
	{
		Error_Linker( EError_Error, "Failed to get attribute %s for formatter %s while formatting AST node %s.\n", rtKey.c_str(), GetName(), pASTNode->GetElementName() );
		return "";
	}

	return ReflectedValueToString( tTarget, pContext, pASTNode );
}

const CReflectionObject* CASTFormatterCommand::GetReflectedObjectFromKey( const std::string& rtKey, CFormatterContext* pContext, const CReflectionObject* pASTNode ) const
{
	std::string tTarget;
	if( !GetValue( rtKey, tTarget ) )
	{
		Error_Linker( EError_Error, "Failed to get attribute %s for formatter %s while formatting AST node %s.\n", rtKey.c_str(), GetName(), pASTNode->GetElementName() );
		return nullptr;
	}

	return ReflectedValueToReflectionObject( tTarget, pContext, pASTNode );
}

void CFormatterContext::PrepareNewline()
{
	for( unsigned int uIndent = 0; uIndent < uNeededIndent; uIndent++ )
	{
		tCurrentElement += pFormatter->GetIndent();
	}
	uNeededIndent = 0;
}