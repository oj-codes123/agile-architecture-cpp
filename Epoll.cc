
#include <errno.h>
#include "Epoll.h"
#include "Logger.h"
#include "TcpConnection.h"
#include "ConnectionManager.h"

namespace agile
{

NetEpoll::NetEpoll(int timeout, int eventNum)
{
    m_timeout  = timeout;
    m_eventNum = eventNum;
	m_epollId  = epoll_create(m_eventNum);
    m_events.resize(m_eventNum);
}

void NetEpoll::Loop()
{
    int numEvents = epoll_wait(m_epollId, &*m_events.begin(), (int)m_events.size(), m_timeout);
	
    if ( numEvents > 0 )
    {
        for (int i = 0; i < numEvents; ++i)
        {
            TcpConnection* conn = static_cast<TcpConnection*>( m_events[i].data.ptr );
			if(conn->GetType() == ConnType_Server_Listen)
			{
				conn->HanldeRead();
			}
			else
			{
				ConnectionManager* mgr = (ConnectionManager*)conn->GetConnectionManager();
				if(mgr)
				{
					(mgr)->HandleEvents(m_events[i].events, conn);
				}
			}

        }
		return;
    }
	
    if (numEvents < 0)
    {
       	int savedErrno = errno;
        if (savedErrno != EINTR)
        {
            LOG_SYS("NetEpoll::Loop() err:%d", savedErrno);
            exit(1);
        }
    }
}

}
