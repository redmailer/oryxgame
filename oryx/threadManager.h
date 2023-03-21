#ifndef __THREAD_MANAGER__
#define __THREAD_MANAGER__

#include "common.h"
#include "thread.h"
#include <iostream>
#include <unordered_map>
#include "LogManager.h"

using namespace std;

typedef unordered_map<INT64, Thread *> THREAD_MAP;

class ThreadManager
{
    SINGLETON_DECLEAR(ThreadManager)

public:
    bool init();

    Thread *getLessPressureThread_Epoll(INT64 threadID = 0);

    bool addThread_Epoll(Thread *thread_t);

    INT32 update();

private:
    THREAD_MAP m_threadMap_Epoll;

public:
    template <typename _Ty>
    inline INT64 push_task(_Ty *pTask)
    {
        Thread *thread = getLessPressureThread_Epoll(pTask->m_thrdID);
        if (thread == NULL)
        {
            TRACEERROR("getLessPressureThread_Epoll failed ,threadID:%ld", pTask->m_thrdID);
            return -1;
        }
        thread->push_task_thread(pTask);
        return thread->getThreadID();
    }
};

#endif