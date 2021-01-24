
#include "Logger.h"
#include "EpollLoop.h"
#include "TcpClient.h"
#include "TcpConnection.h"
#include "ConnectionManager.h"
#include "SignalHandler.h"

namespace agile
{
	
TcpClient::TcpClient()
{
	m_connMgr = nullptr;
}

TcpClient::~TcpClient()
{
	if(m_connMgr)
	{
		delete m_connMgr;
		m_connMgr = nullptr;
	}
}

bool TcpClient::Init(bool flag)
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

IConnectionManager* TcpClient::GetConnectionManager()
{
	return m_connMgr;
}

int TcpClient::Connect(const std::string& name, const std::string& ip, int port, uint32_t timeout, bool reConnect)
{
	ClientConnection* conn = new ClientConnection();
	int ret = conn->Connect(name, ip, port, timeout);
	if (0 == ret)
	{
		if(reConnect)
		{
			conn->SetType(ConnType_Client_ReConnect);
		}
		else
		{
			conn->SetType(ConnType_Client_Connect);
		}
		((ConnectionManager*)m_connMgr)->AddConnection(conn);
	}
	else
	{
		delete conn;
		conn = nullptr;
	}
	return ret;
}

int TcpClient::Connect(const std::string& name, const std::string& ip, int port, int buffSize, uint32_t timeout, bool reConnect)
{
	ClientConnection* conn = new ClientConnection();
	int ret = conn->Connect(name, ip, port, timeout, buffSize);
	if (0 == ret)
	{
		if(reConnect)
		{
			conn->SetType(ConnType_Client_ReConnect);
		}
		else
		{
			conn->SetType(ConnType_Client_Connect);
		}
		
		((ConnectionManager*)m_connMgr)->AddConnection(conn);
	}
	else
	{
		delete conn;
		conn = nullptr;
	}
	return ret;	
}

void TcpClient::Start()
{
	if(!m_connMgr)
	{
		LOG_ERROR("had no init TcpClient");
		return;
	}
	
	((ConnectionManager*)m_connMgr)->SetConnectOpen(true);
	
	SignalHandler::Instance().Init();
	
	LOG_INFO("start client");
	EpollLoop::Instance().Run();
}

void TcpClient::Stop()
{
	if(!m_connMgr)
	{
		LOG_ERROR("had no init TcpClient");
		return;
	}
	
	delete m_connMgr;
	m_connMgr = nullptr;
	
	exit(3);
}

}
