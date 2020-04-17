#include "pch.h"
#include "sys.h"

CSys gSys;

CSys::CSys( void )
{

}

CSys::~CSys( void )
{

}

void CSys::Init( void )
{
	::GetCurrentDirectory( MAX_PATH, m_szCurrentDirectory );
}

void CSys::Shutdown( void )
{

}

void CSys::Error( LPCTSTR lpszError )
{
	MessageBox( NULL, lpszError, APPLICATION_TITLE, MB_ICONERROR );
	TerminateProcess( GetCurrentProcess( ), 0 );
}

LPCTSTR CSys::GetCurrentDirectory( void )
{
	return m_szCurrentDirectory;
}