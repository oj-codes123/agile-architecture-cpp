
#pragma once

#include "IConnectionManager.h"

namespace agile
{
	
class TcpClient
{
public:
    static TcpClient& Instance(){ static TcpClient obj; return obj; }
    virtual ~TcpClient();
	
	bool Init(bool flag = true);

	void Start();

	void Stop();
	
	int Connect(const std::string& name, const std::string& ip, int port, uint32_t timeout=3, bool reConnect=true);
	
	int Connect(const std::string& name, const std::string& ip, int port, int buffSize, uint32_t timeout, bool reConnect);
	
	IConnectionManager* GetConnectionManager();

private:
	TcpClient();

private:
    IConnectionManager* m_connMgr;
};

}

