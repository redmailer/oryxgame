#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/time.h>


#include <google/protobuf/message.h>
#include "../proto/pb/messageID.pb.h"
#include "../oryx/packet.h"

using namespace std;

bool sendData(int fd, char * data, int size) {
	int sendLen = send(fd, data, size, 0);
	//cout << "sendData len " << sendLen << endl;
	return sendLen == size;
}

bool sendProto(INT32 fd, INT32 messageID, INT32 errCode, ::google::protobuf::Message * proto) {
	if (fd <= 0 /*|| proto == NULL || len <= 0*/) {
		//TRACEERROR("sendProtoToSession failed,fd :%d", fd);
		cout << "sendProtoToSession failed,fd :" << fd << endl;
		return false;
	}

	Packet* pPack = Packet::NewPacket(messageID, errCode);
	if (pPack == NULL) {
		cout << "pPack == NULL" << endl;
		return false;
	}

	if (proto != NULL) {
		INT32 sendLen = proto->ByteSize();
		char buff[sendLen];

		if (sendLen <= 0 || proto->SerializeToArray(buff, sendLen)) {
			cout << "proto->SerializeToArray failed,fd :" << fd << " messageID:" << messageID << " errCode:" << errCode << endl;
			ORYX_DEL(pPack);
			return false;
		}
		pPack->Append(buff, sendLen);

	}

	bool result = sendData(fd, pPack->data, pPack->message_len);
	ORYX_DEL(pPack);
	return result;
}

const char * global_addr;
int global_port;

void * mutiClient(void * data) {
	int * pData = (int *)data;
	printf("pthread create at %d\n", *pData);
	int sock_cli = -1;
	sock_cli = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	int port = global_port;
	servaddr.sin_port = htons(port);  ///服务器端口
	servaddr.sin_addr.s_addr = inet_addr(global_addr);  ///服务器ip

	if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		printf("connect err\n");
		return 0;
	}

	for (int i = 0; i < 1000; i++) {
		bool result = sendProto(sock_cli, Proto::MSGID_HEART_BEAT, 0, NULL);
		cout << "sendProto at thread " << *pData << "times:" << i << " result " <<  result << endl;
		usleep(1000);
	}
	close(sock_cli);


	return NULL;
}


int main(int argc ,char * argv[]){
	
        if(argc != 3){
				printf("useage:XXX ip port \n");
                return 0;
        }

		global_addr = argv[1];
		global_port = atoi(argv[2]);

		long int threadid[5];
		int params[5] = { 0,1,2,3,4 };
		pthread_attr_t attr;	//线程属性
		pthread_attr_init(&attr);
		//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);	//线程分离，不需要关心返回值

		for (int i = 0; i < 5; i++) {
			pthread_create((pthread_t *)(threadid + i), &attr, mutiClient, params + i);
		}

		pthread_attr_destroy(&attr);

		for (int i = 0; i < 5; i++) {
			pthread_join(threadid[i], NULL);
		}

        
        return 0;
}
