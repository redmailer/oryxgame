#ifndef __CONFIG_MANAGER__
#define __CONFIG_MANAGER__

#include "../oryx/common.h"
#include <unordered_map>
#include "Player.h"

class ConfigManager {

    SINGLETON_DECLEAR(ConfigManager)

public:
    virtual ~ConfigManager() {};

	virtual bool init();

    bool loadServerConfig();

public:

}

#endif