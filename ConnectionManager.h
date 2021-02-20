
#pragma once
#include <deque>
#include <map>
#include <unordered_map>
#include "IConnectionManager.h"

namespace agile
{
	
class NetEpoll;
class TcpConnection;
class ServerConnection;
class ClientConnection;

typedef std::function<int(Connection*)> FunctionComm;

struct ConnectionFunctions
{
    FunctionComm newCallback   = nullptr;
    FunctionComm readCallback  = nullptr;
    FunctionComm writeCallback = nullptr;
    FunctionComm closeCallback = nullptr;
    FunctionComm errorCallback = nullptr;
};

class ConnectionManager : public IConnectionManager
{
public:
    ConnectionManager();
    virtual ~ConnectionManager();

    virtual void CloseConnectionBySocketId(int socketId);
    virtual void CloseConnectionByIntKey(uint64_t intKey);
	virtual void CloseConnectionByStrKey(const std::string& strKey);
	
	virtual bool GetSokcetIdsByName(const std::string& name, std::vector<int>& socketIds);
	virtual Connection* GetConnectionBySocketId(int socketId);
	virtual Connection* GetConnectionByIntKey(uint64_t intKey);
	virtual Connection* GetConnectionByStrKey(const std::string& strKey);
	virtual Connection* GetConnectionByName(const std::string& name);

	virtual void SendBufferBySocketId(int socketId, Buffer& buff);
    virtual void SendBufferByIntKey(uint64_t intKey, Buffer& buff);
	virtual void SendBufferByStrKey(const std::string& strKey, Buffer& buff);
	virtual void SendBufferByName(const std::string& name, Buffer& buff);
	
	virtual void BroadcastConns(Buffer& buff);
	virtual void BroadcastConns(const std::string& name, Buffer& buff);

    virtual size_t GetConnsSize() { return m_conns.size(); }
    virtual size_t GetConnsSize(const std::string& name);
	
    virtual void SetNewCallback(std::function<int(Connection*)> func)  { m_connFuncs.newCallback   = func; }
    virtual void SetReadCallback(std::function<int(Connection*)> func) { m_connFuncs.readCallback  = func; }
    virtual void SetWriteCallback(std::function<int(Connection*)> func){ m_connFuncs.writeCallback = func; }
    virtual void SetCloseCallback(std::function<int(Connection*)> func){ m_connFuncs.closeCallback = func; }
    virtual void SetErrorCallback(std::function<int(Connection*)> func){ m_connFuncs.errorCallback = func; }

	virtual void SetConnectionRunTime(uint64_t msec);
	virtual void SetConnectionPingTime(uint32_t sec);
	
	virtual void AddMaskIp(const std::string& ip, uint64_t mTime){ m_maskIps[ip] = mTime; }
	
	virtual void SetConnectionDelegateFacotry(ConnectionDelegateFacotry* factory);
	
	void SetConnectOpen(bool isOpen){ m_conOpened = isOpen; }
	
	bool AddConnection(TcpConnection* conn);
	
	bool AddListenConnection(TcpConnection* conn);

    void OnNewConnection(int socketId, int buffSize);

    void HandleClose(TcpConnection* conn);
	void HandleError(TcpConnection* conn);
	void HandleWrite(TcpConnection* conn);
	bool HanldeRead(TcpConnection* conn);
	
	void HandleEvents(int iEvents, TcpConnection* conn);
	
	NetEpoll* GetEpoll(){ return m_epoll; }
	
	bool Init(bool isShared);
	
	void OnSetConnectionIntKey(TcpConnection* conn, uint64_t intKey);
	void OnSetConnectionStrKey(TcpConnection* conn, const std::string& strKey);
	void OnSetConnectionName(TcpConnection* conn, const std::string& name);
	
	void FreeConns();
	
private:
    void CheckClosingConns(uint32_t curTime);

    void CheckConnectingConns(uint32_t curTime);
	
	void DeleteTcpConnection(TcpConnection* conn);
	
	void AddRunConnection(TcpConnection* conn);
	void DeleteRunConnection(int socketId);
	void RunConnections(uint64_t curMSecTime);
	
	void PingConnections(uint32_t curSecTime);
	
private:
	struct NameSocketData
	{
		int index;
		std::vector<int> sockets;
		NameSocketData():index(0){}
		int GetSocket(){
			if(sockets.empty()){return -1;}
			if(index >= (int)sockets.size()){index=0;}
			return sockets[index++];
		}
	};
	
private:
    bool m_conOpened;
	uint64_t m_runMsec;
	uint64_t m_runMsecTrace;
	uint64_t m_mgrId;
	
	uint64_t m_runTimeMsec = 0;
	uint32_t m_pingTimeSec = 0;

    NetEpoll* m_epoll = nullptr;
	
	ConnectionDelegateFacotry* m_factory = nullptr;
	
	std::map<int, TcpConnection*> m_listenConns;
	
	std::unordered_map<int, TcpConnection*> m_runConns;
	
	std::unordered_map<int, TcpConnection*> m_conns;
	
	std::unordered_map<std::string, NameSocketData> m_nameSockets;
	
	std::unordered_map<uint64_t, int> m_intKeySokcets;
	std::unordered_map<std::string, int> m_strKeySockets;
	
	std::map<std::string, uint64_t> m_maskIps;
	
	std::deque<TcpConnection*> m_closeing;
	
	std::deque<TcpConnection*> m_closeingReconnect;

    ConnectionFunctions m_connFuncs;
};

}

