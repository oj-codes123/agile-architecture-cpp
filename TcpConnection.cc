
#include "Logger.h"
#include "Timer.h"
#include "TcpConnection.h"
#include "ConnectionManager.h"

namespace agile
{

TcpConnection::TcpConnection()
{
    m_eventObj.events = GEpollModule;
}
	
TcpConnection::TcpConnection(int socketId, int buffSize)
{
    m_eventObj.events = GEpollModule;
	SetInitValue(socketId, buffSize);
}

void TcpConnection::SetInitValue(int socketId, int buffSize)
{  
    m_socket.SetSocketId(socketId);
    m_socket.CommonSet(buffSize);
}

TcpConnection::~TcpConnection()
{
	if(m_delegate)
	{
		delete m_delegate;
		m_delegate = nullptr;
	}
}

void TcpConnection::FreeConnect()
{
   if( TcpConnState_Closed != m_state && TcpConnState_NULL != m_state )
   {
	   LOG_INFO_S << "Free connection socketId:" << m_socket.GetSocketId() << ",intKey:" << m_intKey
		          << ", strKey:" << m_key << ", name:" << m_name;
				   
       DeleteEvent();
       m_socket.Close();
       m_state = TcpConnState_Closed;
   }
}

void TcpConnection::Close()
{
    LOG_INFO("socket:%d", m_socket.GetSocketId());

	if(TcpConnState_Closing == m_state || TcpConnState_Closed == m_state)
	{
		return;
	}
	
	FreeConnect();
	m_connMgr->HandleClose(this);
}

void TcpConnection::Stop()
{
	if(m_delegate)
	{
		m_delegate->OnStop(this);
	}	
}

void TcpConnection::Start()
{
	if(m_delegate)
	{
		m_delegate->OnStart(this);
	}
}

bool TcpConnection::Run()
{
	if(m_delegate)
	{
		return m_delegate->OnRun(this);
	}
	return false;
}

void TcpConnection::SetName(const std::string& name)
{
	m_name = name;
	m_connMgr->OnSetConnectionName(this, name);
}

void TcpConnection::SetKey(const std::string& key)
{
	m_key = key;
	m_connMgr->OnSetConnectionStrKey(this, m_key);
}

void TcpConnection::SetIntKey(uint64_t key)
{
	m_intKey = key;
	m_connMgr->OnSetConnectionIntKey(this, m_intKey);
}

std::string TcpConnection::GetPeerName(int socketId)
{
	struct sockaddr_in clientAdr;
	socklen_t clientLen = sizeof(clientAdr);
	if (0 == socketId)
	{
		socketId = m_socket.GetSocketId();
	}
	
	getpeername(socketId, (struct sockaddr*) &clientAdr, &clientLen);
	
	std::stringstream ss;
	ss << inet_ntoa(clientAdr.sin_addr);
	return ss.str();
}

int TcpConnection::Send(Buffer& buff)
{
	if( m_state != TcpConnState_Open )
	{
		LOG_ERROR("Send error with Connection state %d", m_state);
		return -1;
	}

	SizeInt packetSize = (SizeInt)buff.GetWriteIndex();
	buff.WriteTo(&packetSize, sizeof(SizeInt), 0);

	return Send(buff.GetBuffer(), packetSize);
}


int TcpConnection::Send(const char* buff, int buff_len)
{
	if( m_state != TcpConnState_Open )
	{
		LOG_ERROR("Send error with Connection state %d", m_state);
		return -1;
	}
	
	m_writeBuffer.Write(buff, buff_len);
	if(!IsWriting()){
		EnableWriting();
	}
	return 0;

	/*int traceIndex = 0;
	int ret = SendCharsHelper(buff, traceIndex, buff_len);
	while (ret < (int)buff_len && ret > 0)
	{
		traceIndex += ret;
		buff_len   -= ret;
		ret = SendCharsHelper(buff, traceIndex, buff_len);
	}
	return ret > 0 ? buff_len : -1;*/
}

void TcpConnection::HandleWrite()
{
	uint32_t rIndex = m_writeBuffer.GetReadIndex();
	uint32_t wIndex = m_writeBuffer.GetWriteIndex();
	uint32_t packetSize = wIndex - rIndex;
	int len = TcpConnection::SendCharsHelper(m_writeBuffer.GetBuffer(), rIndex, packetSize);
	if(len >= packetSize){
		DisableWriting();
		m_writeBuffer.ResetIndex();
	} else {
		EnableWriting();
	}
}

int TcpConnection::SendCharsHelper(const char* buff, int traceIndex, int packetSize)
{
	{ LOG_DEBUG_S << "traceIndex:" << traceIndex << ", packetSize:" << packetSize; }
	
	int ret = send(m_socket.GetSocketId(), buff + traceIndex, (unsigned)packetSize, 0);
	if(ret > 0)
	{
		return ret;
	}
	else if (0 == ret)
	{
		HandleClose();
		LOG_ERROR("Send to socket_id:%d  close ret:%d", m_socket.GetSocketId(), ret);
		return -1;
	}
	else
	{
		if (EINTR == errno || EWOULDBLOCK == errno || EAGAIN == errno || EINPROGRESS == errno )
		{
			LOG_WARN("errno:%d", (int)errno);
			return 0;
		}
		else
		{
			if (EPIPE == errno)
			{
				HandleClose();
				LOG_ERROR("Send close errno:EPIPE");
				return -1;
			}
			else
			{
				HandleClose();
				LOG_ERROR("Send err ret:%d close now", errno);
				return -1;
			}
		}
	}
}

void TcpConnection::Ping()
{
	uint32_t sendSize = 4;
	Send((const char*)&sendSize, sizeof(uint32_t));
}

void TcpConnection::HandleError()
{
	LOG_ERROR("sokcet:%d error", m_socket.GetSocketId());
	if(m_delegate)
	{
		m_delegate->OnError(this);
	}
}

void TcpConnection::HandleClose()
{
    LOG_INFO("socket:%d", m_socket.GetSocketId());
	if(TcpConnState_Closing == m_state || TcpConnState_Closed == m_state)
	{
		return;
	}
	m_connMgr->HandleClose(this);
}

int TcpConnection::HanldeRead()
{
	if(m_delegate)
	{
		m_delegate->OnMessage(this);
	}
	return 0;
}

void TcpConnection::UpdateEvent()
{
   int ret = epoll_ctl(m_connMgr->GetEpoll()->GetEpollId(), EPOLL_CTL_MOD, m_socket.GetSocketId(), &m_eventObj);
   if( ret == -1 )
   {
       LOG_ERROR("ret:%d", ret);
   }
}

void TcpConnection::DeleteEvent()
{
   int ret = epoll_ctl(m_connMgr->GetEpoll()->GetEpollId(), EPOLL_CTL_DEL, m_socket.GetSocketId(), NULL);
   if( ret == -1 )
   {
       LOG_ERROR("ret:%d", ret);
   }
}

void TcpConnection::AddEvent()
{
    m_eventObj.events   |= GEpollReadEvent;
    m_eventObj.data.fd  = m_socket.GetSocketId();
    m_eventObj.data.ptr = this;
    int ret = epoll_ctl(m_connMgr->GetEpoll()->GetEpollId(), EPOLL_CTL_ADD, m_socket.GetSocketId(), &m_eventObj);
    if( ret == -1 )
    {
        LOG_ERROR("ret:%d", ret);
    }
    else
    {
        m_state = TcpConnState_Open;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ServerConnection::InitTcpListener(const std::string& ip, int port, int buffSize)
{
    m_buffSize = buffSize;
    m_socket.Init();
    m_socket.CommonSet(m_buffSize);
	
    int ret = m_socket.Bind(ip, port);
    if( 0 != ret )
    {
        LOG_ERROR("bind ret:%d ip:%s port:%d", ret, ip.c_str(), port);
        return ret;
    }
	
    ret = m_socket.Listen();
    if( 0 != ret )
    {
		LOG_ERROR("Listen ret:%d ip:%s port:%d", ret, ip.c_str(), port);
        return ret;
    }
	
    return 0;
}

int ServerConnection::HanldeRead()
{
    while (true)
    {
        struct sockaddr_in addrIn;
        socklen_t addrLen = sizeof(addrIn);

        int32_t clientId = accept(m_socket.GetSocketId(), (struct sockaddr*)&addrIn, &addrLen);
        if ( -1 == clientId )
        {
            if ( (errno == EAGAIN) || (errno == EWOULDBLOCK) )
            {
				return 1;
            }
            else
            {
                LOG_ERROR("server conn errno = %d", errno);
            }
            return -1;
        }
        else
        {
            LOG_INFO("new connection : %d", clientId);
			m_connMgr->OnNewConnection(clientId, m_buffSize);
        }
    }
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////
int ClientConnection::Connect()
{
	m_socket.Init();
	int ret =m_socket.SetNoBlocking(true);
	if( 0 != ret )
	{
		LOG_ERROR_S << " fail to connect [" << m_name << "]" << m_ip << ":" << m_port <<" SetNoBlocking ret:" << ret;
		return ret;
	}
  
	bool isConnecting = false;
	ret = m_socket.ConnectTo(m_ip, m_port);
	if( 0 != ret )
	{
		if ( errno == EINPROGRESS && ret == -1)
		{
			isConnecting = true;
		}
		else
		{
			LOG_ERROR_S << "fail to connect [" << m_name << "]" << m_ip << ":" << m_port <<" ret:" << ret << " errno:" << (int)errno;
			return ret;
		}
	}
	
	if(isConnecting)
	{
		LOG_INFO_S << "connecting [" << m_name << "]" << m_ip << ":" << m_port << " ...";
	  
		fd_set readfds;
		fd_set writefds;
      
		int sockfd = m_socket.GetSocketId();
		FD_ZERO( &readfds );
		FD_ZERO( &writefds );
		FD_SET( sockfd, &readfds );
		FD_SET( sockfd, &writefds );
	  
		struct timeval timeout;
		timeout.tv_sec = m_timeout;
		timeout.tv_usec = 0;
 
		ret = select( sockfd + 1, &readfds, &writefds, NULL, &timeout );
		if ( ret <= 0 )
		{
			LOG_ERROR_S << "connect fail [" << m_name << "]" << m_ip << ":" << m_port << " closed ";
			m_socket.Close();
			return ret;
		}
 
		if ( !FD_ISSET( sockfd, &readfds ) && !FD_ISSET(sockfd, &writefds) )
		{
			LOG_ERROR_S << "connect fail [" << m_name << "]" << m_ip << ":" << m_port << " no events ";
			m_socket.Close();
			return -1;
		}
 
		int errorNum = 0;
		socklen_t length = sizeof( errorNum );
		if( getsockopt( sockfd, SOL_SOCKET, SO_ERROR, &errorNum, &length ) < 0 )
		{
			LOG_ERROR_S << "connect fail [" << m_name << "]" << m_ip << ":" << m_port << " get socket option failed";
			m_socket.Close();
			return -1;
		}
 
		if( errorNum != 0 )
		{
			LOG_ERROR_S << "connect fail [" << m_name << "]" << m_ip << ":" << m_port << " errorNum:" << errorNum;
			m_socket.Close();
			return -1;
		}
	}
  
	m_socket.CommonSetWithoutSetNoBlocking(m_buffSize);
	
	LOG_INFO_S << "connect [" << m_name << "]" << m_ip << ":" << m_port << " successfully";
	
	return 0;
}

int ClientConnection::Connect(const std::string& name, const std::string& ip, int port, uint32_t timeout, int buffSize)
{
    m_name = name;
    m_ip   = ip;
    m_port = port;
	m_timeout = timeout;
	m_buffSize = buffSize;
    return Connect();
}

int ClientConnection::ReConnect()
{
    LOG_INFO_S << "ReConnect [" << m_name << "]" << m_ip << ":" << m_port;
    FreeConnect();
    return Connect();
}

}
