#pragma once


class CSys
{
public:
	CSys( void );
	~CSys( void );

	void Init( void );
	void Shutdown( void );
	void Error( LPCTSTR lpszError );
	LPCTSTR GetCurrentDirectory( void );

private:
	TCHAR m_szCurrentDirectory[MAX_PATH];
};

extern CSys gSys;