#include "RedisManager.h"
#include "../oryx/LogManager.h"

SINGLETON_DEFINE(RedisManager)

using namespace std;
RedisManager::RedisManager(){

}

bool RedisManager::init() {

    return true;
}