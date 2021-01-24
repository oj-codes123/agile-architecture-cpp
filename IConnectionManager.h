
#pragma once

#include <functional>
#include "Connection.h"

namespace agile
{
class ConnectionDelegateFacotry
{
public:
	ConnectionDelegateFacotry(){}
	virtual ~ConnectionDelegateFacotry(){}
	virtual ConnectionDelegate* CreateConnectionDelegate() = 0;
};

class IConnectionManager
{
public:
    IConnectionManager(){ }
    virtual ~IConnectionManager(){ }

    virtual void CloseConnectionBySocketId(int socketId) = 0;
    virtual void CloseConnectionByIntKey(uint64_t intKey) = 0;
	virtual void CloseConnectionByStrKey(const std::string& strKey) = 0;
	
	virtual bool GetSokcetIdsByName(const std::string& name, std::vector<int>& socketIds) = 0;
	virtual Connection* GetConnectionBySocketId(int socketId) = 0;
	virtual Connection* GetConnectionByIntKey(uint64_t intKey) = 0;
	virtual Connection* GetConnectionByStrKey(const std::string& strKey) = 0;
	virtual Connection* GetConnectionByName(const std::string& name) = 0;
	
	virtual void SendBufferBySocketId(int socketId, Buffer& buff) = 0;
    virtual void SendBufferByIntKey(uint64_t intKey, Buffer& buff) = 0;
	virtual void SendBufferByStrKey(const std::string& strKey, Buffer& buff) = 0;
	virtual void SendBufferByName(const std::string& name, Buffer& buff) = 0;
	
	virtual void BroadcastConns(Buffer& buff) = 0;
	virtual void BroadcastConns(const std::string& name, Buffer& buff) = 0;

    virtual size_t GetConnsSize() = 0;
    virtual size_t GetConnsSize(const std::string& name) = 0;
	
    virtual void SetNewCallback(std::function<int(Connection*)> func) = 0;
    virtual void SetReadCallback(std::function<int(Connection*)> func) = 0;
    virtual void SetWriteCallback(std::function<int(Connection*)> func) = 0;
    virtual void SetCloseCallback(std::function<int(Connection*)> func) = 0;
    virtual void SetErrorCallback(std::function<int(Connection*)> func) = 0;
	
	virtual void SetConnectionRunTime(uint64_t msec) = 0;
	virtual void SetConnectionPingTime(uint32_t sec) = 0;

	virtual void AddMaskIp(const std::string& ip, uint64_t mTime) = 0;
	
	virtual void SetConnectionDelegateFacotry(ConnectionDelegateFacotry* factory) = 0;
};

}
