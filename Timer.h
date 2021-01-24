
#pragma once

#include <functional>
#include <unordered_map>
#include <list>

namespace agile
{

typedef std::function<void(uint64_t)> TimerCallback;

class Timer
{
public:
	static Timer& Instance() { static Timer obj; return obj; }
	~Timer();
	
	void SetCacheSize(uint32_t val);

	void AddSecTimer(uint32_t secVal, TimerCallback callbackFunc);
	
	void AddMSecTimer(uint32_t msecVal, TimerCallback callbackFunc);
	
	void AddCurMSecTimer(uint64_t curMSecVal, TimerCallback callbackFunc);

	void Loop(uint64_t curMsec=0);

private:
	Timer() {  }

	struct TimerObj
	{
		uint64_t val = 0;
		TimerCallback func = nullptr;
	};
	
	uint64_t GetCurSec();
	uint64_t GetCurMSec();
	
	TimerObj* GetTimerObj(uint64_t msecVal, TimerCallback callbackFunc);


private:
	uint32_t m_cacheSize  = 1024;
	uint64_t m_traceTime = 0;
	std::list<TimerObj*> m_timersCache;
	std::unordered_map<uint64_t, std::list<TimerObj*> > m_timers;
};

}
