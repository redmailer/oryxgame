#include "TimerControl.h"


TimerControl::TimerControl()
{
	gettimeofday(&m_lastRunTime, NULL);
	BZERO(&m_nowTime, sizeof(m_nowTime));
}

INT32 TimerControl::update()
{
	m_lastRunTime = m_nowTime;
	gettimeofday(&m_nowTime, NULL);
	INT64 timeNow_MilliSecond = m_nowTime.tv_sec * 1000L + m_nowTime.tv_usec / 1000L;
	INT32 timerNums = 0;

	for (UINT32 i = 0; i < m_vecTimerFun.size(); i++) {
		if (m_vecTimerFun[i]->timeval_fun == NULL) {
			continue;
		}

		if (timeNow_MilliSecond >= m_vecTimerFun[i]->lastRunTime + m_vecTimerFun[i]->interval) {

			m_vecTimerFun[i]->timeval_fun(timeNow_MilliSecond);

			INT64 addVal = timeNow_MilliSecond - m_vecTimerFun[i]->lastRunTime;
			addVal -= addVal % m_vecTimerFun[i]->interval;

			m_vecTimerFun[i]->lastRunTime += addVal;
			timerNums++;
		}
		else if (timeNow_MilliSecond < m_vecTimerFun[i]->lastRunTime) {
			m_vecTimerFun[i]->lastRunTime = 0;
		}

	}

	return timerNums;
}

void TimerControl::register_timerFun(_TIMERVAL_FUN_ _fun, INT64 interval)
{
	TimerFunInfo * timerFunInfo = new TimerFunInfo;
	timerFunInfo->timeval_fun = _fun;
	timerFunInfo->interval = interval;
	timerFunInfo->lastRunTime = 0;

	m_vecTimerFun.push_back(timerFunInfo);
}