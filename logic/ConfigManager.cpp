#include "ConfigManager.h"
#include "../oryx/LogManager.h"
#include<fstream>
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
    config.open("../config/config.json");
    if(!config.is_open()){
        return false;
    }
    Json::Reader reader;  
    Json::Value root;  
    if (!reader.parse(str, root))  
    {  
        return false;
    } 
    std::string server_name = root["server_name"].asString();  // 访问节点，upload_id = "UP000000" 
    cout << "test server_name" << server_name << endl;
    return false;
}