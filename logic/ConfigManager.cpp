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
    this->listen_port = root["listen_port"].asUInt();
    this->io_thread_num = root["io_thread_num"].asInt();
    this->daemon_process = root["daemon_process"].asBool();
    TRACEINFO("server_name:%s",server_name.c_str());
    TRACEINFO("listen_addr:%s",server_name.c_str());
    TRACEINFO("listen_port:%d",listen_port);
    TRACEINFO("io_thread_num:%d",io_thread_num);
    TRACEINFO("daemon_process:%d",daemon_process);

    this->print_screen = root["print_screen"].asBool();
    strncpy(this->log_game.path, root["log_game"]["path"].asString().c_str(), sizeof(this->log_game.path));
    this->log_game.log_level, root["log_game"]["log_level"].asInt();
    strncpy(this->log_epoll.path, root["log_epoll"]["path"].asString().c_str(), sizeof(this->log_epoll.path));
    this->log_epoll.log_level, root["log_epoll"]["log_level"].asInt();
    strncpy(this->log_async.path, root["log_async"]["path"].asString().c_str(), sizeof(this->log_async.path));
    this->log_async.log_level, root["log_async"]["log_level"].asInt();
    TRACEINFO("log_game  path: %s , level:%d", this->log_game.path, this->log_game.log_level);
    TRACEINFO("log_epoll  path: %s , level:%d", this->log_epoll.path, this->log_epoll.log_level);
    TRACEINFO("log_async  path: %s , level:%d", this->log_async.path, this->log_async.log_level);

    INT32 redis_len = root["redis"].size();
    for(INT32 i = 0 ; i < redis_len; i++){
        Redis_Info * redis_info = ORYX_NEW(Redis_Info);
        strncpy(redis_info->addr, root["redis"][i]["addr"].asString().c_str(), sizeof(redis_info->addr));
        strncpy(redis_info->auth, root["redis"][i]["auth"].asString().c_str(), sizeof(redis_info->auth));
        redis_info->port = root["redis"][i]["port"].asInt();
        TRACEINFO("redis :%d ip:%s port:%d auth:%s",i,redis_info->addr,redis_info->port,redis_info->auth);
        this->redis_list.push_back(redis_info);
    }

    return true;
}