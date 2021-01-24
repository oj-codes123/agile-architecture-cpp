
#include "Timer.h"
#include "Logger.h"
#include <sys/time.h>

namespace agile
{

Timer::~Timer()
{
	for( auto& it : m_timersCache )
	{
		delete it;
	}
	m_timersCache.clear();
	
	for( auto& it : m_timers )
	{
		for(auto& it2 : it.second)
		{
			delete it2;
		}
	}
	m_timers.clear();
}

uint64_t Timer::GetCurMSec()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t) ( tv.tv_sec * 1000 + tv.tv_usec / 1000 ); 
}

uint64_t Timer::GetCurSec()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t) tv.tv_sec;
}

Timer::TimerObj* Timer::GetTimerObj(uint64_t msecVal, TimerCallback callbackFunc)
{
	if(m_timersCache.empty())
	{
		Timer::TimerObj* obj = new Timer::TimerObj();
		obj->val = msecVal;
		obj->func = callbackFunc;
		return obj;
	}
	else
	{
		Timer::TimerObj* obj = m_timersCache.front();
		obj->val = msecVal;
		obj->func = callbackFunc;
		m_timersCache.pop_front();
		return obj;
	}
}

void Timer::AddSecTimer(uint32_t secVal, TimerCallback callbackFunc)
{
	uint64_t triggertMsecVal = ( GetCurSec() + secVal ) * 1000;
	AddCurMSecTimer(triggertMsecVal, callbackFunc);
}

void Timer::AddMSecTimer(uint32_t msecVal, TimerCallback callbackFunc)
{
	uint64_t triggertMsecVal = ( GetCurMSec() + msecVal );
	AddCurMSecTimer(triggertMsecVal, callbackFunc);
}

void Timer::AddCurMSecTimer(uint64_t curMSecVal, TimerCallback callbackFunc)
{	
	curMSecVal /= 10;
	auto it = m_timers.find(curMSecVal);
	if(it != m_timers.end())
	{
		Timer::TimerObj* obj = GetTimerObj(curMSecVal, callbackFunc);
		it->second.push_back(obj);
	}
	else
	{
		std::list<Timer::TimerObj*>& newVal = m_timers[curMSecVal];
		Timer::TimerObj* obj = GetTimerObj(curMSecVal, callbackFunc);
		newVal.push_back(obj);
	}
	if(m_traceTime == 0)
	{
		m_traceTime = curMSecVal;
	}
}

void Timer::SetCacheSize(uint32_t val)
{
	m_cacheSize = val;
}

void Timer::Loop(uint64_t curMsec)
{
	if(0 == curMsec)
	{
		curMsec = GetCurMSec();
	}
	
	curMsec /= 10;
	uint64_t i = m_traceTime;
	while(i <= curMsec )
	{
		auto it = m_timers.find(i);
		if(it != m_timers.end())
		{
			for(auto& it2 : it->second)
			{
				it2->func(curMsec);
				if(m_cacheSize > m_timersCache.size())
				{
					m_timersCache.push_back(it2);
				}
				else
				{
					delete it2;
				}
			}
			it->second.clear();
			m_timers.erase(it);
		}
		i += 10;
	}
	m_traceTime = curMsec;
}

}
