#ifndef __TASK_CLIENT_CONN__
#define __TASK_CLIENT_CONN__

#include <iostream>
#include "../oryx/common.h"
#include "../oryx/task.h"
#include "../oryx/DeviceInfo.h"
using namespace std;
class task_clientConn : public Task<task_clientConn, TASK_TYPE_CLIENTCONN>
{

public:
	task_clientConn(
		INT64 threadID,
		INT32 clientFD,
		DEVICE_TYPE deviceType,
		struct sockaddr_in & sock_addr) :Task(threadID) {

		this->clientFD = clientFD;
		device_type = deviceType;
		memcpy(&this->sock_addr, &sock_addr, sizeof(this->sock_addr));
	};
	INT32 clientFD;
	DEVICE_TYPE device_type;
	struct sockaddr_in sock_addr;

public:
	void on_thread_call();
	void on_main_call();
	void on_free() ;

};

class task_clientConnInsert : public Task<task_clientConnInsert, TASK_TYPE_CLIENTCONN_INSERT>
{

public:
	task_clientConnInsert(
		INT32 clientFD,
		DEVICE_TYPE deviceType,
		struct sockaddr_in & sock_addr) :Task(0) {

		this->clientFD = clientFD;
		device_type = deviceType;
		session_id = 0;
		memcpy(&this->sock_addr, &sock_addr, sizeof(this->sock_addr));
	};
	INT32 clientFD;
	DEVICE_TYPE device_type;
	INT64 session_id;
	struct sockaddr_in sock_addr;

public:
	void on_thread_call();
	void on_main_call();
	void on_free();

};

//
class task_clientConnOnClose : public Task<task_clientConnOnClose, TASK_TYPE_CLIENTCONN_ONCLOSE>
{

public:
	task_clientConnOnClose(
		INT64 threadID,
		INT64 sessionID) :Task(threadID) {

		session_id = sessionID;
	};

	INT64 session_id;

public:
	void on_thread_call();
	void on_main_call();
	void on_free();
};


class task_clientConnDelete : public Task<task_clientConnDelete, TASK_TYPE_CLIENTCONN_DELETE>
{

public:
	task_clientConnDelete(
		INT64 threadID,
		INT64 sessionID) :Task(threadID) {

		session_id = sessionID;
	};

	INT64 session_id;

public:
	void on_thread_call();
	void on_main_call();
	void on_free();
};


#endif
