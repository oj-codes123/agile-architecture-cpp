
#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <sys/epoll.h>

namespace agile
{
const static int GEpollNoneEvent  = 0;
const static int GEpollReadEvent  = EPOLLIN | EPOLLPRI;
const static int GEpollWriteEvent = EPOLLOUT;
const static int GEpollModule     = EPOLLET;

class NetEpoll
{
public:
    NetEpoll(int timeout, int eventNum);
    ~NetEpoll(){ }

    void Loop();
    int GetEpollId(){ return m_epollId; }

private:
    int m_timeout;
    int m_epollId;
    int m_eventNum;
    std::vector<epoll_event> m_events;
};

}
