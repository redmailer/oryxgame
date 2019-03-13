#include "ConfigManager.h"
#include "../oryx/LogManager.h"
#include <fstream>
#include <iostream>
#include "json/json.h"
SINGLETON_DEFINE(ConfigManager)

using namespace std;
ConfigManager::ConfigManager(){

}

bool ConfigManager::init() {
    if(!this->loadServerConfig()){
        TRACEERROR("loadServerConfig failed");
        return false;
    }
    return true;
}

bool ConfigManager::loadServerConfig() {
    TRACEINFO("loadServerConfig");
    ifstream config;
    config.open("../config/config.json", std::ios::binary);
    if(!config.is_open()){
        return false;
    }
    Json::Reader reader;  
    Json::Value root;  
    if (!reader.parse(config, root, false))  
    {  
        return false;
    } 
    this->server_name = root["server_name"].asString();
    this->listen_addr = root["listen_addr"].asString();
    this->listen_port = root["listen_port"].asInt();
    this->io_thread_num = root["io_thread_num"].asInt();
    TRACEINFO("server_name:%d",server_name);
    TRACEINFO("listen_addr:%d",listen_addr);
    TRACEINFO("listen_port:%d",listen_port);
    TRACEINFO("io_thread_num:%d",io_thread_num);
    return true;
}