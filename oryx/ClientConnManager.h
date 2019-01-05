#ifndef __CLIENT_CONN_MANAGER__
#define __CLIENT_CONN_MANAGER__

#include "common.h"
#include <unordered_map>
#include <iostream>
#include "../task/task_clientConn.h"
#include "threadManager.h"

#define SESSION_NEW(a,b)		new SessionConn(a,b)
#define SESSION_DEL(params)		if(params != NULL){ delete params ; }

struct SessionConn {

	SessionConn(INT64 threadID,INT64 sessionID, DEVICE_TYPE deviceType, struct sockaddr_in& addr) {
		session_id = sessionID;
		thread_id = threadID;
		player_id = 0;
		device_type = deviceType;
		memcpy(&sock_addr, &addr, sizeof(sock_addr));
	}
	INT64 thread_id;
	INT64 session_id;
	INT64 player_id;
	DEVICE_TYPE device_type;
	struct sockaddr_in sock_addr;
};

typedef std::unordered_map<INT64, SessionConn *> CONN_INFO_MAP;

class ClientConnManager {

	SINGLETON_DECLEAR(ClientConnManager)

public:
	virtual ~ClientConnManager() {};

	virtual bool init();

	inline SessionConn * getSession_BySessionID(INT64 sessionID) {
		CONN_INFO_MAP::iterator it = m_MapSessionConnInfo.find(sessionID);
		if (it != m_MapSessionConnInfo.end()) {
			return it->second;
		}
		return NULL;
	};

	bool addNewConn(INT64 threadID, INT64 sessionID, DEVICE_TYPE deviceType,struct sockaddr_in& addr);

	void closeConn(INT64 sessionID);

	virtual void onSessionBegin(SessionConn * pConn);
	virtual void onSessionClose(SessionConn * pConn);

	void removeSessionConn(INT64 sessionID);

	INT64 getRandNewSession();

	bool sendMsgToConn(INT64 sessionID, void * data, INT32 len);
	

private:
	CONN_INFO_MAP m_MapSessionConnInfo;

	INT64 m_iAutoIncriSessionID;

};


#endif