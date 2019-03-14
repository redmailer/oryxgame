#include "RedisClient.h"
#include "../oryx/LogManager.h"

#include <sstream>
#include <string.h>

RedisClient::RedisClient(const char * ip, unsigned short port, const char * pswd, int id)
{
	strncpy(m_ip, ip, sizeof(m_ip));
	strncpy(m_pswd, pswd, sizeof(m_pswd));
	m_port = port;
	m_c = NULL;
	m_id = id;
	m_connStatus = false;
}

RedisClient::~RedisClient()
{
	redisFree(m_c);
}

int RedisClient::connectRedis() {
	if (m_c){
		//如果m_c不是空，需要将连接释放，在重新建立连接
		redisFree(m_c);
		m_c = NULL;
	}

	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	m_c = redisConnectWithTimeout(m_ip, m_port, tv);
	if(NULL == m_c) {
		TRACEERROR("alloc m_c error");
		return -1;
	}

	if(m_c->err) {
		redisFree(m_c);	
		m_c = NULL;
		TRACEERROR("m_c error free");
		return -2;
	}
	if(m_pswd[0] && _auth() < 0) {
		TRACEERROR("# Redis AUTH %s:%u fail!!", m_ip, m_port);
		return -1;
	}
	TRACEINFO("connect redis success, ip:%s, port:%d", m_ip, m_port);

	m_connStatus = true;
	return 0;
}

int RedisClient::_auth() {
	if(!m_c){
		TRACEERROR("auth, redisContext object is nil");
		return -1;
	}
	char cmd[128];
	snprintf(cmd, 127, "AUTH %s", m_pswd);
	redisReply* r = (redisReply*)redisCommand(m_c, cmd);
	if(NULL == r) {
		redisFree(m_c);
		m_c = NULL;
		TRACEINFO("alloc reply error, cmd:%s",cmd);
		return -2;
	}
	if(!(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str,"OK")==0)){
		if (r->type == REDIS_REPLY_ERROR and strcasecmp(r->str, "ERR Client sent AUTH, but no password is set") == 0){
			TRACEINFO("ERR Client sent AUTH, but no password is set");
		}else{
			TRACEERROR("Failed to execute command[%s]",cmd);
			freeReplyObject(r);
			redisFree(m_c);
			m_c = NULL;
			return -3;
		}
	}
	freeReplyObject(r);
	return 1;
}

int RedisClient::ping() {
	if(!m_c){
		TRACEERROR("ping, redisContext object is nil");
		return -1;
	}
	char cmd[128];
	snprintf(cmd, 127, "PING");
	redisReply* r = (redisReply*)redisCommand(m_c, cmd);
	if(NULL == r) {
		return -2;
	}

	if(r->type != REDIS_REPLY_STATUS) {
		freeReplyObject(r);                                                                     
		return -3;
	}

	if(strcasecmp(r->str,"PONG") != 0) {
		freeReplyObject(r);                                                                     
		return -4;
	}
	freeReplyObject(r);
	return 1;
}

//设置生存时间
int RedisClient::execExpire(const char * key, int second) {
	if(!m_c){
		TRACEERROR("execExpire, redisContext object is nil, key:%s", key);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "EXPIRE %s %d", key, second);
	if(NULL == r){
		TRACEERROR("r is nil,cmd:[EXPIRE %s %d]", key, second);
		return -2;
	}
	if(r->type == REDIS_REPLY_NIL) {
		TRACEERROR("r->type is nil,cmd:[EXPIRE %s %d]", key, second);
		freeReplyObject(r);
		return -3;
	}
	int ret = r->integer;
	freeReplyObject(r);
	return ret;
}

// GET test		$2  s1
// GET zzz		$-1
int RedisClient::execGet(const char * key, void * buf, int maxlen) {
	char cmd[1024];
	bzero(cmd, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "GET %s", key);
	if(!m_c){
		TRACEERROR("execGet, redisContext object is nil, cmd:%s", cmd);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, cmd);  
	if(!r){
		TRACEERROR("r is nil,cmd:[%s]", cmd);
		return -2;
	}
	if(r->type == REDIS_REPLY_NIL){
		TRACEERROR("r->type is nil,cmd:[ %s]",cmd);
		freeReplyObject(r);
		return -3;
	}
	if(r->type != REDIS_REPLY_STRING){
		TRACEERROR("redis execGet,type:%d,key:%s,len:%d", r->type, key, r->len);
		freeReplyObject(r);
		return -4;
	}
	int datalen = r->len;
	if(datalen < 0){
		freeReplyObject(r);
		TRACEERROR("execGet error, data length < 0");
		return -5;
	}
	memcpy(buf, r->str, (datalen < maxlen)? datalen : maxlen); 
	freeReplyObject(r);
	return datalen;
}

