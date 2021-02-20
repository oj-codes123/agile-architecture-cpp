
#pragma once

#include "Socket.h"
#include "Epoll.h"
#include "Connection.h"

namespace agile
{
	
class ConnectionManager;
enum ConnectionType
{
    ConnType_Server_Client = 0,
    ConnType_Server_Listen = 1,
    ConnType_Client_Connect = 2,
	ConnType_Client_ReConnect = 3,
    ConnType_End
};

enum ConnectionState
{
    TcpConnState_NULL = 0,
    TcpConnState_Open = 1,
    TcpConnState_Closing = 2,
    TcpConnState_Closed  = 3,
    TcpConnState_END
};

class TcpConnection : public Connection
{
public:
    TcpConnection();
	
    TcpConnection(int socketId, int buffSize);

	void SetInitValue(int socketId, int buffSize);
	
    virtual ~TcpConnection();
	
	virtual int GetSocketId(){ return m_socket.GetSocketId(); }
	
	virtual void SetName(const std::string& name);
	virtual const std::string& GetName() const { return m_name; }
	
	virtual std::string GetPeerName(int socketId=0);
	
	virtual int Send(Buffer& buff);
    virtual int Send(const char* buff, int buff_len);

	virtual BufferNet& GetBuffer(){ return m_buffer; }

    virtual void SetKey(const std::string& key);
    virtual const std::string& GetKey() const { return m_key; }
	
	virtual void SetIntKey(uint64_t key);
	virtual uint64_t GetIntKey() { return m_intKey; }
	
	virtual void SetMessageLimit(uint8_t secTime, uint8_t recvMsgNum){}
	
	virtual void Close();
	
	virtual IConnectionManager* GetConnectionManager() { return (IConnectionManager*)m_connMgr; }
	
	void Start();
	void Stop();
	void FreeConnect();
	
	bool Run();
	
	void Ping();
	
	void SetType(int8_t cType) {m_type = cType;}
    int8_t GetType() const { return m_type; }
	
	ConnectionDelegate* GetConnectionDelegate() { return m_delegate; }
	void SetDelegate(ConnectionDelegate* delegate) { m_delegate = delegate; }
	void SetConnectionManager(ConnectionManager* mgr) { m_connMgr = mgr; }
	
    void EnableReading()  { m_eventObj.events |= GEpollReadEvent;   UpdateEvent(); }
    void DisableReading() { m_eventObj.events &= ~GEpollReadEvent;  UpdateEvent(); }
    void EnableWriting()  { m_eventObj.events |= GEpollWriteEvent;  UpdateEvent(); }
    void DisableWriting() { m_eventObj.events &= ~GEpollWriteEvent; UpdateEvent(); }
    void DisableAll()     { m_eventObj.events =  GEpollNoneEvent;   UpdateEvent(); }

    bool IsNoneEvent() const { return m_eventObj.events == GEpollNoneEvent; }
    bool IsWriting()   const { return m_eventObj.events & GEpollWriteEvent; }
    bool IsReading()   const { return m_eventObj.events & GEpollReadEvent;  }

    void HandleWrite();

	void AddEvent();
	void UpdateEvent();
    void DeleteEvent();
	
	void HandleError();
    void HandleClose();
	virtual int HanldeRead();
	
protected:
	int SendCharsHelper(const char* buff, int traceIndex, int packetSize);
    void Write(const void* buffer, uint32_t len){ m_buffer.Write(buffer, len); }

protected:
    int8_t m_state = TcpConnState_NULL;
    int8_t m_type  = ConnType_Server_Client;
	
    Socket m_socket;

	std::string  m_name;
    std::string  m_key;
	int64_t      m_intKey = 0;
	
    Buffer       m_writeBuffer;
    BufferNet    m_buffer;

    epoll_event m_eventObj;
	
	ConnectionDelegate* m_delegate = nullptr;
	ConnectionManager*  m_connMgr  = nullptr;
};

class ServerConnection : public TcpConnection
{
public:
	ServerConnection() { m_type = ConnType_Server_Listen; m_buffSize = 10240; }
    virtual ~ServerConnection(){ }

    int InitTcpListener(const std::string& ip, int port, int buffSize=10240);

protected:
    int HanldeRead();

private:
	int m_buffSize;
};

class ClientConnection : public TcpConnection
{
public:
    ClientConnection() { m_type = ConnType_Client_ReConnect; m_buffSize = 10240; }
    virtual ~ClientConnection(){ }

    int Connect(const std::string& name, const std::string& ip, int port, uint32_t timeout, int buffSize=10240);

    int ReConnect();

    int GetPort() { return m_port; }
    const std::string& GetIp(){ return m_ip; }

private:
    int Connect();

private:
    int    m_buffSize;
    int    m_port;
	uint32_t m_timeout;
    std::string m_ip;
	std::string m_tName;
};

}