#ifndef __TASK__
#define __TASK__

//task的内存管理：谁产生，谁回收

#include "../oryx/common.h"

enum TASK_TYPE {

	TASK_TYPE_NONE = 0,
	TASK_TYPE_LISTENER = 1,				//逻辑线程 通知 epoll线程 ，新的监听
	TASK_TYPE_CLIENTCONN = 2,			//epoll线程 通知 逻辑线程 ，conn连接
	TASK_TYPE_CLIENTCONN_INSERT = 3,	//逻辑线程 通知 epoll线程 ，插入conn
	TASK_TYPE_CLIENTCONN_DELETE = 4,	//逻辑线程 通知 epoll线程 ，删除conn
	TASK_TYPE_CLIENTCONN_ONCLOSE = 5,	//epoll线程 通知 逻辑线程 ，conn断开
	TASK_TYPE_CLIENTCONN_PACKET = 6,	//接收
	TASK_TYPE_SEND_PACKET = 7,			//发送

};

template<class _Ty, TASK_TYPE TASKTYPE >
struct Task
{
	static void OnExcute(void *pTask)
	{
		_Ty *pThis = (_Ty *)pTask;
		pThis->on_thread_call();
	}

	static void OnComplete(void *pTask)
	{
		_Ty *pThis = (_Ty *)pTask;
		pThis->on_main_call();
		pThis->on_free();
	}

	static INT64 getThreadID(void *pTask)
	{
		_Ty *pThis = (_Ty *)pTask;
		return pThis->m_thrdID;
	}

	static TASK_TYPE getTaskType(void *pTask)
	{
		_Ty *pThis = (_Ty *)pTask;
		return pThis->m_taskType;
	}

	Task(INT64 thrdID) {
		this->m_thrdID = thrdID;
		this->m_taskType = TASKTYPE;
	}

	INT64 m_thrdID;
	TASK_TYPE m_taskType;

	void on_thread_call();
	void on_main_call();
	void on_free();
};

typedef void (task_node_funPtr) (void *);
typedef INT64 (task_node_GetThreadID) (void *);
typedef TASK_TYPE (task_node_GetTaskType) (void *);

struct tagThreadTaskNode
{
	void *pTask;
	task_node_funPtr *pExcute;			//执行函数
	task_node_funPtr *pCompleted;		//完成函数
	task_node_GetTaskType *pGetTaskType;
	task_node_GetThreadID *pGetThreadID;

};

#endif