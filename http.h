#pragma once

class CWebRequest
{
public:
	CWebRequest( void );
	virtual ~CWebRequest( void );

	void SetUrl( const char *szUrl );
	void SetPost( bool Post );
	void SetTimeout( int nTimeout = 15 );
	void SetConnectTimeout( int nTimeout = 15 );
	long GetStatusCode( void );
	virtual void OnCompleted( void );
	virtual void OnDone( void );
protected:
	virtual size_t OnWrite( void *buffer, size_t size, size_t nmemb );
private:
	static size_t OnWriteStaticProc( void *buffer, size_t size, size_t nmemb, void *pdata );

public:
	inline CURL *GetHandle( void ) {
		return m_pcurl;
	}
protected:
	CURL *m_pcurl;
};

class CWebJsonRequest : public CWebRequest
{
public:
	CWebJsonRequest( void );
	virtual ~CWebJsonRequest( void );

protected:
	virtual size_t OnWrite( void *buffer, size_t size, size_t nmemb );
	virtual void OnDone( void );
protected:
	std::stringstream m_data;
	json_t *m_json;
	json_error_t m_error;
};


class CHttp
{
public:
	CHttp( void );
	~CHttp( void );

	void Init( void );
	void Shutdown( void );
	void Frame( void );

	int GetPendingRequestCount( void );
	bool Run( CWebRequest *pRequest );
private:
	CURLM *m_pcurlm;
	INT m_pending_requests;
	std::list<CWebRequest *> m_requests;
};


extern CHttp gHttp;