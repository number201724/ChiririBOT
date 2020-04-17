#pragma once


class client_t
{
public:
	void ParseServerMessage( json_t *object );
};

extern client_t cl;

void
ParseDanmakuMsg( 
	json_t *object
);

void
ParseSendGift(
	json_t *object
);


void
ParseSuperChat(
	json_t *object
);

void
ParseGuardBuy(
	json_t *object
);