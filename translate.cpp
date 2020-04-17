#include "pch.h"
#include "common.h"
#include "sys.h"
#include "http.h"
#include "translate.h"
#include "util.h"


bool IsShiftJISCharater( wchar_t *chars )
{
	bool status;
	size_t len = wcslen( chars );
	wchar_t c;

	status = true;

	for( size_t i = 0; i < len; i++ )
	{
		c = chars[i];

		if( c >= 0x3040 && c <= 0x309F )
			continue;

		//CJK 部首补充 (CJK Radicals Supplement) 
		if( c >= 0x2E80 && c <= 0x2EFF )
			continue;

		//CJK 符号和标点 (CJK Symbols and Punctuation) 
		if( c >= 0x3000 && c <= 0x303F )
			continue;

		//日文平假名 (Hiragana) 
		//日文片假名 (Katakana)
		if( c >= 0x3040 && c <= 0x30FF )
			continue;

		//CJK 笔画 (CJK Strokes) 
		if( c >= 0x31C0 && c <= 0x31EF )
			continue;

		//日文片假名语音扩展 (Katakana Phonetic Extensions) 
		if( c >= 0x31F0 && c <= 0x31FF )
			continue;

		//封闭式 CJK 文字和月份 (Enclosed CJK Letters and Months) 
		if( c >= 0x3200 && c <= 0x32FF )
			continue;

		//CJK 兼容 (CJK Compatibility) 
		if( c >= 0x3300 && c <= 0x33FF )
			continue;

		//CJK 统一表意符号扩展 A (CJK Unified Ideographs Extension A) 
		if( c >= 0x3400 && c <= 0x4DBF )
			continue;

		//CJK 统一表意符号 (CJK Unified Ideographs) 
		if( c >= 0x4E00 && c <= 0x9FBF )
			continue;

		//CJK 兼容象形文字 (CJK Compatibility Ideographs) 
		if( c >= 0xF900 && c <= 0xFAFF )
			continue;

		//CJK 兼容形式 (CJK Compatibility Forms) 
		if( c >= 0xFE30 && c <= 0xFE4F )
			continue;

		status = false;
		break;
	}

	return status;
}

CTranslateRequest::CTranslateRequest( const char *from, const char *to )
{
	char url[256];
	m_postdata = NULL;
	m_headers = NULL;
	
	m_headers = curl_slist_append( m_headers, "Ocp-Apim-Subscription-Key:********************************" );
	m_headers = curl_slist_append( m_headers, "Content-Type:application/json" );
	m_headers = curl_slist_append( m_headers, "charset:utf-8" );
	curl_easy_setopt( m_pcurl, CURLOPT_HTTPHEADER, m_headers );

	sprintf( url, "https://api-apc.cognitive.microsofttranslator.com/translate?api-version=3.0&from=%s&to=%s", from, to );
	SetUrl( url );
	SetPost( true );

}

CTranslateRequest::~CTranslateRequest( void )
{
	if( m_postdata )
		free( m_postdata );
}

void CTranslateRequest::setText( const char *text )
{
	if( m_postdata )
	{
		free( m_postdata );
		m_postdata = NULL;
	}

	m_original_text = text;

	json_t *json = json_array( );
	json_array_append( json, json_pack( "{s:s}", "Text", text ) );
	m_postdata = json_dumps( json, 0 );
	curl_easy_setopt( m_pcurl, CURLOPT_POSTFIELDS, m_postdata );
	curl_easy_setopt( m_pcurl, CURLOPT_POSTFIELDSIZE, strlen( m_postdata ) );
	json_decref( json );
}

CDanmakuTranslateRequest::CDanmakuTranslateRequest( const char *from /* = "ja" */, const char *to /* = "zh-Hans" */ )
	:CTranslateRequest( from, to )
{
	guard = 0;
	admin = 0;
	UL = 0;
	has_medal = false;
	has_rank = false;
	medal_level = 0;
	sc = false;
}

CDanmakuTranslateRequest::~CDanmakuTranslateRequest( void )
{

}


void CDanmakuTranslateRequest::OnCompleted( void )
{
	const char *text = NULL;

	if( m_json != NULL )
	{
		json_unpack( m_json, "[{s:[{s:s}]}]", "translations", "text", &text );

		auto u_original_text = UTIL_UTF8ToUnicode( m_original_text.c_str( ) );
		auto u_translated_text = UTIL_UTF8ToUnicode( text );
		auto u_nickname = UTIL_UTF8ToUnicode( nickname.c_str( ) );

		if( this->sc )
		{
			SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),
									 FOREGROUND_GREEN | FOREGROUND_INTENSITY );
		}
		else
		{
			if( (guard > 0 && guard < 3) || admin )
			{
				SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),
										 FOREGROUND_GREEN | FOREGROUND_RED |
										 FOREGROUND_INTENSITY );
			}
			else
			{
				SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),
										 FOREGROUND_GREEN | FOREGROUND_RED |
										 FOREGROUND_INTENSITY );
			}
		}


		wprintf( L"[%s][UL %02d]", u_nickname, UL );

		if( has_medal )
		{
			auto u_medal = UTIL_UTF8ToUnicode( medal.c_str( ) );

			wprintf( L"[%s LV.%02d]", u_medal, medal_level );

			free( u_medal );
		}

		if( has_rank )
		{
			auto u_rank = UTIL_UTF8ToUnicode( rank.c_str( ) );
			wprintf( L"[Consumer Rankings:%s]", u_rank );
			free( u_rank );
		}

		wprintf( L"\n" );

		wprintf( L"[ORIGINAL] %s\n", u_original_text );
		wprintf( L"[TRANSLATE] %s\n", u_translated_text );
		wprintf( L"===================================================\n" );

		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),
								 FOREGROUND_RED |
								 FOREGROUND_GREEN |
								 FOREGROUND_BLUE );

		delete[] u_original_text;
		delete[] u_translated_text;
	}
}

void CDanmakuTranslateRequest::setUL( int level )
{
	UL = level;
}

void CDanmakuTranslateRequest::setMedal( const char *medal, int level )
{
	this->medal = medal;
	this->medal_level = level;
	this->has_medal = true;
}

void CDanmakuTranslateRequest::setNickname( const char *nickname )
{
	this->nickname = nickname;
}

void CDanmakuTranslateRequest::setSpecial( int guard, bool admin )
{
	this->guard = guard;
	this->admin = admin;
}

void CDanmakuTranslateRequest::setRank( const char *rank )
{
	this->rank = rank;
	this->has_rank = true;
}

void CDanmakuTranslateRequest::setSC( bool f )
{
	this->sc = f;
}