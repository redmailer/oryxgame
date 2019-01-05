#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/time.h>
using namespace std;





void writefd(int threadid,int tryt) {
	time_t timeNow = time(0) ;
	
	FILE *fp = NULL;
	struct tm stTempTm;
	localtime_r(&timeNow, &stTempTm);

	char fileName[2048];
	sprintf(fileName, "%s-%04d%02d%02d-%02d.log", "game",
		stTempTm.tm_year + 1900,
		stTempTm.tm_mon + 1,
		stTempTm.tm_mday ,
		stTempTm.tm_hour);

	if (fp = fopen(fileName, "a+")) {
		printf("打开文件成功\n");
	}
	else {
		printf("打开文件成败\n");
		return;
	}

	fprintf(fp, "thread %d try %d \n", threadid, tryt);
	fclose(fp);
}


void * filetest(void * data) {

	int * param = (int *)data;
	printf("in thread %d\n", *param);

	int a_try = 0;
	while (a_try++ < 100) {
		writefd(*param, a_try);
	}

	return NULL;
}

int main1() {


	long int threadid[5];
	int params[5] = { 0,1,2,3,4 };
	pthread_attr_t attr;	//线程属性
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);	//线程分离，不需要关心返回值

	for (int i = 0; i < 5; i++) {
		pthread_create((pthread_t *)(threadid+i) , &attr, filetest, params + i);
	}

	pthread_attr_destroy(&attr);
	sleep(5);

	return 0;

}
