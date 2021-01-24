
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include "Utility.h"
#include "Timer.h"
#include "SignalHandler.h"

namespace agile
{
	
void Utility::AddSecTimer(uint32_t secVal, std::function<void(uint64_t)> callbackFunc)
{
	Timer::Instance().AddSecTimer(secVal, callbackFunc);
}
	
void Utility::AddMSecTimer(uint32_t msecVal, std::function<void(uint64_t)> callbackFunc)
{
	Timer::Instance().AddMSecTimer(msecVal, callbackFunc);
}

void AddCurMSecTimer(uint64_t curMSecVal, std::function<void(uint64_t)> callbackFunc)
{
	Timer::Instance().AddCurMSecTimer(curMSecVal, callbackFunc);
}

uint64_t Utility::CurSec()
{
	return time(NULL);
}

uint64_t Utility::CurMSec()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	
	return (uint64_t) ( tv.tv_sec * 1000 + tv.tv_usec / 1000 ); 
}

uint32_t Utility::GetPid()
{
	pid_t m_pid2 = getpid();
	return (uint32_t)m_pid2;
}

void Utility::SetOnStopSignalCallback(std::function<void(int)> func)
{
	SignalHandler::Instance().SetOnStopSignalCallback(func);
}
	
void Utility::SetOnEventSignalCallback(std::function<void(int)> func)
{
	SignalHandler::Instance().SetOnEventSignalCallback(func);
}

void Utility::AgileHandleException()
{
	SignalHandler::Instance().StopAllConnectionManager();
}

}
