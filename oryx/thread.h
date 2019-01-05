#ifndef __THREAD__
#define __THREAD__

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include "task.h"
#include "spsc_queue.h"

#define MAX_SPSC_QUEUE_SIZE			40960
#define MAX_HANDLER_SIZE_PERFRAME   1024

typedef void *(*_thread_fun_)(void *);

class Thread  {
public:

	Thread(_thread_fun_ fun, void * param = NULL);
	INT64 thread_id;			//线程id
	void * param;			//线程启动参数

	bool run();

private:
	_thread_fun_ thread_fun;

	INT32 m_pressure;	//泛压力值

public:
	inline void resetThreadFun(_thread_fun_ fun) { this->thread_fun = fun; };
	inline INT64 getThreadID() { return this->thread_id; };
	inline void setParam(void * data) { this->param = data; }

	inline void addPressure(INT32 addNum) { m_pressure += addNum; }
	inline INT32 getPressure() { return m_pressure; }

	INT32 update_thread();	//自身线程调用
	INT32 update_main();	//逻辑线程调用

	INT32 handlerTask_main();				//逻辑线程调用
	INT32 handlerTask_main_complete();		//逻辑线程调用

	INT32 handlerTask_thread();				//自身线程调用
	INT32 handlerTask_thread_complete();	//自身线程调用

	virtual void dotask(tagThreadTaskNode & node) ;
	virtual void dotask_complete(tagThreadTaskNode & node) ;

public:
	template<typename _Ty>
	inline void push_task_thread(_Ty *pTask){
		tagThreadTaskNode node;
		node.pExcute = _Ty::OnExcute;
		node.pCompleted = _Ty::OnComplete;
		node.pGetTaskType = _Ty::getTaskType;
		node.pGetThreadID = _Ty::getThreadID;
		node.pTask = (void*)pTask;

		m_queue_thread.push(node);
	}

	template<typename _Ty>
	inline void push_task_main(_Ty *pTask) {
		tagThreadTaskNode node;
		node.pExcute = _Ty::OnExcute;
		node.pCompleted = _Ty::OnComplete;
		node.pGetTaskType = _Ty::getTaskType;
		node.pGetThreadID = _Ty::getThreadID;
		node.pTask = (void*)pTask;

		m_queue_main.push(node);
	}

protected:

	spsc_queue<tagThreadTaskNode, MAX_SPSC_QUEUE_SIZE> m_queue_thread;				//投入(别的线程压栈，自身线程取)
	spsc_queue<tagThreadTaskNode, MAX_SPSC_QUEUE_SIZE> m_queue_thread_complete;		//投出-回递（自身线程压栈，别的线程取）

	spsc_queue<tagThreadTaskNode, MAX_SPSC_QUEUE_SIZE> m_queue_main;				//投出（自身线程压栈，别的线程取）
	spsc_queue<tagThreadTaskNode, MAX_SPSC_QUEUE_SIZE> m_queue_main_complete;		//投入-回递(别的线程压栈，自身线程取)

	tagThreadTaskNode m_obj_thread;
	tagThreadTaskNode m_obj_main;

};



#endif
