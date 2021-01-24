
#pragma once

#include "IConnectionManager.h"

namespace agile
{
	
class TcpServer
{
public:
	TcpServer();

	virtual ~TcpServer();

	bool Init(bool flag = true);

	int AddListen(const std::string& ip, int port, int buffSize = 10240);

	void Start();

	void Stop();

	IConnectionManager* GetConnectionManager();

private:
    IConnectionManager* m_connMgr;
};

}
