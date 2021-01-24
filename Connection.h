
#pragma once

#include "BufferNet.h"

namespace agile
{
	
class ConnectionDelegate;
class IConnectionManager;
class Connection
{
public:
    Connection(){ }
    virtual ~Connection(){ }

	virtual int Send(Buffer& buff) = 0;

	virtual int Send(const char* buff, int buff_len) = 0;
	
	virtual BufferNet& GetBuffer() = 0;

	virtual int GetSocketId() = 0;
	
    virtual void SetKey(const std::string& key) = 0;
    virtual const std::string& GetKey() const = 0;
	
	virtual void SetIntKey(uint64_t key) = 0;
	virtual uint64_t GetIntKey() = 0;

	virtual void SetName(const std::string& name) = 0;
	virtual const std::string& GetName() const = 0;
	
	virtual std::string GetPeerName(int socketId=0) = 0;
	
	virtual void SetMessageLimit(uint8_t secTime, uint8_t recvMsgNum) = 0;
	
	virtual void Close() = 0;
	
	virtual IConnectionManager* GetConnectionManager() = 0;
};

class ConnectionDelegate
{
public:
	ConnectionDelegate(){}
	virtual ~ConnectionDelegate(){}
	
	virtual void OnStart(Connection* conn){}
	
	virtual void OnStop(Connection* conn){}
	
	virtual void OnMessage(Connection* conn){}
	
	virtual void OnError(Connection* conn){}
	
	virtual bool OnRun(Connection* conn){ return false; }
};

}

