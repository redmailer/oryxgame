#include "RedisManager.h"
#include "../oryx/LogManager.h"

SINGLETON_DEFINE(RedisManager)

using namespace std;
RedisManager::RedisManager(){

}

bool RedisManager::init(Redis_List & redis_list) {
    for(INT32 i = 0 ; i < redis_list.size(); i ++){
        Redis_Info* redis_info = redis_list[i];
        RedisClient * redis_client = ORYX_NEW(RedisClient, redis_info->addr, redis_info->port, redis_info->auth, redis_info->id);
        this->m_redisClients.push_back(redis_client);
        TRACEINFO("add RedisClient id:%d %s:%d auth:%s",redis_info->id,redis_info->addr, redis_info->port, redis_info->auth);
        INT32 result = redis_client->connectRedis();
        if(result < 0){
            TRACEERROR("RedisClient id:%d connect failed, result:%d",redis_info->id,result);
        }
    }
    return true;
}

void RedisManager::checkRedisConnection() {
    RedisClientVec::iterator it = this->m_redisClients.begin();
    for(; it != this->m_redisClients.end(); it ++){
        if((*it)->m_connStatus && (*it)->ping() < 0){
            TRACEERROR("RedisClient id:%d disconnected",(*it)->m_id);
            (*it)->m_connStatus = false;
        }
        if(!(*it)->m_connStatus){
            INT32 result = (*it)->connectRedis();
            if(result < 0){
                TRACEERROR("RedisClient id:%d connect failed, result:%d",(*it)->m_id,result);
            }
        }
    }
}

