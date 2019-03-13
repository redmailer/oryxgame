#ifndef __CONFIG_MANAGER__
#define __CONFIG_MANAGER__

#include "../oryx/common.h"
#include <string>

using namespace std;

class ConfigManager {

    SINGLETON_DECLEAR(ConfigManager)

public:
    virtual ~ConfigManager() {};

	virtual bool init();

    bool loadServerConfig();

public:
    string server_name;
    string listen_addr;
    UINT16  listen_port;
    INT32 io_thread_num;
};

#endif