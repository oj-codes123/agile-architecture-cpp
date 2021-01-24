
#pragma once

#include <iostream>
#include <vector>
#include <functional>

namespace agile
{

class IConnectionManager;	
class SignalHandler
{
public:
    static SignalHandler& Instance(){ static SignalHandler obj; return obj; }
	~SignalHandler();
	
	bool Init();
	
    void SetOnStopSignalCallback(std::function<void(int)> func);
    void SetOnEventSignalCallback(std::function<void(int)> func);
	
	void CallStopSignalFunc(int signalVal);
	void CallEventSignalFunc(int signalVal);
	
	void AddConnectionManager(IConnectionManager* mgr);
	void StopAllConnectionManager();
	
private:
	SignalHandler() { }
	
private:
    bool m_isInit = false;
	
    std::function<void(int)> m_stop_func = nullptr;
	std::function<void(int)> m_event_func  = nullptr;
	
	std::vector<IConnectionManager*> m_connMgrs;
};

}