// SET test 3\r\n abc  +OK
// *3\r\n$3\r\nSET\r\n$6\r\ntest11\r\n$8\r\n123456789\r\n   
int RedisClient::execSet(const char * key, const void * buf, int len) {
	if(!m_c){
		TRACEERROR("execSet, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "SET %b %b", key, strlen(key), buf, (size_t)len);
	if(!r){
		TRACEERROR("execSet r is nil,cmd:[SET %s]", key);
		return -2;
	}

	if(r->type != REDIS_REPLY_STATUS) {
		TRACEERROR("execSet r->type error, type:%d", r->type);
		freeReplyObject(r);
		return -3;
	}

	if(!r->str || strcasecmp(r->str, "OK"))  {
		freeReplyObject(r);
		TRACEERROR("set ok fail, str;%s", r->str);
		return -4;
	}
	freeReplyObject(r);
	return 0;
}
/*
 * 将值 value 关联到 key ，并将 key 的生存时间设为 seconds (以秒为单位)。
 * 如果 key 已经存在， SETEX 命令将覆写旧值。
 */
int RedisClient::execSetEx(const char * key, const void * buf, int len, unsigned int seconds){
	if(!m_c){
		TRACEERROR("execSetEx, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "SETEX %b %u %b ", key, strlen(key), seconds, buf, (size_t)len);
	if(!r){
		TRACEERROR("execSetEx r is nil,cmd:[SET %s]", key);
		return -2;
	}

	if(r->type != REDIS_REPLY_STATUS) {
		printf("r->type error, type:%d,key:%s\n", r->type, key);
		freeReplyObject(r);
		return -3;
	}

	if(!r->str || strcasecmp(r->str, "OK")) {
		freeReplyObject(r);
		printf("set ok fail\n");
		return -4;
	}
	freeReplyObject(r);
	return 0;
}

int RedisClient::execSet(const char * key, long long value)
{
	if(!m_c){
		TRACEERROR("execSet, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "SET %s %lld", key, value);

	if(!r){
		TRACEERROR("execSet r is nil,cmd:[SET %s]", key);
		return -2;
	}
	if(r->type != REDIS_REPLY_STATUS) {
		freeReplyObject(r);
		TRACEERROR("execSet r->type error, type:%d,key:%s",r->type, key);
		return -3;
	}

	if(!r->str || strcasecmp(r->str, "OK")) {
		freeReplyObject(r);
		TRACEERROR("set ok fail, key:%s", key);
		return -4;
	}

	freeReplyObject(r);
	return 0;	
}

/*
 * 将值 value 关联到 key ，并将 key 的生存时间设为 seconds (以秒为单位)。
 * 如果 key 已经存在， SETEX 命令将覆写旧值。
 */
int RedisClient::execSetEx(const char * key, long long value, unsigned int seconds){
	if(!m_c){
		TRACEERROR("execSetEx, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "SETEX %s %u %lld", key, seconds, value);
	if(!r){
		TRACEERROR("execSetEx r is nil,cmd:[SET %s]", key);
		return -2;
	}
	if(r->type != REDIS_REPLY_STATUS){
		freeReplyObject(r);
		TRACEERROR("execSetEx r->type error, type:%d,key:%s",r->type, key);
		return -3;
	}
	if(!r->str || strcasecmp(r->str, "OK")){
		freeReplyObject(r);
		TRACEERROR("set ok fail, key:%s", key);
		return -4;
	}
	freeReplyObject(r);
	return 0;
}

int RedisClient::execIncrby(const char * key, long long increment){
	if(!m_c){
		TRACEERROR("execIncrby, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* rSet = (redisReply*)redisCommand(m_c, "INCRBY %s %lld ", key, increment);
	if(!rSet){
		TRACEERROR("execIncrby r is nil,cmd:[SET %s]", key);
		return -2;
	}
	if(rSet->type != REDIS_REPLY_INTEGER){
		freeReplyObject(rSet);
		TRACEERROR("execIncrby r->type error, type:%d,key:%s",rSet->type, key);
		return -3;
	}
	freeReplyObject(rSet);
	return 0;
}

// 返回：>=0：成功；-1：错误；
int RedisClient::execHSetL(const char * key, int function, long value) {
	if(m_c == NULL){
		TRACEERROR("execHSetL, redisContext object is nil, key:%s", key);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "HSET %s %d %ld", key, function, value);

	if(NULL == r)
 		return -2;
 
	if(r->type != REDIS_REPLY_INTEGER)
	{
		freeReplyObject(r);
		return -3;
	}

	int ret = r->integer;

	freeReplyObject(r);

	return ret;	
}

// 返回：>=0：成功；-1：错误；-2：空值
long RedisClient::execHGetL(const char * key, int function)
{
	if(m_c == NULL){
		TRACEERROR("execHGetL, redisContext object is nil, key:%s", key);
 		return -1;
	}
 
	redisReply* r = (redisReply*)redisCommand(m_c, "HGET %s %d", key, function);

	if(NULL == r){
		return -2;
	}

	if(r->type == REDIS_REPLY_NIL) {
 		freeReplyObject(r);
		return -3;
	}

	if(r->type != REDIS_REPLY_STRING) {
 		freeReplyObject(r);
		return -4;
	}

	long ret = atol(r->str);
	freeReplyObject(r);

	return ret;
}

// 返回：>=0：成功；-1：错误
int RedisClient::execHSetI(const char * key, const char * function, int value)
{
	if(!m_c ){
		TRACEERROR("execHSetI, redisContext object is nil, key:%s", key);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "HSET %s %s %d", key, function, value);

	if(NULL == r){
		return -2;
	}

	if(r->type != REDIS_REPLY_INTEGER) {
		freeReplyObject(r);
		return -3;
	}

	int ret = r->integer;
	freeReplyObject(r);
	return ret;	
}

// 返回：>=0：成功；-1：错误；-2：空值
int RedisClient::execHGetI(const char * key, const char * function) {
	if(!m_c){
		TRACEERROR("execHGetI, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "HGET %s %s", key, function);

	if(NULL == r){
		return -2;
	}

	if(r->type == REDIS_REPLY_NIL) {
 		freeReplyObject(r);
		return -3;
	}

	if(r->type != REDIS_REPLY_STRING) {
 		freeReplyObject(r);
		return -4;
	}
	long ret = atol(r->str);
	freeReplyObject(r);
	return ret;
}

// 返回：>=0：成功；-1：错误
int RedisClient::execHSetS(const char * key, const char * function, const char * buf) {
	if(!m_c){
		TRACEERROR("execHSetS, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "HSET %s %s %s", key, function, buf);
	if(NULL == r){
		return -2;
	}

	if(r->type != REDIS_REPLY_INTEGER) {
		freeReplyObject(r);
		return -3;
	}
	int ret = r->integer;
	freeReplyObject(r);
	return ret;	
}

// 返回：>=0：字符串长度；-1：错误；-2：空值
int RedisClient::execHGetS(const char * key, const char * function, char * buf, int maxlen) {
	if(!m_c ){
		TRACEERROR("execHGetS, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "HGET %s %s", key, function);
	if(NULL == r){
		return -2;
	}

	if(r->type == REDIS_REPLY_NIL) {
		freeReplyObject(r);
		return -3;
	}

	if(r->type != REDIS_REPLY_STRING) {
		freeReplyObject(r);
		return -4;
	}

	int datalen = r->len;
	strncpy(buf, r->str, (datalen < maxlen)? datalen : maxlen); 
	freeReplyObject(r);
	return datalen;
}

// 返回：>=0：成功；-1：错误
int RedisClient::execHSet(const char * key, const char * field, void * buff, int len)
{
	if (!m_c){
		TRACEERROR("execHSet, redisContext object is nil, key:%s", key);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "HSET %s %s %b", key, field, buff, (size_t)len);

	if (!r){
		return -2;
	}
	if (r->type != REDIS_REPLY_STATUS) {
		if (m_c->err != 0) {
			freeReplyObject(r);
			return -3;
		}
	}

	if (!r->str || strcasecmp(r->str, "OK")) {
		if (m_c->err != 0) {
			freeReplyObject(r);
			return -4;
		}
	}

	freeReplyObject(r);
	return 0;
}

// 返回：>=0：成功；-1：错误
int RedisClient::execHGet(const char * key, const char * field, void * buff, int maxlen) {
	if (!m_c){
		TRACEERROR("execHGet, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "HGET %s %s", key, field);
	if (!r)
	{
		return -2;
	}

	if (r->type != REDIS_REPLY_STRING)
	{
		freeReplyObject(r);
		return -3;
	}
	int datalen = r->len;
	if (datalen < 0)
	{
		freeReplyObject(r);
		return -4;
	}
	memcpy(buff, r->str, (datalen < maxlen) ? datalen : maxlen);
	freeReplyObject(r);
	return 0;
}

// 返回：>=0：成功删除条数；-1：错误
int RedisClient::execHDel(const char * key, const char * function) {
	if(!m_c){
		TRACEERROR("execHDel, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "HDEL %s %s", key, function);
	if(NULL == r){
		return -2;
	}
	if(r->type != REDIS_REPLY_INTEGER)
	{
		freeReplyObject(r);
		return -3;
	}
	int ret = r->integer;
	freeReplyObject(r);
	return ret;	
}
// 返回：>=0：成功删除条数；-1：错误
int RedisClient::execHDel(const char * key, int field)
{
	if(!m_c){
		TRACEERROR("execHDel, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "HDEL %s %d", key, field);
	if(NULL == r){
		return -2;
	}
	if(r->type != REDIS_REPLY_INTEGER) {
		freeReplyObject(r);
		return -3;
	}

	int ret = r->integer;
	freeReplyObject(r);
	return ret;
}

// 返回：>=0：链表条数；-1：错误
int RedisClient::execRPush(const char * key, const char * function)
{
	if(!m_c){
		TRACEERROR("execRPush, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "RPUSH %s %s", key, function);
	if(NULL == r)
		return -2;

	if(r->type != REDIS_REPLY_INTEGER)
	{
		freeReplyObject(r);
		return -3 ;
	}

	int ret = r->integer;

	freeReplyObject(r);

	return ret;	
}

// 返回：>=0：字符串长度；-1：错误；-2：空值
int RedisClient::execRPop(const char * key, char * buf, int maxlen)
{
	if(!m_c){
		TRACEERROR("execRPop, redisContext object is nil, key:%s", key);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "RPOP %s", key);

	if(NULL == r)
		return -2;

	if(r->type == REDIS_REPLY_NIL)
	{
		freeReplyObject(r);
		return -3;
	}

	if(r->type != REDIS_REPLY_STRING)
	{
		freeReplyObject(r);
		return -4;
	}

	int datalen = r->len;
	strncpy(buf, r->str, (datalen < maxlen)? datalen : maxlen); 

	freeReplyObject(r);
	return datalen;
}

// 返回：>=0：成功；-1：错误
int RedisClient::execZAdd(const char * key, long function, const char * value)
{
	if(!m_c){
		TRACEERROR("execZAdd, redisContext object is nil, key:%s", key);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "ZADD %s %ld %s", key, function, value);

	if(NULL == r)
		return -1;

	if(r->type != REDIS_REPLY_INTEGER)
	{
		freeReplyObject(r);
		return -1;
	}

	int ret = r->integer;

	freeReplyObject(r);

	return ret;	
}

int RedisClient::execZincrby(const char * key, long long increment, const char * member)
{
	if(!m_c){
		TRACEERROR("execZincrby, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "ZINCRBY %s %ld %s", key, increment, member);
	if(NULL == r)
		return -1;

	if(r->type != REDIS_REPLY_INTEGER)
	{
		freeReplyObject(r);
		return -1;
	}

	int ret = r->integer;
	freeReplyObject(r);
	return ret;
}

// 返回：>=0：成功；-1：错误；-2：空值
int RedisClient::execZRangeByScore(const char * key, long function, std::vector<std::string>& vElement)
{
	if(!m_c){
		TRACEERROR("execZRangeByScore, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "ZRANGEBYSCORE %s -inf %ld WITHSCORES", key, function);
	if(NULL == r){
		return -2;
	}

	if(r->type == REDIS_REPLY_NIL) {
		freeReplyObject(r);
		return -3;
	}

	if(r->type != REDIS_REPLY_ARRAY)
	{
		freeReplyObject(r);
		return -4;
	}

	for (unsigned int i = 0; i < r->elements; ++i)  {
		redisReply* childReply = r->element[i];
		if (childReply->type == REDIS_REPLY_STRING) {
			vElement.push_back(std::string(childReply->str));
		} else {
			return -5;
		}
	}
	freeReplyObject(r);
	return 0;
}

// 返回：>=0：成功；-1：错误
int RedisClient::execZRemRangeByScore(const char * key, long function)
{
	if(!m_c){
		TRACEERROR("execZRangeByScore, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "ZREMRANGEBYSCORE %s -inf %ld", key, function);
	if(NULL == r){
		return -2;
	}
	if(r->type != REDIS_REPLY_INTEGER) {
		freeReplyObject(r);
		return -3;
	}
	int ret = r->integer;
	freeReplyObject(r);
	return ret;	
}


int RedisClient::sadd(const char * key, const char * value){
	if(m_c == NULL){
		TRACEERROR("sadd, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "SADD %s %s", key, value);
	if(NULL == r){
		freeReplyObject(r);
		return -2;
	}

	if(r->type != REDIS_REPLY_INTEGER ){
		freeReplyObject(r);
		return -3;
	}
	int ret = r->integer;
	freeReplyObject(r);
	return ret;
}

int RedisClient::sadd(const char * key, int value){
	if(m_c == NULL){
		TRACEERROR("sadd, redisContext object is nil, key:%s", key);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "SADD %s %d", key, value);
	if(NULL == r){
		freeReplyObject(r);
		return -2;
	}

	if(r->type != REDIS_REPLY_INTEGER ){
		freeReplyObject(r);
		return -3;
	}
	int ret = r->integer;
	freeReplyObject(r);
	return ret;
}

/**
 * SMOVE source destination member
 * 将 member 元素从 source 集合移动到 destination 集合。
 * SMOVE 是原子性操作。
 * 如果 source 集合不存在或不包含指定的 member 元素，则 SMOVE 命令不执行任何操作，仅返回 0 。否则， member 元素从 source 集合中被移除，并添加到 destination 集合中去。
 * 当 destination 集合已经包含 member 元素时， SMOVE 命令只是简单地将 source 集合中的 member 元素删除。
 * 当 source 或 destination 不是集合类型时，返回一个错误。
*/
int RedisClient::smove(const char * sourceKey, const char * destinationKey, const char * value){
	if(m_c == NULL){
		TRACEERROR("smove, redisContext object is nil, sourceKey:%s, destinationKey:%s", sourceKey, destinationKey);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "SMOVE %s %s %s", sourceKey, destinationKey, value);
	if(NULL == r){
		freeReplyObject(r);
		return -2;
	}

	if(r->type != REDIS_REPLY_INTEGER ){
		freeReplyObject(r);
		return -3;
	}
	int ret = r->integer;
	freeReplyObject(r);
	return ret;
}

int RedisClient::smove(const char * sourceKey, const char * destinationKey, int value){
	if(m_c == NULL){
		TRACEERROR("smove, redisContext object is nil, sourceKey:%s, destinationKey:%s", sourceKey, destinationKey);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "SMOVE %s %s %d", sourceKey, destinationKey, value);
	if(NULL == r){
		freeReplyObject(r);
		return -2;
	}

	if(r->type != REDIS_REPLY_INTEGER ){
		freeReplyObject(r);
		return -3;
	}
	int ret = r->integer;
	freeReplyObject(r);
	return ret;
}

int RedisClient::srem(const char * key,  long long member){
	if(m_c == NULL){
		TRACEERROR("srem, redisContext object is nil, key:%s", key);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "SREM %s %lld", key, member);
	if(NULL == r){
		freeReplyObject(r);
		return -2;
	}

	if(r->type != REDIS_REPLY_INTEGER ){
		freeReplyObject(r);
		return -3;
	}
	int ret = r->integer;
	freeReplyObject(r);
	return ret;
}

int RedisClient::srem(const char * key, const char * member){
	if(m_c ==  NULL){
		TRACEERROR("srem, redisContext object is nil, key:%s", key);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "SREM %s %s", key, member);
	if(NULL == r){
		freeReplyObject(r);
		return -2;
	}

	if(r->type != REDIS_REPLY_INTEGER ){
		freeReplyObject(r);
		return -3;
	}
	int ret = r->integer;
	freeReplyObject(r);
	return ret;
}


int RedisClient::hincrBy(const char * key, const char * field, long long number)
{
	if (m_c == NULL) {
		TRACEERROR("hincrBy, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply* r = (redisReply*)redisCommand(m_c, "hincrby %s %s %lld", key, field, number);
	if (NULL == r) {
		freeReplyObject(r);
		return -2;
	}
	if (r->type != REDIS_REPLY_INTEGER) {
		freeReplyObject(r);
		return -3;
	}
	int ret = r->integer;
	freeReplyObject(r);
	return ret;
}

int RedisClient::smembers(const char * key, std::vector<std::string>& allMembers){
	if(m_c == NULL){
		TRACEERROR("del, redisContext object is nil, key:%s", key);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "SMEMBERS  %s", key);
	if(NULL == r){
		freeReplyObject(r);
		return -2;
	}

	if(r->type == REDIS_REPLY_NIL) {
		freeReplyObject(r);
		return -3;
	}

	if(r->type != REDIS_REPLY_ARRAY)
	{
		freeReplyObject(r);
		return -4;
	}

	int count = 0;
	for (unsigned int i = 0; i < r->elements; ++i)  {
		redisReply* childReply = r->element[i];
		if (childReply && childReply->type == REDIS_REPLY_STRING) {
			allMembers.push_back(std::string(childReply->str));
			count ++;
		} else {
			return -5;
		}
	}
	freeReplyObject(r);
	return count;
}

/**
 * DEL key [key ...]
 * 删除给定的一个或多个 key 。
 * 不存在的 key 会被忽略。
 */
int RedisClient::del(const char * key){
	if(m_c == NULL){
		TRACEERROR("del, redisContext object is nil, key:%s", key);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "DEL %s", key);
	if(NULL == r){
		freeReplyObject(r);
		return -2;
	}

	if(r->type != REDIS_REPLY_INTEGER ){
		freeReplyObject(r);
		return -3;
	}
	int ret = r->integer;
	freeReplyObject(r);
	return ret;
}

// 返回：>=0：成功；-1：错误
int RedisClient::execHExists(const char * key, const char * field) {
	if(m_c == NULL){
		TRACEERROR("execHExists, redisContext object is nil, key:%s", key);
		return -1;
	}

	redisReply* r = (redisReply*)redisCommand(m_c, "HEXISTS %s %s", key, field);

	if(NULL == r)
		return -1;

	if(r->type != REDIS_REPLY_INTEGER)
	{
		freeReplyObject(r);
		return -1;
	}

	int ret = r->integer;
	if (ret > 0) {
		ret = 0;
	} else {
		ret = -6;
	}
	freeReplyObject(r);
	return ret;
}

// 返回：>=0：成功；-1：错误
int RedisClient::execHGetAll(const char * key, std::map<std::string, std::string>& result) {
	if(m_c == NULL){
		TRACEERROR("execHGetAll, redisContext object is nil, key:%s", key);
		return -1;
	}
	redisReply *r = (redisReply *) redisCommand(m_c, "HGETALL %s", key);
	if (NULL == r) {
		return -2;
	}
	if(r->type == REDIS_REPLY_NIL) {
		freeReplyObject(r);
		return -3;
	}

	if(r->type != REDIS_REPLY_ARRAY) {
		freeReplyObject(r);
		return -4;
	}

	if(r->elements%2) {
		freeReplyObject(r);
		return -5;
	}

	for (unsigned int i = 0; i < r->elements; i+=2) {
		redisReply* field = r->element[i];
		redisReply* value = r->element[i+1];
		result.insert(std::make_pair(field->str, value->str));
	}

	freeReplyObject(r);
	return 0;
}

// 返回：>=0：成功；-1：错误
int RedisClient::execHMSet(const char * key, const std::map<std::string, std::string>& function) {
	if(m_c == NULL){
		TRACEERROR("execHMSet, redisContext object is nil, key:%s", key);
		return -1;
	}
	std::stringstream strFormat;
	strFormat << "HMSET " << key;
	for (std::map<std::string, std::string>::const_iterator it = function.begin(); it != function.end(); it++) {
		if(strcmp("", it->second.c_str()) == 0){
			strFormat << " " << it->first << " " << "\"\"";
		}else{
			strFormat << " " << it->first << " " << it->second;
		}
	}
	redisReply* r = (redisReply*)redisCommand(m_c, strFormat.str().c_str());
	if(NULL == r){
		return -2;
	}
	if(r->type != REDIS_REPLY_STATUS) {
		freeReplyObject(r);
		return -3;
	}

	if(!r->str || strcasecmp(r->str, "OK")) {
		freeReplyObject(r);
		return -4;
	}
	freeReplyObject(r);
	return 0;
}
