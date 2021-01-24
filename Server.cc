
#include "Agile.h"

using namespace agile;

class ClientDelegate : public ConnectionDelegate
{
public:
	ClientDelegate(){}
	virtual ~ClientDelegate(){}
	
	virtual void OnStart(Connection* conn)
	{
		LOG_INFO_S << " Delegate start socket:" << conn->GetSocketId() << ", name:" << conn->GetName();
	}
	
	virtual void OnStop(Connection* conn)
	{
		LOG_INFO_S << " Delegate stop socket:" << conn->GetSocketId() << ", name:" << conn->GetName();
	}
	
	virtual void OnMessage(Connection* conn)
	{
		std::string name;
		int score = 0;
		float val = 0;
		BufferNet& buffer = conn->GetBuffer();
		
		int size = 0;
		buffer >> size;
	    if(size == 4)
		{
			LOG_INFO(" [client] ping message");
			return;
		}
		buffer >> name >> score >> val;
		LOG_INFO(" [Delegate client] size:%d, name:%s, score:%d, val:%f", size, name.c_str(), score, val);	
	}
	
	//virtual void OnError(Connection* conn){}
	virtual bool OnRun(Connection* conn)
	{ 
		LOG_INFO(" [Delegate client] runing");
		return true; 
	}
};

class ClientDelegateFacotry : public ConnectionDelegateFacotry
{
public:
	ClientDelegateFacotry(){}
	virtual ~ClientDelegateFacotry(){}
	virtual ConnectionDelegate* CreateConnectionDelegate(){ return new ClientDelegate(); }
};

class ClientTest
{
public:
    static ClientTest& Instance(){ static ClientTest obj; return obj; }
    virtual ~ClientTest(){}
	
	void SendMsgTimer(uint32_t curTime)
	{
		std::string name = "oliver";
	    int score = (int)Utility::GetPid();
		float val = 33.33;
		
		Buffer sender;
		//Buffer buffer;
		sender << score;
		sender << name << score << val;
		
		LOG_INFO(" [client] send to server ");
		
		TcpClient::Instance().GetConnectionManager()->BroadcastConns("test_client", sender);
		
		Utility::AddSecTimer(5, std::bind(&ClientTest::SendMsgTimer, &ClientTest::Instance(), std::placeholders::_1));
	}
	
	int OnReceive(Connection* conn)
	{
		std::string name;
		int score = 0;
		float val = 0;
		BufferNet& buffer = conn->GetBuffer();
		
		int size = 0;
		buffer >> size;
		if(size == 4)
		{
			LOG_INFO(" [client] ping message");
			return 0;
		}
		buffer >> name >> score >> val;
		
		LOG_INFO(" [client] size:%d, name:%s, score:%d, val:%f", size, name.c_str(), score, val);
		return 0;
	}
	
	int OnClosed(Connection* conn)
	{
		LOG_INFO(" [client] on closed ");
		return 0;
	}
	
	int OnError(Connection* conn)
	{
		LOG_INFO(" [client] on error ");
		return 0;
	}
	
private:
	ClientTest(){}
};

class ServerDelegate : public ConnectionDelegate
{
public:
	ServerDelegate(){}
	virtual ~ServerDelegate(){}
	
	virtual void OnStart(Connection* conn)
	{
		LOG_INFO_S << " Delegate start socket:" << conn->GetSocketId() << ", name:" << conn->GetName();
	}
	
	virtual void OnStop(Connection* conn)
	{
		LOG_INFO_S << " Delegate stop socket:" << conn->GetSocketId() << ", name:" << conn->GetName();
	}
	
	virtual void OnMessage(Connection* conn)
	{
		std::string name;
		int score = 0;
		float val = 0;
		BufferNet& buffer = conn->GetBuffer();
		
		int size = 0;
		buffer >> size;
		if(size == 4)
		{
			LOG_INFO("[server] ping message");
			return;
		}
		buffer >> name >> score >> val;
		
		LOG_INFO(" [Delegate server] socketId:%d, size:%d, name:%s, score:%d, val:%f", conn->GetSocketId(), size, name.c_str(), score, val);	
		
		name = "server sender";
		score = 100;
		val = 100;
		
		Buffer sender;
		sender << score;
		sender << name << score << val;
		conn->Send(sender);
	}

	virtual bool OnRun(Connection* conn)
	{ 
		LOG_INFO(" [Delegate server] runing socketId:%d", conn->GetSocketId());
		return true; 
	}	
	//virtual void OnError(Connection* conn){}
	//virtual bool OnRun(Connection* conn){ return true; }
};

class ServerDelegateFacotry : public ConnectionDelegateFacotry
{
public:
	ServerDelegateFacotry(){}
	virtual ~ServerDelegateFacotry(){}
	virtual ConnectionDelegate* CreateConnectionDelegate(){ return new ServerDelegate(); }
};

class ServerTest
{
public:
    static ServerTest& Instance(){ static ServerTest obj; return obj; }
    virtual ~ServerTest(){}
	
	int OnNew(Connection* conn)
	{
		LOG_INFO("new name:%s", conn->GetPeerName().c_str() );
		return 0;
	}
	
