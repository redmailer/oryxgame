#ifndef __TASK_LISTENER__
#define __TASK_LISTENER__

#include <iostream>
#include "../oryx/common.h"
#include "../oryx/task.h"
#include "../oryx/DeviceInfo.h"
using namespace std;
class task_listener : public Task<task_listener,TASK_TYPE_LISTENER>
{

public:
	task_listener(
		INT32 listenFD, 
		DEVICE_TYPE deviceType,
		struct sockaddr_in& sock_addr) :Task(0) {
		this->listenFD = listenFD;
		device_type = deviceType;
		memcpy(&this->sock_addr, &sock_addr, sizeof(this->sock_addr));
	};
	INT32 listenFD;
	DEVICE_TYPE device_type;
	struct sockaddr_in sock_addr;

public:
	void on_thread_call() ;
	void on_main_call() ;
	void on_free() ;

};


#endif
