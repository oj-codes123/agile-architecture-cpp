
#include "Logger.h"
#include "Timer.h"
#include "EpollLoop.h"

namespace agile
{

EpollLoop::~EpollLoop()
{
	if(m_sharedEpoll)
	{
		delete m_sharedEpoll;
		m_sharedEpoll = nullptr;
	}
	
	for(auto it = m_epolls.begin(); it != m_epolls.end(); it++)
	{
		delete *it;
	}
	m_epolls.clear();
}

void EpollLoop::Run()
{
	static bool isRunning = false;
    if(isRunning) 
	{ 
		return; 
	}
    isRunning = true;

    while(true)
    {
		if(m_sharedEpoll)
		{
			m_sharedEpoll->Loop();
		}
		
		if(!m_epolls.empty())
		{
			for(auto& it : m_epolls)
			{
				it->Loop();
			}
		}
        Timer::Instance().Loop();
    }
	
    LOG_INFO_S << "finish loop epoll size:" << (uint32_t)m_epolls.size();
}

NetEpoll* EpollLoop::ShareEpoll()
{
	if(!m_sharedEpoll)
	{
		m_sharedEpoll = new NetEpoll(3, 100000);
	}
	return m_sharedEpoll;
}

void EpollLoop::AddEpoll(NetEpoll* epollObj)
{
	if(!epollObj)
	{ 
		return;
	}
	
	for(auto& it : m_epolls)
	{
		if(it->GetEpollId() == epollObj->GetEpollId() )
		{
			LOG_ERROR_S << "equal epollId:" << epollObj->GetEpollId();
			return;
		}
	}
	m_epolls.push_back(epollObj);
}

void EpollLoop::DeleteEpoll(NetEpoll* epollObj)
{
	if(!epollObj)
	{ 
		return;
	}
	
	for(auto it = m_epolls.begin(); it != m_epolls.end(); it++)
	{
		if((*it)->GetEpollId() == epollObj->GetEpollId() )
		{
			m_epolls.erase(it);
			return;
		}
	}	
}

}