	int OnReceive(Connection* conn)
	{
		std::string name;
		int score = 0;
		float val = 0;
		BufferNet& buffer = conn->GetBuffer();
		
		uint32_t size = 0;
		buffer >> size;
		if(size == 4)
		{
			LOG_INFO("[server] ping message");
			return 0;
		}
		buffer >> name >> score >> val;
		
		LOG_INFO(" [server] size:%d, name:%s, score:%d, val:%f", size, name.c_str(), score, val);
		
		name = "server sender";
		score = 100;
		val = 100;
		
		Buffer sender;
		sender << score;
		sender << name << score << val;
		conn->Send(sender);
		
		return 0;
	}
	
	int OnClosed(Connection* conn)
	{
		LOG_INFO(" [server] on closed ");
		return 0;
	}
	
	int OnError(Connection* conn)
	{
		LOG_INFO(" [server] on error ");
		return 0;
	}
	
	void OnStopSignalCallback(int val)
	{
		LOG_INFO(" [server] OnStopSignalCallback val:%d ", val);
	}
	
    void OnEventSignalCallback(int val)
	{
		LOG_INFO(" [server] OnEventSignalCallback val:%d ", val);
	}
	
private:
	ServerTest(){}
};

int main(int argc,char *argv[])
{
	if(argc > 1)
	{
		Logger::InitLog("test_client_log");
		
		TcpClient::Instance().Init(false);
		TcpClient::Instance().GetConnectionManager()->SetConnectionDelegateFacotry( new ClientDelegateFacotry() );
		TcpClient::Instance().GetConnectionManager()->SetReadCallback( std::bind(&ClientTest::OnReceive, &ClientTest::Instance(), std::placeholders::_1) );
		TcpClient::Instance().GetConnectionManager()->SetCloseCallback( std::bind(&ClientTest::OnClosed, &ClientTest::Instance(), std::placeholders::_1) );
		TcpClient::Instance().GetConnectionManager()->SetErrorCallback( std::bind(&ClientTest::OnError, &ClientTest::Instance(), std::placeholders::_1) );
		
		int port = atoi(argv[1]);
		if( 0 != TcpClient::Instance().Connect("test_client", "127.0.0.1", port) )
		{
			return -1;
		}
		
		TcpClient::Instance().GetConnectionManager()->SetConnectionRunTime(20000);
		TcpClient::Instance().GetConnectionManager()->SetConnectionPingTime(10);
		Utility::AddSecTimer(5, std::bind(&ClientTest::SendMsgTimer, &ClientTest::Instance(), std::placeholders::_1));
		
		TcpClient::Instance().Start();
	}
	else
	{
		Logger::InitLog("test_server_log");
		
		LOG_INFO("start server now %d ...", 3638, 3738);

		TcpServer server;
		server.Init(false);
		server.AddListen("INADDR_ANY", 3638);
		
		server.GetConnectionManager()->SetConnectionDelegateFacotry( new ServerDelegateFacotry() );
		server.GetConnectionManager()->SetNewCallback( std::bind(&ServerTest::OnNew, &ServerTest::Instance(), std::placeholders::_1) );
		server.GetConnectionManager()->SetReadCallback( std::bind(&ServerTest::OnReceive, &ServerTest::Instance(), std::placeholders::_1) );
		server.GetConnectionManager()->SetCloseCallback( std::bind(&ServerTest::OnClosed, &ServerTest::Instance(), std::placeholders::_1) );
		server.GetConnectionManager()->SetErrorCallback( std::bind(&ServerTest::OnError, &ServerTest::Instance(), std::placeholders::_1) );
		
		server.GetConnectionManager()->SetConnectionRunTime(20000);
		server.GetConnectionManager()->SetConnectionPingTime(20);
		
		TcpServer server2;
		server2.Init(false);
		server2.AddListen("INADDR_ANY", 3738);
		
		server2.GetConnectionManager()->SetConnectionDelegateFacotry( new ServerDelegateFacotry() );
		server2.GetConnectionManager()->SetNewCallback( std::bind(&ServerTest::OnNew, &ServerTest::Instance(), std::placeholders::_1) );
		server2.GetConnectionManager()->SetReadCallback( std::bind(&ServerTest::OnReceive, &ServerTest::Instance(), std::placeholders::_1) );
		server2.GetConnectionManager()->SetCloseCallback( std::bind(&ServerTest::OnClosed, &ServerTest::Instance(), std::placeholders::_1) );
		server2.GetConnectionManager()->SetErrorCallback( std::bind(&ServerTest::OnError, &ServerTest::Instance(), std::placeholders::_1) );
		
		server2.GetConnectionManager()->SetConnectionRunTime(20000);
		server2.GetConnectionManager()->SetConnectionPingTime(20);
		
		Utility::SetOnStopSignalCallback( std::bind(&ServerTest::OnStopSignalCallback, &ServerTest::Instance(), std::placeholders::_1) );
		Utility::SetOnEventSignalCallback( std::bind(&ServerTest::OnEventSignalCallback, &ServerTest::Instance(), std::placeholders::_1) );
		
		server.Start();
	}
    return 0;
}

