
#include "Timer.h"
#include "Logger.h"
#include "EpollLoop.h"
#include "TcpConnection.h"
#include "ConnectionManager.h"

namespace agile
{

ConnectionManager::ConnectionManager()
{
    m_conOpened  = true;
    m_epoll      = nullptr;
	m_factory    = nullptr;
	
	static uint64_t globalMgrId = 0;
	++globalMgrId;
	m_mgrId = globalMgrId;
	m_runMsec    = 0;
	m_runMsecTrace = 0;

    Timer::Instance().AddSecTimer(1, std::bind(&ConnectionManager::CheckClosingConns, this, std::placeholders::_1) );
	
	Timer::Instance().AddSecTimer(10, std::bind(&ConnectionManager::CheckConnectingConns, this, std::placeholders::_1));	
}

bool ConnectionManager::GetSokcetIdsByName(const std::string& name, std::vector<int>& socketIds)
{
	std::map<std::string, NameSocketData>::iterator it_name = m_nameSockets.find(name);
	if( it_name != m_nameSockets.end() )
	{
		socketIds = it_name->second.sockets;
		return true;
	}
	return false;
}

Connection* ConnectionManager::GetConnectionBySocketId(int socketId)
{
	std::map<int, TcpConnection*>::iterator it = m_conns.find(socketId);
	return it != m_conns.end() ? it->second : nullptr;
}

Connection* ConnectionManager::GetConnectionByIntKey(uint64_t intKey)
{
	std::map<uint64_t, int>::iterator it_int = m_intKeySokcets.find(intKey);
	return it_int != m_intKeySokcets.end() ? GetConnectionBySocketId(it_int->second) : nullptr;
}

Connection* ConnectionManager::GetConnectionByStrKey(const std::string& strKey)
{
	std::map<std::string, int>::iterator it_str = m_strKeySockets.find(strKey);
	return it_str != m_strKeySockets.end() ? GetConnectionBySocketId(it_str->second) : nullptr;
}

Connection* ConnectionManager::GetConnectionByName(const std::string& name)
{
	std::map<std::string, NameSocketData>::iterator it_name = m_nameSockets.find(name);
	if( it_name != m_nameSockets.end() )
	{
		int socketId = it_name->second.GetSocket();
		return socketId > 0 ? GetConnectionBySocketId(socketId) : nullptr;
	}
	return nullptr;
}

void ConnectionManager::DeleteTcpConnection(TcpConnection* conn)
{
	auto it = m_conns.find(conn->GetSocketId());
	if(it != m_conns.end())
	{
		m_conns.erase(it);
	}
	
	auto it_run = m_runConns.find(conn->GetSocketId());
	if(it_run != m_runConns.end())
	{
		m_runConns.erase(it_run);
	}
	
	auto it_int = m_intKeySokcets.find(conn->GetIntKey());
	if( it_int != m_intKeySokcets.end() )
	{
		m_intKeySokcets.erase(it_int);
	}
	
	auto it_str = m_strKeySockets.find(conn->GetKey());
	if( it_str != m_strKeySockets.end() )
	{
		m_strKeySockets.erase(it_str);
	}
	
	auto it_name = m_nameSockets.find(conn->GetName());
	if( it_name != m_nameSockets.end() )
	{
		auto it_v = it_name->second.sockets.begin();
		while( it_v != it_name->second.sockets.end() )
		{
			if( *it_v == conn->GetSocketId() )
			{
				it_v = it_name->second.sockets.erase(it_v);
				break;
			}
			else
			{
				++it_v;
			}
		}
	}
}

void ConnectionManager::CloseConnectionBySocketId(int socketId)
{
	TcpConnection* conn = (TcpConnection*)GetConnectionBySocketId(socketId);	
	if(conn)
	{
		conn->Stop();
		m_closeing.push_back(conn);
		DeleteTcpConnection(conn);
	}
}

void ConnectionManager::CloseConnectionByIntKey(uint64_t intKey)
{
	int socketId = -1;
	auto it = m_intKeySokcets.find(intKey);
	if( it != m_intKeySokcets.end() )
	{
		socketId = it->second;
		m_intKeySokcets.erase(it);
	}
	
	if(socketId > 0)
	{
		CloseConnectionBySocketId(socketId);
	}
}

void ConnectionManager::CloseConnectionByStrKey(const std::string& strKey)
{
	int socketId = -1;
	auto it = m_strKeySockets.find(strKey);
	if( it != m_strKeySockets.end() )
	{
		socketId = it->second;
		m_strKeySockets.erase(it);
	}
	
	if(socketId > 0)
	{
		CloseConnectionBySocketId(socketId);
	}
}

void ConnectionManager::SendBufferBySocketId(int socketId, Buffer& buff)
{
	Connection* conn = GetConnectionBySocketId(socketId);
	if(conn)
	{
		conn->Send(buff);
	}
	else
	{
		LOG_ERROR_S << "no socketId:" << socketId;
	}
}

void ConnectionManager::SendBufferByIntKey(uint64_t intKey, Buffer& buff)
{
	Connection* conn = GetConnectionByIntKey(intKey);
	if(conn)
	{
		conn->Send(buff);
	}
	else
	{
		LOG_ERROR_S << "no intKey:" << intKey;
	}	
}

void ConnectionManager::SendBufferByStrKey(const std::string& strKey, Buffer& buff)
{
	Connection* conn = GetConnectionByStrKey(strKey);
	if(conn)
	{
		conn->Send(buff);
	}
	else
	{
		LOG_ERROR_S << "no strKey:" << strKey;
	}	
}

void ConnectionManager::SendBufferByName(const std::string& name, Buffer& buff)
{
	Connection* conn = GetConnectionByName(name);
	if(conn)
	{
		conn->Send(buff);
	}
	else
	{
		LOG_ERROR_S << "no name:" << name;
	}	
}


void ConnectionManager::BroadcastConns(Buffer& buff)
{
	for(auto& it : m_conns)
	{
		it.second->Send(buff);
	}
	
	if(m_conns.empty())
	{
		LOG_INFO_S << "no connections";
	}
}

void ConnectionManager::BroadcastConns(const std::string& name, Buffer& buff)
{
	bool is_ok = false;
	
	auto it = m_nameSockets.find(name);
	if(it != m_nameSockets.end())
	{
		is_ok = !it->second.sockets.empty();
		auto it_v = it->second.sockets.begin();
		for( ; it_v != it->second.sockets.end(); it_v++)
		{
			auto it_conn = m_conns.find(*it_v);
			if( it_conn != m_conns.end() )
			{
				it_conn->second->Send(buff);
			}
		}
	}
	
	if(!is_ok)
	{
		LOG_INFO_S << "no connection with name:" << name;
	}
}
	
size_t ConnectionManager::GetConnsSize(const std::string& name)
{
	auto it = m_nameSockets.find(name);
	return it != m_nameSockets.end() ? it->second.sockets.size() : 0;
}

ConnectionManager::~ConnectionManager()
{
	if(m_factory)
	{
		delete m_factory;
		m_factory = nullptr;
	}
	
	FreeConns();
	EpollLoop::Instance().DeleteEpoll(m_epoll);
}

void ConnectionManager::FreeConns()
{	
    auto it = m_conns.begin();
    while( it != m_conns.end() )
    {
		if(it->second)
		{
			if (m_connFuncs.closeCallback) 
			{ 
				(m_connFuncs.closeCallback)(it->second); 
			}
			it->second->Stop();
		
			delete it->second;
			it->second = nullptr;
		}
        it++;
    }
    m_conns.clear();
	    
	auto itL = m_listenConns.begin();
    while( itL != m_listenConns.end() )
    {
		if(itL->second)
		{
			delete itL->second;
			itL->second = nullptr;
		}
        itL++;
    }
	
	m_runConns.clear();
	
	m_nameSockets.clear();
	m_intKeySokcets.clear();
	m_strKeySockets.clear();
	
	for(size_t i = 0; i < m_closeing.size(); i++)
	{
		TcpConnection* tcpConn = m_closeing[i];
		if(tcpConn)
		{
			if (m_connFuncs.closeCallback) 
			{ 
				(m_connFuncs.closeCallback)(tcpConn); 
			}
			tcpConn->Stop();

			delete tcpConn;
			tcpConn = nullptr;
		}
	}
	m_closeing.clear();
}

void ConnectionManager::CheckClosingConns(uint32_t curTime)
{
	int num = 500;
	while( !m_closeing.empty() && num > 0 )
	{
		TcpConnection* tcpConn = m_closeing.front();
		if(tcpConn)
		{
			if (m_connFuncs.closeCallback) 
			{ 
				(m_connFuncs.closeCallback)(tcpConn); 
			}
			
			tcpConn->Stop();
		
			delete tcpConn;
			tcpConn = nullptr;
		}
		m_closeing.pop_front();
		--num;
	}
	Timer::Instance().AddSecTimer(1, std::bind(&ConnectionManager::CheckClosingConns, this, std::placeholders::_1) );
}

void ConnectionManager::CheckConnectingConns(uint32_t curTime)
{	
 	int num = 20;
	int count = (int)m_closeingReconnect.size();
	while( count > 0 && num > 0 )
	{
		ClientConnection* conn = (ClientConnection*)m_closeingReconnect.front();
		if( 0 == conn->ReConnect() )
		{
			AddConnection(conn);
			m_closeingReconnect.pop_front();
		}
		else
		{
			m_closeingReconnect.pop_front();
			m_closeingReconnect.push_back(conn);
		}
		--num;
		--count;
	}
	Timer::Instance().AddSecTimer(10, std::bind(&ConnectionManager::CheckConnectingConns, this, std::placeholders::_1));
}

void ConnectionManager::OnNewConnection(int socketId, int buffSize)
{
	Connection* conn = GetConnectionBySocketId(socketId);
	if(conn)
	{
		CloseConnectionBySocketId(socketId);
		LOG_ERROR("same socketId:%d", socketId);
		return;
	}

    if(m_conOpened)
    {
        TcpConnection* newConn = new TcpConnection();
		newConn->SetInitValue(socketId, buffSize);
	  
		std::string cIp = newConn->GetPeerName();
		auto itM = m_maskIps.find(cIp);
		if (itM != m_maskIps.end())
		{
		  //TODO
		  //if (itM->second < Timer::Instance().CurSecDeviation1() )
		  {
			  LOG_WARN("mask ip:%s", cIp.c_str());
			  delete newConn;
			  newConn = nullptr;
			  return;
		  }
	  }

	  if(m_factory)
	  {
		  newConn->SetDelegate(m_factory->CreateConnectionDelegate() );
	  }
	  newConn->SetConnectionManager(this);
      newConn->AddEvent();
	  
      m_conns[socketId] = newConn;
	  
	  if( m_connFuncs.newCallback )
      {
		(m_connFuncs.newCallback)(newConn);
      }
	  
	  newConn->Start();
	  AddRunConnection(newConn);
    }
    else
    {
        Socket cSocket;
        cSocket.SetSocketId(socketId);
        cSocket.Close();
		LOG_WARN("new connection state err(not opened) socketId:%d ---------------------------------------- ", socketId);
    }
}

bool ConnectionManager::Init(bool isShared)
{
    if(!m_epoll)
    {
		if(isShared)
		{
			m_epoll = EpollLoop::Instance().ShareEpoll();
		}
		else
		{
			m_epoll = new NetEpoll(3, 10000);
			EpollLoop::Instance().AddEpoll(m_epoll);
		}
    }
	return true;
}

void ConnectionManager::SetConnectionDelegateFacotry(ConnectionDelegateFacotry* factory)
{
	m_factory = factory;
}

bool ConnectionManager::AddConnection(TcpConnection* conn)
{
	if(m_factory)
	{
		conn->SetDelegate(m_factory->CreateConnectionDelegate() );
	}
	conn->SetConnectionManager(this);
	conn->AddEvent();
	
	int socketId = conn->GetSocketId();
	auto it = m_conns.find(socketId);
	if(it != m_conns.end() )
	{
		LOG_ERROR("socketId:%d exist", socketId);
		return false;
	}
	m_conns[socketId] = conn;
	
	if(conn->GetIntKey() > 0)
	{
		OnSetConnectionIntKey(conn, conn->GetIntKey());
	}
	
	if(!conn->GetKey().empty())
	{
		OnSetConnectionStrKey(conn, conn->GetKey());
	}
	
	if(!conn->GetName().empty())
	{
		OnSetConnectionName(conn, conn->GetName());
	}
	
	conn->Start();
	AddRunConnection(conn);
	
	return true;
}

void ConnectionManager::SetConnectionRunTime(uint64_t msec)
{
	if(msec < 10)
	{
		LOG_ERROR_S << "timer msec too small with:" << msec;
		return;
	}
	
	m_runTimeMsec = msec;
	Timer::Instance().AddMSecTimer(m_runTimeMsec, std::bind(&ConnectionManager::RunConnections, this, std::placeholders::_1) );
}

void ConnectionManager::RunConnections(uint64_t curMSecTime)
{
	auto it = m_runConns.begin();
	while( it != m_runConns.end() )
	{
		if( !it->second->Run() )
		{
			m_runConns.erase(it++);
		}
		else
		{
			it++;
		}
	}
	Timer::Instance().AddMSecTimer(m_runTimeMsec, std::bind(&ConnectionManager::RunConnections, this, std::placeholders::_1) );
}

void ConnectionManager::SetConnectionPingTime(uint32_t sec)
{
	if(sec < 10)
	{
		LOG_ERROR_S << "timer sec too small with:" << sec;
		return;
	}
	m_pingTimeSec = sec;
	Timer::Instance().AddSecTimer(m_pingTimeSec, std::bind(&ConnectionManager::PingConnections, this, std::placeholders::_1) );	
}

void ConnectionManager::PingConnections(uint32_t curSecTime)
{
	for(auto& it : m_conns)
	{
		it.second->Ping();
	}
	Timer::Instance().AddSecTimer(m_pingTimeSec, std::bind(&ConnectionManager::PingConnections, this, std::placeholders::_1) );	
}

bool ConnectionManager::AddListenConnection(TcpConnection* conn)
{
	if(!conn)
	{
		return false;
	}

	conn->SetConnectionManager(this);
	conn->AddEvent();
	
	int socketId = conn->GetSocketId();
	
	auto it = m_listenConns.find(socketId);
	if(it != m_listenConns.end() )
	{
		LOG_ERROR("socketId:%d exist", socketId);
		return false;
	}
	m_listenConns[socketId] = conn;
	return true;
}

void ConnectionManager::AddRunConnection(TcpConnection* conn)
{
	if(conn)
	{
		m_runConns[conn->GetSocketId()] = conn;
	}
}

void ConnectionManager::DeleteRunConnection(int socketId)
{
	auto it = m_runConns.find(socketId);
	if( it != m_runConns.end() )
	{
		m_runConns.erase(it);
	}
}

void ConnectionManager::OnSetConnectionIntKey(TcpConnection* conn, uint64_t intKey)
{
	auto it = m_intKeySokcets.find(intKey);
	if( it != m_intKeySokcets.end())
	{
		LOG_ERROR("socketId:%d intKey:%lld exist", conn->GetSocketId(), intKey);
		return;
	}
	m_intKeySokcets[intKey] = conn->GetSocketId();
}

void ConnectionManager::OnSetConnectionStrKey(TcpConnection* conn, const std::string& strKey)
{
	auto it = m_strKeySockets.find(strKey);
	if( it != m_strKeySockets.end())
	{
		LOG_ERROR("socketId:%d strKey:%s exist", conn->GetSocketId(), strKey.c_str());
		return;
	}
	m_strKeySockets[strKey] = conn->GetSocketId();	
}

void ConnectionManager::OnSetConnectionName(TcpConnection* conn, const std::string& name)
{
	auto it = m_nameSockets.find(name);
	if( it != m_nameSockets.end() )
	{
		for(size_t i = 0; i < it->second.sockets.size(); i++)
		{
			if(conn->GetSocketId() == it->second.sockets[i])
			{
				LOG_ERROR("socketId:%d name:%s exist", conn->GetSocketId(), name.c_str());
				return;
			}
		}
		it->second.sockets.push_back(conn->GetSocketId());
	}
	else
	{
		NameSocketData new_data;
		new_data.sockets.push_back(conn->GetSocketId());
		m_nameSockets[name] = new_data;
	}
}

void ConnectionManager::HandleEvents(int iEvents, TcpConnection* conn)
{
    if ( (iEvents & EPOLLHUP ) && !(iEvents & EPOLLIN) )
    {
		LOG_ERROR(" close iEvents:%d sokcet:%d error", iEvents, conn->GetSocketId());
		HandleClose(conn);
		return;
    }

    if ( iEvents & EPOLLERR )
    {
		LOG_ERROR(" close iEvents:%d sokcet:%d EPOLLERR error", iEvents, conn->GetSocketId());
        HandleError(conn);
		return;
    }

    if ( iEvents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP) )
    {
		HanldeRead(conn);
        return;
    }

