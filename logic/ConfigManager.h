#ifndef __CONFIG_MANAGER__
#define __CONFIG_MANAGER__

#include "../oryx/common.h"
#include <unordered_map>
#include "Player.h"

class ClientConnManager {

    SINGLETON_DECLEAR(ClientConnManager)

public:
    virtual ~ClientConnManager() {};

	virtual bool init();

    bool loadServerConfig();

public:

}

#endif