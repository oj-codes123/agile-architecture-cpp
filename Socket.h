
#pragma once

#include <errno.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h> 
#include <vector>

namespace agile
{
	
class Socket
{
public:
    Socket(){ }
    virtual ~Socket(){ }

    int Init(){ m_socketId = socket(AF_INET, SOCK_STREAM, 0); return m_socketId; }

    void SetSocketId(int socketId){ m_socketId = socketId; }
    int GetSocketId(){ return m_socketId; }

    int SetSendBuffer(int size)
    {
        int optval = size;
        return ::setsockopt(m_socketId, SOL_SOCKET, SO_SNDBUF, (const char*)&optval, sizeof(int) );
    }

    int SetRecvBuffer(int size)
    {
        int optval = size;
        return ::setsockopt(m_socketId, SOL_SOCKET, SO_RCVBUF, (const char*)&optval, sizeof(int) );
    }

    int SetSendTimeout(int timeout)
    {
        int optval = timeout;
        return ::setsockopt(m_socketId, SOL_SOCKET, SO_SNDTIMEO, (const char*)&optval, sizeof(int) );
    }

    int SetRecvTimeout(int timeout)
    {
        int optval = timeout;
        return ::setsockopt(m_socketId, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, sizeof(int) );
    }

    int SetTcpNoDelay(bool on)
    {
        int optval = on ? 1 : 0;
		return ::setsockopt(m_socketId, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(int));
    }

    int SetReuseAddr(bool on)
    {
        int optval = on ? 1 : 0;
        return ::setsockopt(m_socketId, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(int));
    }

    int SetReusePort(bool on)
    {
        int optval = on ? 1 : 0;
		return ::setsockopt(m_socketId, SOL_SOCKET, SO_REUSEPORT, (const char*)&optval, sizeof(int));
    }

    int SetKeepAlive(bool on)
    {
        int optval = on ? 1 : 0;
        return ::setsockopt(m_socketId, SOL_SOCKET, SO_KEEPALIVE, (const char*)&optval, sizeof(int));
    }

    int SetLinger(bool on, int timeVal)
    {
        linger sLinger;
        sLinger.l_onoff  = (int)on;
        sLinger.l_linger = timeVal;
        return ::setsockopt(m_socketId, SOL_SOCKET, SO_LINGER, (const char*)&sLinger, sizeof(linger) );
    }

    int SetNoBlocking(bool on)
    {
        int flags = fcntl (m_socketId, F_GETFL, 0);
        if ( -1 == flags)
        {
            return -1;
        }
		
        int bVal = on ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
        if ( -1 == fcntl(m_socketId, F_SETFL, bVal) )
        {
            return -2;
        }
        return 0;
    }

    int CommonSet(int bufferSize)
    {
        int ret = SetTcpNoDelay(true);
        ret |= SetReuseAddr(true);
        ret |= SetReusePort(true);
        ret |= SetKeepAlive(true);
        ret |= SetNoBlocking(true);
        ret |= SetLinger(false, 0);
		ret |= SetSendBuffer(bufferSize);
		ret |= SetRecvBuffer(bufferSize);
        return ret;
    }
	
	int CommonSetWithoutSetNoBlocking(int bufferSize)
    {
        int ret = SetTcpNoDelay(true);
        ret |= SetReuseAddr(true);
        ret |= SetReusePort(true);
        ret |= SetKeepAlive(true);
        ret |= SetLinger(false, 0);
		ret |= SetSendBuffer(bufferSize);
		ret |= SetRecvBuffer(bufferSize);
        return ret;
    }

    int Bind(const std::string& ip, int port)
    {
        struct sockaddr_in local;
        local.sin_port        = htons(port);
        local.sin_family      = AF_INET;
		if(ip=="INADDR_ANY" || ip == "inaddr_any" )
		{
			local.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			uint32_t int_ip = inet_addr(ip.c_str());
			local.sin_addr.s_addr = htonl(int_ip);
		}
        return bind(m_socketId, (struct sockaddr*)&local, sizeof(local) );
    }
	void GetIpsWithName(const std::string& name, std::vector<std::string>& ips)
	{
		struct hostent* hptr = gethostbyname( name.c_str() );
		if (hptr)
		{
			struct in_addr** addr_list = (struct in_addr**)hptr->h_addr_list; 
			for(int i = 0; addr_list[i] != nullptr; i++) 
			{
				std::string ip( inet_ntoa(*addr_list[i]) );
				ips.push_back(ip);
			}
		}
	}

    int Listen()
    {
        SetNoBlocking(true);
        return listen(m_socketId, SOMAXCONN);
    }

    int ConnectTo(const std::string& ip, int port)
    {
        struct sockaddr_in serveraddr;
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port   = htons(port);
        //bzero(&(serveraddr.sin_zero), 8);
        inet_pton(AF_INET, ip.c_str(), &serveraddr.sin_addr.s_addr);
        return connect(m_socketId, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    }

    int Close()
    {
		return close(m_socketId);
    }

private:
    int m_socketId = 0;
};

}
