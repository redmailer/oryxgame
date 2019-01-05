#ifndef __TASK_CLIENT_PACKET__
#define __TASK_CLIENT_PACKET__

#include <iostream>
#include "../oryx/common.h"
#include "../oryx/task.h"
using namespace std;

class task_clientConnPacket : public Task<task_clientConnPacket, TASK_TYPE_CLIENTCONN_PACKET>
{

public:
	task_clientConnPacket(
		INT64 threadID,
		INT64 sessionID) :Task(threadID) {

		this->session_id = sessionID;
		data_size = 0;
		data_packet = 0;
	};
	
	inline void InitData(void * data,INT32 size){
		if (data != NULL && size > 0) {
			data_size = size;
			data_packet = new BYTE[size + 1];
			memcpy(data_packet, data, size);
			data_packet[size] = 0;
		}
	}

	INT64 session_id;
	INT32 data_size;
	BYTE * data_packet;


public:
	void on_thread_call();
	void on_main_call();
	void on_free();

};



#endif