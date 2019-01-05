#include "thread.h"
#include <iostream>
#include <stdio.h>
#include <thread>
#include "LogManager.h"
using namespace std;

Thread::Thread(_thread_fun_ fun, void * param)
{
	this->thread_id = -1;
	this->thread_fun = fun;
	this->param = param;
}

bool Thread::run()
{
	if (this->thread_fun == NULL) {
		return false;
	}

	pthread_attr_t attr;	//线程属性
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);	//线程分离，不需要关心返回值

	pthread_create((pthread_t *)&this->thread_id, &attr, this->thread_fun, this->param);

	pthread_attr_destroy(&attr);

	TRACEINFO("create  thread %ld", thread_id);

	return true;

}

INT32 Thread::update_main()
{
	INT32 handlerTaskNum = 0;
	handlerTaskNum += this->handlerTask_main_complete();
	handlerTaskNum += this->handlerTask_main();

	this->m_queue_thread_complete.checkCache();
	this->m_queue_thread.checkCache();

	return handlerTaskNum;
}

INT32 Thread::update_thread()
{
	INT32 handlerTaskNum = 0;
	handlerTaskNum += this->handlerTask_thread_complete();
	handlerTaskNum += this->handlerTask_thread();
	
	this->m_queue_main_complete.checkCache();
	this->m_queue_main.checkCache();

	return handlerTaskNum;
}

INT32 Thread::handlerTask_main()
{
	INT32 handlerTaskNum = 0;

	while (handlerTaskNum < MAX_HANDLER_SIZE_PERFRAME && m_queue_main.pop(m_obj_main)) {
		dotask(m_obj_main);
		m_queue_thread_complete.push(m_obj_main);
		handlerTaskNum++;
	}
	return handlerTaskNum;
}

INT32 Thread::handlerTask_main_complete()
{
	INT32 handlerTaskNum = 0;
	while (m_queue_main_complete.pop(m_obj_main)) {
		dotask_complete(m_obj_main);
		handlerTaskNum++;
	}
	return handlerTaskNum;
}

INT32 Thread::handlerTask_thread()
{
	INT32 handlerTaskNum = 0;
	while (handlerTaskNum < MAX_HANDLER_SIZE_PERFRAME && m_queue_thread.pop(m_obj_thread)) {
		dotask(m_obj_thread);
		m_queue_main_complete.push(m_obj_thread);
		handlerTaskNum++;
	}
	return handlerTaskNum;
}

INT32 Thread::handlerTask_thread_complete()
{
	INT32 handlerTaskNum = 0;
	while (m_queue_thread_complete.pop(m_obj_thread)) {
		dotask_complete(m_obj_thread);
		handlerTaskNum++;
	}
	return handlerTaskNum;
}

void Thread::dotask(tagThreadTaskNode & node)
{
	node.pExcute(node.pTask);
}

void Thread::dotask_complete(tagThreadTaskNode & node)
{
	node.pCompleted(node.pTask);
}

