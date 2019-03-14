#ifndef __CONFIG_MANAGER__
#define __CONFIG_MANAGER__

#include "../oryx/common.h"
#include "ConfigStruct.h"
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
    bool daemon_process;
    bool print_screen;
    Log_Info log_game;
    Log_Info log_epoll;
    Log_Info log_async;
    Redis_List redis_list;
};

#endif