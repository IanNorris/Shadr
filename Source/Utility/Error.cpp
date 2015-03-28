#include "Error.h"
#include <string>
#include <vector>

bool g_bErrors = false;
std::vector<std::string> g_tFilenameStack;

void PushCurrentFile( const char* pszFilename )
{
	g_tFilenameStack.push_back( pszFilename );
}

void PopCurrentFile()
{
	g_tFilenameStack.pop_back();
}

void Error_Compiler( EError eLevel, unsigned int uSourceRow, unsigned int uSourceCol, const char* pszDescription, ... )
{
	const char* pszFile = "Internal Error";
	if( !g_tFilenameStack.empty() )
	{
		pszFile = g_tFilenameStack.back().c_str();
	}

	va_list argList;
	va_start( argList, pszDescription );

	const char* pszErrorType = NULL;
	FILE* pFile = NULL;

	switch( eLevel )
	{
		case EError_Fatal:
			pszErrorType = "fatal error";
			g_bErrors = true;
			pFile = stderr;
			break;
		case EError_Error:
			pszErrorType = "error";
			g_bErrors = true;
			pFile = stderr;
			break;

		case EError_Warning:
			pszErrorType = "warning";
			pFile = stderr;
			break;

		case EError_Information:
			pszErrorType = "information";
			pFile = stdout;
			break;

		case EError_Debug:
			pszErrorType = "debug";
			pFile = stdout;
			break;

		default:
			Assert( 0, "Unexpected value %d as error level", eLevel );
	}

	fprintf( pFile, "%s:%u:%u: %s: ", pszFile, uSourceRow+1, uSourceCol+1, pszErrorType );

	vfprintf( pFile, pszDescription, argList );

	fprintf( pFile, "\n");
}

void Error_Linker( EError eLevel, const char* pszDescription, ... )
{
	const char* pszFile = "Internal Error";
	if( !g_tFilenameStack.empty() )
	{
		pszFile = g_tFilenameStack.back().c_str();
	}

	va_list argList;
	va_start( argList, pszDescription );

	const char* pszErrorType = NULL;
	FILE* pFile = NULL;

	switch( eLevel )
	{
		case EError_Fatal:
			pszErrorType = "fatal error";
			g_bErrors = true;
			pFile = stderr;
			break;
		case EError_Error:
			pszErrorType = "error";
			g_bErrors = true;
			pFile = stderr;
			break;

		case EError_Warning:
			pszErrorType = "warning";
			pFile = stderr;
			break;

		case EError_Information:
			pszErrorType = "information";
			pFile = stdout;
			break;

		case EError_Debug:
			pszErrorType = "debug";
			pFile = stdout;
			break;

		default:
			Assert( 0, "Unexpected value %d as error level", eLevel );
	}

	fprintf( pFile, "%s: %s: ", pszFile, pszErrorType );

	vfprintf( pFile, pszDescription, argList );

	fprintf( pFile, "\n");

	if( eLevel == EError_Fatal )
	{
		fprintf( pFile, "Unable to continue from previous error.\n" );
		abort();
	}
}

void Error_Fatal( const char* pszDescription, ... )
{
	g_bErrors = true;

	const char* pszFile = "Internal Error";
	if( !g_tFilenameStack.empty() )
	{
		pszFile = g_tFilenameStack.back().c_str();
	}

	va_list argList;
	va_start( argList, pszDescription );

	fprintf( stderr, "%s: fatal error: ", pszFile );

	vfprintf( stderr, pszDescription, argList );

	fprintf( stderr, "\nUnable to continue from previous error.\n" );
	abort();
}

void Error_Assert( const char* pszExpression, const char* pszSourceFile, int iLine, const char* pszDescription, ... )
{
	g_bErrors = true;

	const char* pszFile = "Internal Error";
	if( !g_tFilenameStack.empty() )
	{
		pszFile = g_tFilenameStack.back().c_str();
	}

	va_list argList;
	va_start( argList, pszDescription );

	fprintf( stderr, "Assertion Failed.\nSource: %s(%d)\nDescription: ", pszSourceFile, iLine );

	vfprintf( stderr, pszDescription, argList );

	fprintf( stderr, "\nExpression: %s\nUnable to continue from previous error.\n", pszExpression );
	abort();
}

bool DidCompilationSucceed( void )
{
	return g_bErrors;
}
