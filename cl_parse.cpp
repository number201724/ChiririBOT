#include "pch.h"
#include "common.h"
#include "sys.h"
#include "protocol.h"
#include "client.h"
#include "http.h"
#include "translate.h"

client_t cl;

void client_t::ParseServerMessage( json_t *object )
{
	const char *cmd;
	if( json_unpack( object, "{s:s}", "cmd", &cmd ) )
		return;


	if( !strncmp( cmd, "DANMU_MSG", charsmax( "DANMU_MSG" ) ) )
	{
		ParseDanmakuMsg( object );
		return;
	}

	if( !strncmp( cmd, "SEND_GIFT", charsmax( "SEND_GIFT" ) ) )
	{
		ParseSendGift( object );
		return;
	}

	if( !strncmp( cmd, "GUARD_BUY", charsmax( "GUARD_BUY" ) ) )
	{
		ParseGuardBuy( object );
		return;
	}

	if( !strncmp( cmd, "SUPER_CHAT_MESSAGE", charsmax( "SUPER_CHAT_MESSAGE" ) ) )
	{
		ParseSuperChat( object );
		return;
	}

	//OutputDebugStringA( cmd );
	//OutputDebugStringA( "\n" );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : 
// Output : 
//-----------------------------------------------------------------------------
void
ParseDanmakuMsg(
	json_t *object
)
{
	danmaku_data_t dmk;
	json_t *j_dmkinfo, *j_userinfo, *j_medal, *j_rank, *j_title, *junkno;
	const char *text;
	int code;

	memset( &dmk, 0, sizeof( dmk ) );

	code = json_unpack( object, "{s:[o,s,o,o,o,o,o,i]}",
						"info",
						&j_dmkinfo,
						&text,
						&j_userinfo,
						&j_medal,
						&j_rank,
						&j_title,
						&junkno,
						&dmk.guard
	);

	if( code )
		return;


	if( j_dmkinfo )
	{
		const char *userhash;

		code = json_unpack( j_dmkinfo, "[i,i,i,i,I,i,i,s,i,i,i]",
							&dmk.dminfo.zero,
							&dmk.dminfo.mode,
							&dmk.dminfo.fontsize,
							&dmk.dminfo.color,
							&dmk.dminfo.ts,
							&dmk.dminfo.rnd,
							&dmk.dminfo.zero2,
							&userhash,
							&dmk.dminfo.zero3,
							&dmk.dminfo.type,
							&dmk.dminfo.bubble );

		if( code )
		{
			return;
		}

		if( userhash )
		{
			strncpy_s( dmk.dminfo.userhash, userhash, charsmax( dmk.dminfo.userhash ) );
			dmk.dminfo.userhash[charsmax( dmk.dminfo.userhash )] = 0;
		}
	}
	

	if( text )
	{
		strncpy( dmk.text, text, charsmax( dmk.text ) );
		dmk.text[charsmax( dmk.text ) - 1] = 0;
	}

	if( j_userinfo )
	{
		const char *nickname;
		const char *nickcolor;

		code = json_unpack( j_userinfo, "[I,s,i,i,i,i,i,s]",
							&dmk.userinfo.uid,
							&nickname,
							&dmk.userinfo.is_admin,
							&dmk.userinfo.is_vip,
							&dmk.userinfo.is_svip,
							&dmk.userinfo.rank,
							&dmk.userinfo.phone_validate,
							&nickcolor);

		if( code )
		{
			return;
		}

		if( nickname )
		{
			strncpy( dmk.userinfo.nickname, nickname, charsmax( dmk.userinfo.nickname ) );
			dmk.userinfo.nickname[charsmax( dmk.userinfo.nickname ) - 1] = 0;
		}

		if( nickcolor )
		{
			strncpy_s( dmk.userinfo.nickname_color, nickcolor, charsmax( dmk.userinfo.nickname_color ) );
			dmk.userinfo.nickname_color[charsmax( dmk.userinfo.nickname_color )] = 0;
		}
	}

	if( j_medal )
	{
		if( json_array_size( j_medal )  != 0 )
		{
			const char *medal_name;
			const char *anchor_name;
			const char *style;
			code = json_unpack( j_medal, "[i,s,s,i,i,s,i]",
								&dmk.medal.level,
								&medal_name,
								&anchor_name,
								&dmk.medal.anchor_id,
								&dmk.medal.color,
								&style,
								&dmk.medal.modal_icon_id
			);

			if( code )
				return;

			if( medal_name )
			{
				strncpy( dmk.medal.medal_name, medal_name, charsmax( dmk.medal.medal_name ) );
				dmk.medal.medal_name[charsmax( dmk.medal.medal_name ) - 1] = 0;
			}

			if( anchor_name )
			{
				strncpy( dmk.medal.anchor_name, anchor_name, charsmax( dmk.medal.anchor_name ) );
				dmk.medal.anchor_name[charsmax( dmk.medal.anchor_name ) - 1] = 0;
			}

			if( style )
			{
				strncpy_s( dmk.medal.style, style, charsmax( dmk.medal.style ) );
				dmk.medal.style[charsmax( dmk.medal.style )] = 0;
			}

			dmk.has_medal = true;
		}
		else
		{
			dmk.has_medal = false;
		}
	}

	if( j_rank )
	{
		json_t *rank;
		code = json_unpack( j_rank, "[i,i,i,o]",
							&dmk.rank.level,
							&dmk.rank.zero,
							&dmk.rank.color,
							&rank
		);

		if( code )
		{
			_tprintf( TEXT( "json_unpack failed.\n" ) );
			return;
		}

		if( rank && json_typeof( rank ) == JSON_STRING )
		{
			strncpy_s( dmk.rank.rank, json_string_value( rank ), charsmax( dmk.rank.rank ) );
			dmk.rank.rank[charsmax( dmk.rank.rank )] = 0;
		}

		if( rank && json_typeof( rank ) == JSON_INTEGER )
		{
			_snprintf_s( dmk.rank.rank, charsmax( dmk.rank.rank ), "%d",  (int)json_integer_value( rank ) );
			dmk.rank.rank[charsmax( dmk.rank.rank )] = 0;
		}
	}

	if( j_title )
	{
		if( json_array_size( j_title ) != 0 )
		{
			const char *oldname, *newname;
			
			code = json_unpack( j_title, "[s,s]",
								&oldname,
								&newname 
			);

			if( code )
				return;

			if( oldname )
			{
				strncpy_s( dmk.title.oldname, oldname, charsmax( dmk.title.oldname ) );
				dmk.title.oldname[charsmax( dmk.title.oldname )] = 0;
			}

			if( newname )
			{
				strncpy_s( dmk.title.newname, newname, charsmax( dmk.title.newname ) );
				dmk.title.newname[charsmax( dmk.title.newname )] = 0;
			}

			dmk.has_title = true;
		}
		else
		{
			dmk.has_title = false;
		}
	}
	CDanmakuTranslateRequest *request;

	if( dmk.dminfo.type != 0 )
		return;
#ifdef ONLY_GUARD
	if( dmk.guard == 0 && !dmk.userinfo.is_admin )
		return;

	request = new CDanmakuTranslateRequest( "zh-Hans", "ja" );
#else

#ifdef ONLY_NANA
	if( dmk.userinfo.uid != 386900246 )
		return;

	request = new CDanmakuTranslateRequest( "ja", "zh-Hans" );
#else
	request = new CDanmakuTranslateRequest( "zh-Hans", "ja" );
#endif

#endif
	

	request->setText( dmk.text );
	request->setNickname( dmk.userinfo.nickname );
	request->setSpecial( dmk.guard, dmk.userinfo.is_admin );
	request->setRank( dmk.rank.rank );
	if( dmk.has_medal )
		request->setMedal( dmk.medal.medal_name, dmk.medal.level );

	request->setUL( dmk.rank.level );

	gHttp.Run( request );

}


//-----------------------------------------------------------------------------
// Purpose:
// Input  : 
// Output : 
//-----------------------------------------------------------------------------
void
ParseSendGift(
	json_t *object
)
{
	const char *giftName;
	int num;
	const char *uname;
	const char *face;
	int guard_level;
	json_int_t uid;
	json_int_t timestamp;
	int giftId;
	int giftType;
	const char *coin_type;

	int code;

	code = json_unpack( object, "{s:{s:s,s:i,s:s,s:s,s:i,s:I,s:I,s:i,s:i,s:s}}", "data",
						"giftName", &giftName,
						"num", &num,
						"uname", &uname,
						"face", &face,
						"guard_level", &guard_level,
						"uid", &uid,
						"timestamp", &timestamp,
						"giftId", &giftId,
						"giftType", &giftType,
						"coin_type", &coin_type
	);

	if( code )
		return;

	//OutputDebugStringA( json_dumps( object, 0 ) );
	//OutputDebugStringA( "\n" );
}


//-----------------------------------------------------------------------------
// Purpose:
// Input  : 
// Output : 
//-----------------------------------------------------------------------------
void
ParseSuperChat(
	json_t *object
)
{
	superchat_data_t sc;
	const char *message;
	int code;
	json_t *medal_info, *user_info;

	medal_info = NULL;
	user_info = NULL;
	int user_level;

	//OutputDebugStringA( json_dumps( object,0 ) );

	code = json_unpack( object, "{s:{s:I,s:I,s:s,s:o,s:o}}", "data",
						"uid", &sc.uid,
						"price", &sc.price,
						"message", &message,
						"medal_info", &medal_info,
						"user_info", &user_info
	);

	if( code )
	{
		return;
	}


	strncpy_s( sc.message, message, charsmax( sc.message ) );
	sc.message[charsmax( sc.message )] = 0;

	const char *nickname;
	int guard_level;

	code = json_unpack( user_info, "{s:s,s:i,s:i}",
						"uname", &nickname,
						"guard_level", &guard_level,
						"user_level", &user_level
	);

	if( code ) {
		return;
	}

#ifndef ONLY_NANA

	CDanmakuTranslateRequest *request = new CDanmakuTranslateRequest( "zh-Hans", "ja" );

	request->setText( message );
	request->setNickname( nickname );
	request->setSpecial( guard_level, false );
	request->setUL( user_level );
	request->setSC( true );

	gHttp.Run( request );
#endif
}


//-----------------------------------------------------------------------------
// Purpose:
// Input  : 
// Output : 
//-----------------------------------------------------------------------------
void
ParseGuardBuy(
	json_t *object
)
{
	//OutputDebugStringA( json_dumps( object, 0 ) );
	//OutputDebugStringA( "\n" );
}