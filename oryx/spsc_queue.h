#ifndef __SPSC_QUEUE__
#define __SPSC_QUEUE__


#include "common.h"
#include <vector>
#include <boost/lockfree/spsc_queue.hpp>


//spsc_queue
//单生产者单消费者 队列
// pop/push 只能在单一线程中调用 ,pop和push可以是同一线程

//为了同步安全和效率，将无锁队列的长度固定，并增加缓存机制。缓存的清理必须在 操作push的线程中使用

template <class _T,INT32 size>
struct spsc_queue {
public:
	inline void push(_T data) {
		if (m_cache.size() > 0) {
			m_cache.push_back(data);
		}
		else {
			if (m_queue.push(data) == false) {
				m_cache.push_back(data);
			}
		}
		checkCache();
	}

	inline bool pop(_T & data) {
		return m_queue.pop(data);
	}

	inline INT32 checkCache() {
		INT32 cacheNum = 0;
		while (m_cache.size() > 0) {
			if (m_queue.push(m_cache[0])) {
				m_cache.erase(m_cache.begin());
				cacheNum++;
				continue;
			}
			break;
		}

		if (m_cache.size() == 0 && m_cache.capacity() > 0) {
			std::vector<_T>().swap(m_cache);
		}

		return cacheNum;

	}
private:
	std::vector<_T>  m_cache;
	boost::lockfree::spsc_queue<_T, boost::lockfree::capacity<size> > m_queue;
};


#endif