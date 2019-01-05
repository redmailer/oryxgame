#include "threadManager.h"
#include "EpollHandler.h"

SINGLETON_DEFINE(ThreadManager)

ThreadManager::ThreadManager() {}

bool ThreadManager::init()
{
	for (INT32 i = 0; i < 4; i++) {
		Thread * thread_t = new EpollHandler();
		thread_t->run();
		addThread_Epoll(thread_t);
	}
	return true;
}

Thread * ThreadManager::getLessPressureThread_Epoll(INT64 threadID)
{
	Thread * nowThread = NULL;

	if (threadID != 0) {
		THREAD_MAP::iterator it = m_threadMap_Epoll.find(threadID);
		if (it != m_threadMap_Epoll.end()) {
			nowThread = it->second;
		}
		return nowThread;
	}

	THREAD_MAP::iterator it = m_threadMap_Epoll.begin();
	while (it != m_threadMap_Epoll.end()) {
		if (nowThread == NULL) {
			nowThread = it->second;
		}
		else if(it->second != NULL && nowThread->getPressure() > it->second->getPressure()){
			nowThread = it->second;
		}
		it++;
	}
	return nowThread;
}

bool ThreadManager::addThread_Epoll(Thread * thread_t)
{
	if (thread_t == NULL) {
		return false;
	}
	THREAD_MAP::iterator it = m_threadMap_Epoll.find(thread_t->getThreadID());
	if (it != m_threadMap_Epoll.end()) {
		return false;
	}
	m_threadMap_Epoll[thread_t->getThreadID()] = thread_t;
	return true;
	
}

INT32 ThreadManager::update()
{
	INT32 handlerNums = 0;
	THREAD_MAP::iterator it = m_threadMap_Epoll.begin();
	while (it != m_threadMap_Epoll.end()) {
		if (it->second != NULL) {
			handlerNums += it->second->update_main();
		}
		it++;
	}
	return handlerNums;
}

