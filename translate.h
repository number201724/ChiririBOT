#pragma once


class CTranslateRequest :public CWebJsonRequest
{
public:
	CTranslateRequest( const char *from = "ja", const char *to = "zh-Hans" );
	virtual ~CTranslateRequest( void );

	// virtual void OnCompleted( void );

	void setText( const char *text );

protected:
	char *m_postdata;
	std::string m_original_text;
	struct curl_slist *m_headers;
};

class CDanmakuTranslateRequest : public CTranslateRequest
{
public:
	CDanmakuTranslateRequest( const char *from = "ja", const char *to = "zh-Hans" );
	virtual ~CDanmakuTranslateRequest( void );
	virtual void OnCompleted( void );
	void setUL( int level );
	void setMedal( const char *medal, int level );
	void setNickname( const char *nickname );
	void setSpecial( int guard, bool admin );
	void setRank( const char *rank );
	void setSC( bool f );

protected:
	std::string nickname;
	int guard;
	bool admin;
	int UL;

	bool has_medal;
	std::string medal;
	int medal_level;

	bool has_rank;
	std::string rank;
	bool sc;
};