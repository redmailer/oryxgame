#ifndef __THREAD__
#define __THREAD__

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include "task.h"
#include "spsc_queue.h"

#define MAX_SPSC_QUEUE_SIZE 40960
#define MAX_HANDLER_SIZE_PERFRAME 1024

typedef void *(*_thread_fun_)(void *);

class Thread
{
public:
    Thread(_thread_fun_ fun, void *param = NULL);
    INT64 thread_id; // �߳�id
    void *param;     // �߳���������

    bool run();

private:
    _thread_fun_ thread_fun;

    INT32 m_pressure; // ��ѹ��ֵ

public:
    inline void resetThreadFun(_thread_fun_ fun) { this->thread_fun = fun; };
    inline INT64 getThreadID() { return this->thread_id; };
    inline void setParam(void *data) { this->param = data; }

    inline void addPressure(INT32 addNum) { m_pressure += addNum; }
    inline INT32 getPressure() { return m_pressure; }

    INT32 update_thread(); // �����̵߳���
    INT32 update_main();   // �߼��̵߳���

    INT32 handlerTask_main();          // �߼��̵߳���
    INT32 handlerTask_main_complete(); // �߼��̵߳���

    INT32 handlerTask_thread();          // �����̵߳���
    INT32 handlerTask_thread_complete(); // �����̵߳���

    virtual void dotask(tagThreadTaskNode &node);
    virtual void dotask_complete(tagThreadTaskNode &node);

public:
    template <typename _Ty>
    inline void push_task_thread(_Ty *pTask)
    {
        tagThreadTaskNode node;
        node.pExcute = _Ty::OnExcute;
        node.pCompleted = _Ty::OnComplete;
        node.pGetTaskType = _Ty::getTaskType;
        node.pGetThreadID = _Ty::getThreadID;
        node.pTask = (void *)pTask;

        m_queue_thread.push(node);
    }

    template <typename _Ty>
    inline void push_task_main(_Ty *pTask)
    {
        tagThreadTaskNode node;
        node.pExcute = _Ty::OnExcute;
        node.pCompleted = _Ty::OnComplete;
        node.pGetTaskType = _Ty::getTaskType;
        node.pGetThreadID = _Ty::getThreadID;
        node.pTask = (void *)pTask;

        m_queue_main.push(node);
    }

protected:
    spsc_queue<tagThreadTaskNode, MAX_SPSC_QUEUE_SIZE> m_queue_thread;          // Ͷ��(����߳�ѹջ�������߳�ȡ)
    spsc_queue<tagThreadTaskNode, MAX_SPSC_QUEUE_SIZE> m_queue_thread_complete; // Ͷ��-�صݣ������߳�ѹջ������߳�ȡ��

    spsc_queue<tagThreadTaskNode, MAX_SPSC_QUEUE_SIZE> m_queue_main;          // Ͷ���������߳�ѹջ������߳�ȡ��
    spsc_queue<tagThreadTaskNode, MAX_SPSC_QUEUE_SIZE> m_queue_main_complete; // Ͷ��-�ص�(����߳�ѹջ�������߳�ȡ)

    tagThreadTaskNode m_obj_thread;
    tagThreadTaskNode m_obj_main;
};

#endif
