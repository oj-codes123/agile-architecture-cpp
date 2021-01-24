#include "Logger.h"

#include "AgileGlobal.h"
#include "TcpSysClient.h"
#include "TcpSysConnection.h"
#include <map>

namespace agile
{

TcpSysClient::TcpSysClient()
{
	
}

TcpSysClient::~TcpSysClient()
{
	
}

static std::map<std::string, TcpSysConnection> global_sys_tcps;

static TcpProtocol* global_sys_client_protocol = nullptr;

void TcpSysClient::SetTcpProtocol(TcpProtocol* protocol)
{
	global_sys_client_protocol = protocol;
}

TcpProtocol* TcpSysClient::GetTcpProtocol()
{
	return global_sys_client_protocol;
}

int TcpSysClient::Send(const std::string& ip, uint32_t port, Buffer& sender, Buffer& receiver)
{
	std::stringstream ss;
	ss << ip << ":" << port;
	
	auto it = global_sys_tcps.find( ss.str() );
	if( it != global_sys_tcps.end() )
	{
		int ret_int = it->second.Send(sender, receiver);
		if( 0 == ret_int )
		{
			return 0;
		}
		it->second.Close();
		global_sys_tcps.erase(it);
	}

	TcpSysConnection tcpConn;
	int ret = tcpConn.Connect(ip, port);
	if( 0 != ret )
	{
		LOG_ERROR("fail to connect");
		return ret;
	}
	
	ret = tcpConn.Send(sender, receiver);
	if(0 == ret)
	{
		global_sys_tcps[ ss.str() ] = tcpConn;
	}
	else
	{
		LOG_ERROR("fail to send");
		tcpConn.Close();
	}
	
	return ret;
}

}
