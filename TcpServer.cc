
#include "EpollLoop.h"
#include "Logger.h"
#include "TcpConnection.h"
#include "TcpServer.h"
#include "ConnectionManager.h"
#include "SignalHandler.h"

namespace agile
{
	
TcpServer::TcpServer()
{
	m_connMgr = nullptr;
}

TcpServer::~TcpServer()
{
	if(m_connMgr)
	{
		delete m_connMgr;
		m_connMgr = nullptr;
	}
}

bool TcpServer::Init(bool flag)
{
	if(m_connMgr)
	{
		return true;
	}
	
	m_connMgr = new ConnectionManager();
	((ConnectionManager*)m_connMgr)->Init(flag);
	
	SignalHandler::Instance().AddConnectionManager(m_connMgr);
	
	return true;
}

IConnectionManager* TcpServer::GetConnectionManager()
{
	return m_connMgr;
}

int TcpServer::AddListen(const std::string& ip, int port, int buffSize)
{
	if (!m_connMgr)
	{
		LOG_ERROR("had no init TcpServer");
		return -1;
	}
	
	ServerConnection* listenConn = new ServerConnection();
	int ret = listenConn->InitTcpListener(ip, port, buffSize);
	if (0 == ret)
	{
		if( ((ConnectionManager*)m_connMgr)->AddListenConnection(listenConn) )
		{
			LOG_INFO_S << "ip:" << ip << ", port:" << port << ", size:" << buffSize << " listen success";
		}
	}
	return ret;
}

void TcpServer::Start()
{
	if (!m_connMgr)
	{
		LOG_ERROR("had no init TcpServer");
		return;
	}
	
	((ConnectionManager*)m_connMgr)->SetConnectOpen(true);
	
	SignalHandler::Instance().Init();
	
	LOG_INFO("start server");
	
	EpollLoop::Instance().Run();
}

void TcpServer::Stop()
{
	if(!m_connMgr)
	{
		LOG_ERROR("had no init TcpServer");
		return;
	}
	
	delete m_connMgr;
	m_connMgr = nullptr;
	
	exit(0);
}

}
