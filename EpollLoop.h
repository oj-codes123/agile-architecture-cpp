
#pragma once

#include <list>
#include "Epoll.h"

namespace agile
{
class EpollLoop
{
public:
    static EpollLoop& Instance(){ static EpollLoop obj; return obj; }
	~EpollLoop();
	
	void Run();
	
	NetEpoll* ShareEpoll();
	
	void AddEpoll(NetEpoll* epollObj);
	
	void DeleteEpoll(NetEpoll* epollObj);

private:
	EpollLoop(){ }
	
private:
    NetEpoll* m_sharedEpoll = nullptr;
    std::list<NetEpoll*> m_epolls;
};

}
