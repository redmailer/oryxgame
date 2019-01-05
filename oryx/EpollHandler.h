#ifndef _EPOLL_HANDLER_
#define _EPOLL_HANDLER_

#include "common.h"

#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/epoll.h> 
#include <errno.h>

#include <queue>
#include <unordered_map>
#include "thread.h"
#include "DeviceInfo.h"

const INT32 MAXEVENTS = 4096;                //每次循环最大事件数

typedef std::unordered_map<INT32, DEVICE_INFO *>	DEVICE_INFO_MAP;
typedef std::unordered_map<INT64, DEVICE_INFO *>	DEVICE_SESSION_INFO_MAP;

class EpollHandler : public Thread {

public:
	EpollHandler();

public:
	INT32 epfd;

private:
	struct epoll_event events_slot[MAXEVENTS];    //监听事件数组

	DEVICE_INFO_MAP m_MapDeviceInfo;
	DEVICE_SESSION_INFO_MAP m_MapDeviceSessionInfo;

	static void * epoll_thread_run(void * param);

public:
	virtual void dotask(tagThreadTaskNode & node);
	virtual void dotask_complete(tagThreadTaskNode & node);

public:
	bool Init();
	bool CreateEpoll();
	bool EpollRegister(INT32 fd, INT32 eventtype = EPOLLIN | EPOLLET);
	void EpollRemove(INT32 fd);
	INT32 EpollRun(INT32 maxEvent = MAXEVENTS);

	void AddListener(tagThreadTaskNode & node);
	void AddClientConn(tagThreadTaskNode & node);
	void DelClientConn(tagThreadTaskNode & node);
	void WritePacketToConn(tagThreadTaskNode & node);

	bool addDevice(INT32 fd, INT64 sessionID, DEVICE_TYPE deviceType, struct sockaddr_in& sock_addr);
	void removeDevice_BySession(INT64 sessionID);
	void removeDevice_ByFD(INT32 fd);

	void doEpollEvents(INT32 nfds);

	bool doAccept(INT32 fd);
	bool doRead(INT32 fd);
	bool doWrite(INT32 fd);

	inline INT32 GetDeviceNum() { return m_MapDeviceInfo.size(); }

public:
	inline DEVICE_INFO * GetDeviceInfo_ByFD(INT32 fd) {
		DEVICE_INFO_MAP::iterator it = m_MapDeviceInfo.find(fd);
		if (it == m_MapDeviceInfo.end()) {
			return NULL;
		}
		return it->second;
	}

	inline DEVICE_INFO * GetDeviceInfo_BySesion(INT64 sessionID) {
		DEVICE_SESSION_INFO_MAP::iterator it = m_MapDeviceSessionInfo.find(sessionID);
		if (it == m_MapDeviceSessionInfo.end()) {
			return NULL;
		}
		return it->second;
	}

	inline DEVICE_TYPE GetDeviceType_ByFD(INT32 fd) {
		DEVICE_INFO * pDevice = GetDeviceInfo_ByFD(fd);
		if (pDevice) {
			return pDevice->device_type;
		}
		return DEVICE_UNKNOWN;
	}

	inline DEVICE_TYPE GetDeviceType_BySession(INT64 sessionID) {
		DEVICE_INFO * pDevice = GetDeviceInfo_BySesion(sessionID);
		if (pDevice) {
			return pDevice->device_type;
		}
		return DEVICE_UNKNOWN;
	}



};


#endif
