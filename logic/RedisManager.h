#ifndef __REDIS_MANAGER__
#define __REDIS_MANAGER__

#include "../oryx/common.h"
#include "../oryx/RedisClient.h"
#include <vector>
#include "ConfigStruct.h"


typedef std::vector<RedisClient *>  RedisClientVec;
using namespace std;

class RedisManager {

    SINGLETON_DECLEAR(RedisManager)

public:
    virtual ~RedisManager() {};
	virtual bool init(Redis_List & redis_list);

    void checkRedisConnection();
    RedisClient * getRedisClient(INT32 id = 0);

private:
    RedisClientVec	m_redisClients;

};

#endif