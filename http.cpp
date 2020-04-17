#include "pch.h"
#include "common.h"
#include "sys.h"
#include "http.h"

CHttp gHttp;


CHttp::CHttp( void )
{
	m_pcurlm = NULL;
	m_pending_requests = 0;
}

CHttp::~CHttp( void )
{

}

void CHttp::Init( void )
{
	curl_global_init( CURL_GLOBAL_ALL );

	m_pcurlm = curl_multi_init( );
	if( m_pcurlm == NULL )
	{
		gSys.Error( TEXT( "curl_multi_init init failed." ) );
	}

	m_pending_requests = 0;
}

void CHttp::Shutdown( void )
{
	if( m_pcurlm )
	{
		curl_multi_cleanup( m_pcurlm );
		m_pcurlm = NULL;
	}

	m_pending_requests = 0;
	curl_global_cleanup( );
}


void CHttp::Frame( void )
{
	int  msgs_left;
	CURLMsg *pMsg;

	for( ;;)
	{
		if( CURLM_CALL_MULTI_PERFORM != curl_multi_perform( m_pcurlm, &m_pending_requests ) )
			break;
	}

	while( (pMsg = curl_multi_info_read( m_pcurlm, &msgs_left )) )
	{
		CWebRequest *pRequest = NULL;
		if( CURLMSG_DONE != pMsg->msg )
			continue;

		curl_multi_remove_handle( m_pcurlm, pMsg->easy_handle );

		curl_easy_getinfo( pMsg->easy_handle, CURLINFO_PRIVATE,&pRequest );

		if( pRequest )
		{
			pRequest->OnDone( );

			for( auto pos = m_requests.begin( ); pos != m_requests.end( ); pos++ )
			{
				if( *pos == pRequest )
				{
					m_requests.erase( pos );
					break;
				}
			}

			delete pRequest;
		}
	}
}

bool CHttp::Run( CWebRequest *pRequest )
{
	if( !curl_multi_add_handle( m_pcurlm, pRequest->GetHandle( ) ) )
	{
		m_requests.push_back( pRequest );
		return true;
	}
	
	return false;
}

int CHttp::GetPendingRequestCount( void )
{
	return m_pending_requests;
}

CWebRequest::CWebRequest( void )
{
	m_pcurl = curl_easy_init( );
	curl_easy_setopt( m_pcurl, CURLOPT_FOLLOWLOCATION, 1 );
	curl_easy_setopt( m_pcurl, CURLOPT_PRIVATE, this );
	curl_easy_setopt( m_pcurl, CURLOPT_VERBOSE, 0 );
	curl_easy_setopt( m_pcurl, CURLOPT_HEADER, 0 );
	curl_easy_setopt( m_pcurl, CURLOPT_NOSIGNAL, 1 );
	curl_easy_setopt( m_pcurl, CURLOPT_WRITEDATA, this );
	curl_easy_setopt( m_pcurl, CURLOPT_WRITEFUNCTION, OnWriteStaticProc );
}

CWebRequest::~CWebRequest( void )
{
	curl_easy_cleanup( m_pcurl );
}

void CWebRequest::SetUrl( const char *szUrl )
{
	curl_easy_setopt( m_pcurl, CURLOPT_URL, szUrl );
}

void CWebRequest::SetPost( bool Post )
{
	curl_easy_setopt( m_pcurl, CURLOPT_POST, (int)Post );
}

void CWebRequest::SetTimeout( int nTimeout )
{
	curl_easy_setopt( m_pcurl, CURLOPT_TIMEOUT, nTimeout );
}

void CWebRequest::SetConnectTimeout( int nTimeout )
{
	curl_easy_setopt( m_pcurl, CURLOPT_CONNECTTIMEOUT, nTimeout );
}

long CWebRequest::GetStatusCode( void )
{
	long status_code = 0;

	curl_easy_getinfo( m_pcurl, CURLINFO_RESPONSE_CODE, &status_code );

	return status_code;
}

size_t CWebRequest::OnWriteStaticProc( void *buffer, size_t size, size_t nmemb, void *pdata )
{
	CWebRequest *pRequest = (CWebRequest *)pdata;

	return pRequest->OnWrite( buffer, size, nmemb );
}

void CWebRequest::OnCompleted( void )
{

}

void CWebRequest::OnDone( void )
{
	OnCompleted( );
}

size_t CWebRequest::OnWrite( void *buffer, size_t size, size_t nmemb )
{
	return size * nmemb;
}

CWebJsonRequest::CWebJsonRequest( void )
{
	m_json = NULL;
}

CWebJsonRequest::~CWebJsonRequest( void )
{
	if( m_json != NULL )
	{
		json_decref( m_json );
		m_json = NULL;
	}
}

size_t CWebJsonRequest::OnWrite( void *buffer, size_t size, size_t nmemb )
{
	m_data.write( (char*)buffer, (int)(size * nmemb) );
	return size * nmemb;
}

void CWebJsonRequest::OnDone( void )
{
	m_json = json_loads( m_data.str( ).c_str( ), 0, &m_error );

	OnCompleted( );
}