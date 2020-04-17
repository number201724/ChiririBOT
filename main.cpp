#include "pch.h"
#include "common.h"
#include "sys.h"
#include "net.h"
#include "http.h"


#include <io.h>
#include <fcntl.h>
#include <iostream>

int wmain( int argc, char *argv[] )
{
	SetConsoleOutputCP( CP_UTF8 );
	SetConsoleCP( CP_UTF8 );

	CONSOLE_FONT_INFOEX fontInfo;
	fontInfo.cbSize = sizeof( fontInfo );
	fontInfo.FontFamily = 54;
	fontInfo.FontWeight = 400;
	fontInfo.nFont = 0;
	fontInfo.dwFontSize = {18, 18};
	_tcscpy( fontInfo.FaceName, L"KaiTi" );

	char *a = setlocale( LC_ALL, "chinese" );

	SetCurrentConsoleFontEx( GetStdHandle( STD_OUTPUT_HANDLE ), false, &fontInfo );
	_setmode( _fileno( stdout ), _O_U16TEXT );

#ifdef ONLY_GUARD
	SetConsoleTitle( TEXT( "BILIBILI GUARD DANMAKU AUTO TRANSLATOR" ) );
#else
#ifdef ONLY_NANA
	SetConsoleTitle( TEXT( "BILIBILI NANA DANMAKU AUTO TRANSLATOR" ) );
#else
	SetConsoleTitle( TEXT( "BILIBILI DANMAKU AUTO TRANSLATOR" ) );
#endif
#endif
	
	wprintf( L"[BILIBILI DANMAKU AUTO TRANSLATOR]\n" );
	wprintf( L"Author:number201724\n" );
	wprintf( L"Release Date: %s %s\n", TEXT( __DATE__ ), TEXT( __TIME__ ) );

	gSys.Init( );
	gNet.Init( );
	gHttp.Init( );
	gNet.Init( );
	gHttp.Init( );

	gDanmakuNet.Init( 21304638, 11588230 );

	while( true )
	{
		gDanmakuNet.Frame( );
		gHttp.Frame( );

		Sleep( 30 );
	}

	return 0;
}