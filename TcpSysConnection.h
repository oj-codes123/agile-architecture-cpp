
#pragma once

#include "Socket.h"

namespace agile
{

class TcpSysConnection
{
public:
    TcpSysConnection();

    ~TcpSysConnection();
	
	int Connect(const std::string& ip, uint32_t port);

	int Send(Buffer& sender, Buffer& receiver);
	
	void Close();

private:
	int ReceiveBuffer(Buffer& receiver);

	int SendBuffer(Buffer& buff);

	int SendBufferHelper(Buffer& buff, int traceIndex, SizeInt packetSize);

private:
    Socket       m_socket;
	std::string  m_ip;
	uint32_t     m_port;
	
};

}
