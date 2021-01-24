
#include <assert.h>
#include <signal.h>
#include "SignalHandler.h"
#include "ConnectionManager.h"

namespace agile
{

static void on_stop_signal_catchfunc(int val)
{
	SignalHandler::Instance().CallStopSignalFunc(val);
}

static void on_sigusr1_signal_catchfunc(int val)
{
	SignalHandler::Instance().CallEventSignalFunc(val);
}

SignalHandler::~SignalHandler()
{
	m_connMgrs.clear();
}

bool SignalHandler::Init()
{
	if(m_isInit)
	{
		return true;
	}
	
	signal(SIGINT, &on_stop_signal_catchfunc);
	signal(SIGTERM, &on_stop_signal_catchfunc);
	signal(SIGQUIT, &on_stop_signal_catchfunc);
	signal(SIGUSR2, &on_stop_signal_catchfunc);
	
	signal(SIGUSR1, &on_sigusr1_signal_catchfunc);
	
	/*
	assert( signal(SIGINT, &on_stop_signal_catchfunc) == SIG_DFL );
	assert( signal(SIGTERM, &on_stop_signal_catchfunc) == SIG_DFL );
	assert( signal(SIGQUIT, &on_stop_signal_catchfunc) == SIG_DFL );
	assert( signal(SIGUSR2, &on_stop_signal_catchfunc) == SIG_DFL );
	
	assert( signal(SIGUSR1, &on_sigusr1_signal_catchfunc) == SIG_DFL );
	*/
	m_isInit = true;
	
	return true;
}

void SignalHandler::AddConnectionManager(IConnectionManager* mgr)
{
	m_connMgrs.push_back(mgr);
}

void SignalHandler::StopAllConnectionManager()
{
	for(auto& it : m_connMgrs)
	{
		IConnectionManager* mgr = it;
		((ConnectionManager*)mgr)->FreeConns();
	}
	m_connMgrs.clear();
}

void SignalHandler::CallStopSignalFunc(int signalVal)
{
	if(m_stop_func)
	{
		m_stop_func(signalVal);
	}
	
	StopAllConnectionManager();

	exit(0);
}

void SignalHandler::CallEventSignalFunc(int signalVal)
{
	if(m_event_func)
	{
		m_event_func(signalVal);
	}
}

void SignalHandler::SetOnStopSignalCallback(std::function<void(int)> func)
{ 
	m_stop_func = func; 
}

void SignalHandler::SetOnEventSignalCallback(std::function<void(int)> func)
{
	m_event_func = func; 
}

}
