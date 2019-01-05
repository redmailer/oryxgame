#ifndef __PLAYER__
#define __PLAYER__

#include "../oryx/common.h"

#define MAX_PLAYER_NAME_LEN 128


class Player {

public:
	Player() {};

public:
	INT64 playerID;
	char name[MAX_PLAYER_NAME_LEN];

};



#endif
