#ifndef __DEVICE_INFO__
#define __DEVICE_INFO__

#include "common.h"
#include <iostream>
#include <arpa/inet.h>
#include "LogManager.h"
using namespace std;

enum DEVICE_TYPE {
	DEVICE_UNKNOWN = 0,				//δ֪������
	DEVICE_SERVER_INTERNAL = 1,		//�ڲ�������
	DEVICE_SERVER_EXTERNAL = 2,		//�ⲿ������
	DEVICE_CLIENT_INTERNAL = 3,		//�ڲ������豸
	DEVICE_CLIENT_EXTERNAL = 4,		//�ⲿ�����豸
};

enum PROTO_TYPE {
	PROTO_NONE = 0,
	PROTO_ORYX = 1,
	PROTO_WEBSOCKET = 2,
};

const INT32 MAX_RECV_BUFFER_LEN = 10240;
const INT32 MAX_SEND_BUFFER_LEN = 10240;



struct DEVICE_INFO {
	INT32 fd;
	INT64 session_id;
	DEVICE_TYPE  device_type;			//�豸����
	char recv_buffer[MAX_RECV_BUFFER_LEN];
	INT32 recv_begin;
	INT32 recv_end;
	UINT32 recv_idx;
	char send_buffer[MAX_SEND_BUFFER_LEN];
	INT32 send_begin;
	INT32 send_end;
	UINT32 send_idx;
	PROTO_TYPE  proto_type;
	struct sockaddr_in sock_addr;

	DEVICE_INFO(DEVICE_TYPE deviceType = DEVICE_UNKNOWN) {
		device_type = deviceType;
		recv_begin = 0;
		recv_end = 0;
		recv_idx = 0;
		send_begin = 0;
		send_end = 0;
		send_idx = 0;
		proto_type = PROTO_NONE;
	}

	inline void clear_Recv_buff() {
		if (this->recv_begin > 0) {
			if (this->recv_end == this->recv_begin) {
				this->recv_end = 0;
				this->recv_begin = 0;
			}
			else if (this->recv_end > this->recv_begin) {
				memcpy(this->recv_buffer, this->recv_buffer + this->recv_begin, this->recv_end - this->recv_begin);
				this->recv_end -= this->recv_begin;
				this->recv_begin = 0;
			}
			else {
				TRACEEPOLL(LOG_LEVEL_ERROR,"clear_Recv_buff wrong session:%ld ,recv_end :%d recv_begin:%d", session_id, recv_end, recv_begin);
				this->recv_end = 0;
				this->recv_begin = 0;
			}
		}
	}


	inline void clear_Send_buff() {
		if (this->send_begin > 0) {
			if (this->send_end == this->send_begin) {
				this->send_end = 0;
				this->send_begin = 0;
			}
			else if (this->send_end > this->send_begin) {
				memcpy(this->send_buffer, this->send_buffer + this->send_begin, this->send_end - this->send_begin);
				this->send_end -= this->send_begin;
				this->send_begin = 0;
			}
			else {
				TRACEEPOLL(LOG_LEVEL_ERROR, "clear_Send_buff wrong session:%ld ,recv_end :%d recv_begin:%d", session_id, send_end, send_begin);
				this->send_end = 0;
				this->send_begin = 0;
			}
		}
	}

	inline INT32 write(const char * data, INT32 size) {
		if (data == NULL || size == 0) {
			TRACEEPOLL(LOG_LEVEL_WARN, "session %ld write failed,(data == NULL || size == 0)", session_id);

			return 0;
		}
		clear_Send_buff();
		
		switch(this->proto_type){
			case PROTO_WEBSOCKET:{
				
			}
			break;

			default: {
				if (MAX_SEND_BUFFER_LEN - this->send_end < size ) {
					TRACEEPOLL(LOG_LEVEL_WARN, "session %ld write failed,data too long", session_id);
					return 0;
				}
				memcpy(send_buffer + send_end, data, size);
				send_end += size;
			}
			break;
		}
		
		return size;
	}

	static inline DEVICE_TYPE getClientDeviceType(DEVICE_TYPE deviceType) {
		switch (deviceType) {
		case DEVICE_UNKNOWN:
			return DEVICE_UNKNOWN;
		case DEVICE_SERVER_INTERNAL:
			return DEVICE_CLIENT_INTERNAL;
		case DEVICE_SERVER_EXTERNAL:
			return DEVICE_CLIENT_EXTERNAL;
		case DEVICE_CLIENT_INTERNAL:
			return DEVICE_UNKNOWN;
		case DEVICE_CLIENT_EXTERNAL:
			return DEVICE_UNKNOWN;
		default:
			return DEVICE_UNKNOWN;
		}
	}

};


#endif