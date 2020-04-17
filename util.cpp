#include "pch.h"
#include "util.h"



LPWSTR UTIL_UTF8ToUnicode( LPCSTR lpUTF8String )
{
	LPWSTR lpUnicodeString;
	int nLength;
	
	nLength = MultiByteToWideChar( CP_UTF8, 0, lpUTF8String, -1, NULL, 0 );

	if( nLength == -1 )
		return NULL;

	lpUnicodeString = new WCHAR[nLength + 1];
	if( lpUnicodeString == NULL )
		return NULL;

	memset( lpUnicodeString, 0, sizeof( WCHAR ) * (nLength + 1) );

	MultiByteToWideChar( CP_UTF8, 0, lpUTF8String, -1, lpUnicodeString, nLength );

	return lpUnicodeString;
}
