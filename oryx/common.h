#ifndef __COMMON__
#define __COMMON__

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h> 
#include <string.h>
#include <time.h>
#include <typeinfo>
#include <stdio.h>

#define ORYX_NEW(_type,...) new _type(__VA_ARGS__)
#define ORYX_DEL(_param) if(_param != NULL){delete _param;}

typedef short int16, INT16;
typedef unsigned short uint16, UINT16;

typedef int int32, INT32;
typedef unsigned int uint32, UINT32;

typedef long long int64, INT64;
typedef unsigned long long uint64, UINT64;

typedef char byte, BYTE;

typedef bool real, REAL;

#define SINGLETON_INIT(_type,...)  _type::initManager(new _type)->init(__VA_ARGS__)

#define SINGLETON_DECLEAR(classname)  \
	public : \
		classname(); \
	private: \
		static classname * m_singleton; \
	public: \
		static classname * getInstance(){ \
			if(m_singleton == NULL) {	\
				m_singleton = new classname();	\
			}	\
			return m_singleton;	\
		}	\
		static classname * initManager(classname * _type) {	\
			if(m_singleton == NULL) {	\
				m_singleton = _type;	\
			}	\
			else{	\
				printf("initManager repeated:%s",typeid(classname).name());	\
			}	\
			return m_singleton;	\
		}

#define SINGLETON_DEFINE(classname) \
	classname * classname::m_singleton;


//#define bcopy(src,dst,size)	memcpy(dst,src,size);

#define BZERO(Dest,Count)	memset(Dest, 0, Count);


inline void setnonblocking(int sock) {
	int opts;
	opts = fcntl(sock, F_GETFL);
	if (opts < 0)
	{
		//perror("fcntl(sock,GETFL)");
		printf("fcntl(sock,GETFL) \n");
		return;
	}
	opts = opts | O_NONBLOCK;
	if (fcntl(sock, F_SETFL, opts) < 0)
	{
		printf("fcntl(sock,SETFL,opts) \n");
		return;
	}
};

inline void setreuseaddr(int sock) {
	int opt;
	opt = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(&opt)) < 0)
	{
		//perror("setsockopt");
		printf("setreuseaddr failed \n");
		return;
	}
};


inline void printMemery(byte* data, INT32 len) {

	printf("printMemery len:%d\n", len);
	if (data == NULL || len <= 0) {
		return;
	}
	for (int i = 0; i < len; i++) {
		if (i % 8 == 0) {
			printf("\n");
		}
		printf("%8d", data[i]);
	}
	printf("\n");
};

inline INT64 GetCourrentTime() {
	return (INT64)time(NULL);
}

#endif