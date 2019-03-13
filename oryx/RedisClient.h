#include "hiredis/hiredis.h"
#include <map>
#include <vector>
class RedisClient
{
public:
	RedisClient(const char * ip, unsigned short port, const char * pswd);
	~RedisClient();

	//重连 (<0失败)
	int	connectRedis();

	// 返回: <0 错误
	int ping();

	//设置生存时间
	int execExpire(const char * key, int second);

	//获得内存块数据
	int execGet(const char * key, void * buf, int maxlen);
	
	//设置内存块
	int execSet(const char * key, const void * buf, int len);

	/*
	 * 将值 value 关联到 key ，并将 key 的生存时间设为 seconds (以秒为单位)。
	 * 如果 key 已经存在， SETEX 命令将覆写旧值。
	 */
	int execSetEx(const char * key, const void * buf, int len, unsigned int seconds);

	//设置数字类型
	int execSet(const char * key, long long value);

	/*
	 * SETEX key seconds value
	 * 将值 value 关联到 key ，并将 key 的生存时间设为 seconds (以秒为单位)。
	 * 如果 key 已经存在， SETEX 命令将覆写旧值。
	 */
	int execSetEx(const char * key, long long value, unsigned int seconds);

	/*
	 * INCRBY key increment
	 * 将 key 所储存的值加上增量 increment 。负数表示减
	 * 如果 key 不存在，那么 key 的值会先被初始化为 0 ，然后再执行 INCRBY 命令。
	 * 如果值包含错误的类型，或字符串类型的值不能表示为数字，那么返回一个错误。
	 * 本操作的值限制在 64 位(bit)有符号数字表示之内。
	 */
	int execIncrby(const char * key, long long increment);

	// 返回：>=0：成功；-1：错误；
	int execHSetL(const char * key, int function, long value);

    // 返回：>=0：成功；-1：错误；-2：空值
	long execHGetL(const char * key, int function);

	// 返回：>=0：成功；-1：错误
	int execHSetI(const char * key, const char * function, int value);

	// 返回：>=0：成功；-1：错误；-2：空值
	int execHGetI(const char * key, const char * function);

	// 返回：>=0：成功；-1：错误
	int execHSetS(const char * key, const char * function, const char * buf);

	// 返回：>=0：成功；-1：错误；-2：空值
	int execHGetS(const char * key, const char * function, char * buf, int maxlen);

	// 返回：>=0：成功；-1：错误
	int execHSet(const char * key, const char * field, void * buff, int len);

	// 返回：>=0：成功；-1：错误
	int execHGet(const char * key, const char * field, void * buff, int maxlen);

	// 返回：>=0：成功；-1：错误
	int execHGetAll(const char * key, std::map<std::string, std::string>& result);

	// 返回：>=0：成功；-1：错误
	int execHMSet(const char * key, const std::map<std::string, std::string>& function);

	// 返回：>=0：成功；-1：错误
	int execHExists(const char * key, const char * field);

	// 返回：>=0：成功；-1：错误
	int execHDel(const char * key, const char * function);
	int execHDel(const char * key, int field);

	// 返回：>=0：链表条数；-1：错误
	int execRPush(const char * key, const char * function);

	// 返回：>=0：字符串长度；-1：错误；-2：空值
	int execRPop(const char * key, char * buf, int maxlen);

	// 返回：>=0：成功；-1：错误
	int execZAdd(const char * key, long function, const char * value);

	/**
	 * ZINCRBY key increment member
	 * 为有序集 key 的成员 member 的 score 值加上增量 increment 。
	 * 可以通过传递一个负数值 increment ，让 score 减去相应的值，比如 ZINCRBY key -5 member ，就是让 member 的 score 值减去 5 。
	 * 当 key 不存在，或 member 不是 key 的成员时， ZINCRBY key increment member 等同于 ZADD key increment member
	 * 当 key 不是有序集类型时，返回一个错误。
	 * score 值可以是整数值或双精度浮点数。
	*/
	int execZincrby(const char * key, long long increment, const char * member);


	// 返回：>=0：成功；-1：错误；-2：空值
	int execZRangeByScore(const char * key, long function, std::vector<std::string>& vElement);

	// 返回：>=0：成功；-1：错误
	int execZRemRangeByScore(const char * key, long function);

	int testexecGet(const char * key, void * buf, int maxlen);
	
	// 返回: <0 则buf里是错误信息; 0则OK
	int testexecSet(const char * key, const void * buf, int len);
	
	/**
	 *
	 * SADD key member [member ...]
	 * 将一个或多个 member 元素加入到集合 key 当中，已经存在于集合的 member 元素将被忽略.
	 * 假如 key 不存在，则创建一个只包含 member 元素作成员的集合。
	 * 当 key 不是集合类型时，返回一个错误。
	 */
	int sadd(const char * key, const char * value);
	int sadd(const char * key, int value);

	/**
	 * SMOVE source destination member
	 * 将 member 元素从 source 集合移动到 destination 集合。
	 * SMOVE 是原子性操作。
	 * 如果 source 集合不存在或不包含指定的 member 元素，则 SMOVE 命令不执行任何操作，仅返回 0 。否则， member 元素从 source 集合中被移除，并添加到 destination 集合中去。
	 * 当 destination 集合已经包含 member 元素时， SMOVE 命令只是简单地将 source 集合中的 member 元素删除。
	 * 当 source 或 destination 不是集合类型时，返回一个错误。
	*/
	int smove(const char * sourceKey, const char * destinationKey, const char * value);
	int smove(const char * sourceKey, const char * destinationKey, int value);

	/**
	 * SREM key member [member ...]
	 * 移除集合 key 中的一个或多个 member 元素，不存在的 member 元素会被忽略
	 * 当 key 不是集合类型，返回一个错误。
	 */
	int srem(const char * key, long long member);
	int srem(const char * key, const char * member);

	int hincrBy(const char * key, const char * field, long long number);

	/*
	 * SMEMBERS key
	 * 返回集合 key 中的所有成员。
	 * 不存在的 key 被视为空集合。
	 * 可用版本：
	 * >= 1.0.0
	 * 时间复杂度:
	 * O(N)， N 为集合的基数。
	 * 返回值:
	 * 集合中的所有成员。
	 */
	int smembers(const char * key, std::vector<std::string>& allMembers);

	/**
	 * DEL key [key ...]
	 * 删除给定的一个或多个 key 。
	 * 不存在的 key 会被忽略。
	 */
	int del(const char * key);

protected:
	int _auth();

public:
	char			m_ip[20];
	char			m_pswd[128];
	unsigned short	m_port;
	char			m_errmsg[128];
	redisContext*	m_c;
};