    if ( iEvents & EPOLLOUT )
    {
        HandleWrite(conn);
    }
}

void ConnectionManager::HandleClose(TcpConnection* conn)
{
	TcpConnection* closeConn = (TcpConnection*)GetConnectionBySocketId(conn->GetSocketId());	
	if(closeConn)
	{
		closeConn->FreeConnect();
		
		if( closeConn->GetType() == (int8_t)ConnType_Client_ReConnect)
		{
			m_closeingReconnect.push_back(closeConn);
			closeConn->Stop();
		}
		else
		{
			m_closeing.push_back(closeConn);
		}
		DeleteTcpConnection(closeConn);
	}
}

void ConnectionManager::HandleError(TcpConnection* conn)
{
	if(conn)
	{
		conn->HandleError();
	}
	
	if (m_connFuncs.errorCallback)
	{
		(m_connFuncs.errorCallback)(conn);
	}
}

bool ConnectionManager::HanldeRead(TcpConnection* conn)
{	
	int buffSize  = 0;
	int readCount = 0;
	while(true)
	{
		buffSize = conn->GetBuffer().GetWriteAbleSize();
		if(buffSize < 12)
		{
			uint32_t pSize = conn->GetBuffer().GetPacketSize();
			if(pSize > conn->GetBuffer().GetBufferSize() )
			{
				conn->Close();
				LOG_ERROR_S << "pSize=" << pSize << " > bSize:" << conn->GetBuffer().GetBufferSize();
				break;
			}
		}
		
		readCount = read(conn->GetSocketId(), conn->GetBuffer().CurWrite(), buffSize);
		if(readCount > 0)
		{
			LOG_DEBUG_S << "GetPacketSize:" << conn->GetBuffer().GetPacketSize();
			
			conn->GetBuffer().IncrWriteIndex(readCount);
			if(conn->GetBuffer().IsFullPacket())
			{
				ConnectionDelegate* conDelegate = conn->GetConnectionDelegate();
				if(conDelegate)
				{
					conDelegate->OnMessage(conn);
				}
				else
				{
					if (m_connFuncs.readCallback)
					{
						(m_connFuncs.readCallback)(conn);
					}
				}
				
				LOG_DEBUG_S << "readCount:" << readCount;
				
				conn->GetBuffer().FinishReadPacket();
			}
		  
			if( readCount < buffSize )
			{
				break;
			}
			else
			{
				continue;	
			}		  
		}

		if ( -1 == readCount )
		{
			if (EPIPE == errno)
			{
				LOG_ERROR("read sokcet:%d error EPIPE", conn->GetSocketId());
				HandleClose(conn);
			}
			return false;
		}	
		else if(readCount == 0)
		{
			LOG_ERROR("read sokcet:%d readCount == 0", conn->GetSocketId());
			HandleClose(conn);
			return false;
		}
		else
		{
			if (EINTR == errno || EWOULDBLOCK == errno || EAGAIN == errno)
			{
				continue;
			}

			if(EINPROGRESS == errno)
			{
				LOG_ERROR("read sokcet:%d readCount:%d error EINPROGRESS", conn->GetSocketId(), readCount);
				continue;
			}
			
			if (EPIPE == errno)
			{
				LOG_ERROR("read sokcet:%d readCount:%d error EPIPE", conn->GetSocketId(), readCount);
				HandleClose(conn);
				return false;
			}
			LOG_ERROR("read handle errno:%d readCount:%d error EPIPE", (int)errno, readCount);
			HandleError(conn);
			return false;
		}
	}
	
	return true;
}

void ConnectionManager::HandleWrite(TcpConnection* conn)
{
	conn->HandleWrite();
}

}
