#include "ConfigManager.h"
#include "../oryx/LogManager.h"

SINGLETON_DEFINE(ConfigManager)

ConfigManager:ConfigManager(){

}

bool ConfigManager::init() {
    if(!this->loadServerConfig()){
        TRACEERROR("loadServerConfig failed");
        return false;
    }
    return true;
}

bool ConfigManager::loadServerConfig() {
    return false;
}