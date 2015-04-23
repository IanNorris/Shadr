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

CASTFormatter* ProcessASTFormatter( TiXmlElement* pElement )
{
	CASTFormatter* pFormatter = new CASTFormatter();

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
		

		pFormat->AddASTType( pASTFormatter->Value(), ProcessASTFormatter( pASTFormatter ) );

		pASTFormatter = pASTFormatter->NextSiblingElement();
	}

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

					CFormatterContext tContext;
					pFormat->GetASTType( "Block" )->Action( &tContext, NULL );
				}
			}
		}

		closedir( pDir );
	}
}