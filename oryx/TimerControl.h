#ifndef __TIMER_CONTROL__
#define __TIMER_CONTROL__

#include<iostream>
#include <sys/time.h>
#include "common.h"
#include<vector>

typedef struct timeval TIMEVAL;

typedef void (*_TIMERVAL_FUN_)(INT64);

struct TimerFunInfo {
	_TIMERVAL_FUN_  timeval_fun;
	INT64  interval;
	INT64 lastRunTime;
};


class TimerControl {

public:
	TimerControl();
	~TimerControl() {};

	virtual INT32 update();

	void register_timerFun(_TIMERVAL_FUN_ _fun, INT64 interval);


private:
	TIMEVAL m_lastRunTime;
	TIMEVAL m_nowTime;

	std::vector<TimerFunInfo *>  m_vecTimerFun;

};


#endif