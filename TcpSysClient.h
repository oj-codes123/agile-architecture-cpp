
#pragma once

#include "TcpProtocol.h"

namespace agile
{
class TcpSysClient
{
public:
    static TcpSysClient& Instance(){ static TcpSysClient obj; return obj; }
	
    virtual ~TcpSysClient();

	void SetTcpProtocol(TcpProtocol* protocol);
	TcpProtocol* GetTcpProtocol();
	
	int Send(const std::string& ip, uint32_t port, Buffer& sender, Buffer& receiver);	
	
private:
    TcpSysClient();
};

}
