#ifndef __TASK_SEND_PACKET__
#define __TASK_SEND_PACKET__

#include <iostream>
#include "../oryx/common.h"
#include "../oryx/task.h"
#include "../oryx/DeviceInfo.h"

using namespace std;

class task_sendPacket : public Task<task_sendPacket, TASK_TYPE_SEND_PACKET>
{

public:
	task_sendPacket(
		INT64 threadID,
		INT64 sessionID) :Task(threadID) {

		this->session_id = sessionID;
		data_size = 0;
		data_packet = NULL;
	};

	inline bool InitData(void * data, INT32 size) {
		if (data == NULL || size <= 0 || size > MAX_SEND_BUFFER_LEN) {
			return false;
		}

		data_size = size;
		data_packet = new BYTE[size];
		memcpy(data_packet, data, size);
		data_packet[size] = 0;

		return true;
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