#ifndef __TASK__
#define __TASK__

// task���ڴ������˭������˭����

#include "../oryx/common.h"

enum TASK_TYPE
{

    TASK_TYPE_NONE = 0,
    TASK_TYPE_LISTENER = 1,           // �߼��߳� ֪ͨ epoll�߳� ���µļ���
    TASK_TYPE_CLIENTCONN = 2,         // epoll�߳� ֪ͨ �߼��߳� ��conn����
    TASK_TYPE_CLIENTCONN_INSERT = 3,  // �߼��߳� ֪ͨ epoll�߳� ������conn
    TASK_TYPE_CLIENTCONN_DELETE = 4,  // �߼��߳� ֪ͨ epoll�߳� ��ɾ��conn
    TASK_TYPE_CLIENTCONN_ONCLOSE = 5, // epoll�߳� ֪ͨ �߼��߳� ��conn�Ͽ�
    TASK_TYPE_CLIENTCONN_PACKET = 6,  // ����
    TASK_TYPE_SEND_PACKET = 7,        // ����

};

template <class _Ty, TASK_TYPE TASKTYPE>
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

    Task(INT64 thrdID)
    {
        this->m_thrdID = thrdID;
        this->m_taskType = TASKTYPE;
    }

    INT64 m_thrdID;
    TASK_TYPE m_taskType;

    void on_thread_call();
    void on_main_call();
    void on_free();
};

typedef void(task_node_funPtr)(void *);
typedef INT64(task_node_GetThreadID)(void *);
typedef TASK_TYPE(task_node_GetTaskType)(void *);

struct tagThreadTaskNode
{
    void *pTask;
    task_node_funPtr *pExcute;    // ִ�к���
    task_node_funPtr *pCompleted; // ��ɺ���
    task_node_GetTaskType *pGetTaskType;
    task_node_GetThreadID *pGetThreadID;
};

#endif