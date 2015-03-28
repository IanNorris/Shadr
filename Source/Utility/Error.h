#if !defined( SHADR_ERROR_H )
#define SHADR_ERROR_H

#include <stdio.h>
#include <stdarg.h>

enum EError
{
	EError_Fatal,
	EError_Error,
	EError_Warning,
	EError_Information,
	EError_Debug,
};

void PushCurrentFile( const char* pszFilename );
void PopCurrentFile();

void Error_Compiler( EError eLevel, unsigned int uSourceRow, unsigned int uSourceCol, const char* pszDescription, ... );
void Error_Linker( EError eLevel, const char* pszDescription, ... );
void Error_Fatal( const char* pszDescription, ... );

void Error_Assert( const char* pszExpression, const char* pszSourceFile, int iLine, const char* pszDescription, ... );

bool DidCompilationSucceed( void );

#define Assert( condition, description, ... ) if( !(condition) ){ Error_Assert( #condition, __FILE__, __LINE__, description, __VA_ARGS__ ); }

#endif //SHADR_ERROR_H