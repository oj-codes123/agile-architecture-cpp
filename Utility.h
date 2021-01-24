
#pragma once

#include <iostream>
#include <string>
#include <functional>

namespace agile
{

struct Utility
{
	static void AddSecTimer(uint32_t secVal,  std::function<void(uint64_t)> callbackFunc);
	
	static void AddMSecTimer(uint32_t msecVal, std::function<void(uint64_t)> callbackFunc);
	
	static void AddCurMSecTimer(uint64_t curMSecVal, std::function<void(uint64_t)> callbackFunc);
	
	static uint64_t CurSec();
	
    static uint64_t CurMSec();
	
	static uint32_t GetPid();
	
	static void SetOnStopSignalCallback(std::function<void(int)> func);
	
    static void SetOnEventSignalCallback(std::function<void(int)> func);
	
	static void AgileHandleException();
};

}
