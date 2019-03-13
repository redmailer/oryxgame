#ifndef __REDIS_MANAGER__
#define __REDIS_MANAGER__

#include "../oryx/common.h"
#include "../oryx/RedisClient.h"
#include <vector>


typedef std::vector<RedisClient *>  RedisClientVec;
using namespace std;

class RedisManager {

    SINGLETON_DECLEAR(RedisManager)

public:
    virtual ~RedisManager() {};
	virtual bool init();

private:
    RedisClientVec	m_redisClients;

};

#endif