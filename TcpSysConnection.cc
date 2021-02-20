
#include "Logger.h"
#include "AgileGlobal.h"
#include "Utility.h"
#include "TcpSysClient.h"
#include "TcpSysConnection.h"

namespace agile
{

TcpSysConnection::TcpSysConnection()
{

}

TcpSysConnection::~TcpSysConnection()
{
	
}

int TcpSysConnection::SendBuffer(Buffer& buff)
{
	TcpProtocol* protocol = TcpSysClient::Instance().GetTcpProtocol();
	if(protocol)
	{
		protocol->Encode(buff);
	}
	
	SizeInt packetSize = (SizeInt)buff.GetWriteIndex();

	int traceIndex = 0;
	int ret = SendBufferHelper(buff, traceIndex, packetSize);

	traceIndex += ret;
	packetSize -= (SizeInt)ret;

	int warn_count = 10;
	while (0 < (int)packetSize && ret > 0)
	{
		ret = SendBufferHelper(buff, traceIndex, packetSize);
		traceIndex += ret;
		packetSize -= (SizeInt)ret;
		
		if(ret == 0 && warn_count > 0) { ret = 1; --warn_count; }
	}
	return ret > 0 ? packetSize : -1;
}

int TcpSysConnection::SendBufferHelper(Buffer& buff, int traceIndex, SizeInt packetSize)
{
	int ret = send(m_socket.GetSocketId(), buff.GetBuffer() + traceIndex, packetSize, 0);
	if(ret > 0)
	{
		return ret;
	}
	else if (0 == ret)
	{
		LOG_ERROR("Send to socket_id:%d  close ret:%d", m_socket.GetSocketId(), ret);
		return -1;
	}
	else
	{
		if (EINTR == errno || EWOULDBLOCK == errno || EAGAIN == errno)
		{
			//LOG_WARN(" errno:%d", (int)errno);
			return 0;
		}
		else
		{
			if (EPIPE == errno)
			{
				LOG_ERROR("Send close errno:EPIPE");
				return -1;
			}
			else
			{
				LOG_ERROR("Send err ret:%d close now", errno);
				return -1;
			}
		}
	}
}

int TcpSysConnection::Send(Buffer& sender, Buffer& receiver)
{
	int ret = SendBuffer(sender);
	if( 0 != ret )
	{
		LOG_ERROR_S << "fail to send data to ip:" << m_ip << " port:" << m_port << " ret:" << ret;
		return ret;
	}

	ret = ReceiveBuffer(receiver);
	if(ret == 100)
	{
		receiver.SetWriteIndex(0);
		receiver.SetReadIndex(0);
		ret = ReceiveBuffer(receiver);
	}
	return ret == 100 ? 0 : ret;
}

int TcpSysConnection::ReceiveBuffer(Buffer& receiver)
{
	fd_set readfds;
   
	int sockfd = m_socket.GetSocketId();
	FD_ZERO( &readfds );
	FD_SET( sockfd, &readfds );

	struct timeval timeout;
	timeout.tv_sec = AgileGlobal::Instance().sys_client_receive_timeout / 2;
	timeout.tv_usec = 0;

	uint64_t startMTime = Utility::CurMSec();
	uint64_t timeoutVal = AgileGlobal::Instance().sys_client_receive_timeout * 1000;

	TcpProtocol* protocol = TcpSysClient::Instance().GetTcpProtocol();
	
	while(true)
	{
		int ret_int = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
		switch (ret_int)
		{
			case -1:
			{
				LOG_ERROR_S << " ip:" << m_ip << " port:" << m_port << " closed ";
				return -1;
			}
			break;

			case 0:
			{
				//LOG_INFO_S << " ip:" << m_ip << " port:" << m_port << " case 0 ";
			}
			break;

			default:
			{
				if(ret_int < 0)
				{
					LOG_ERROR_S << " ip:" << m_ip << " port:" << m_port << " select ret_int:" << ret_int;
					return -2;
				}

				if ( FD_ISSET(sockfd, &readfds) )
				{
					static char buff[10240];
					static int buffSize = 10240;
					while(true)
					{	
						int readCount = read(m_socket.GetSocketId(), buff, buffSize);
						if( readCount > 0 )
						{
							receiver.Write(buff, readCount);
							if( readCount >= buffSize)
							{
								continue;
							}
							
							if(!protocol)
							{
								return 0;
							}
							
							uint32_t dataLen = 0;
							bool isFinish = false;
							if( !protocol->Decode(receiver, isFinish, dataLen) )
							{
								LOG_ERROR_S << "protocol decode error";
								return -1;
							}
							
							if(isFinish)
							{
								return dataLen == 4 ? 100 : 0;
							}
							else
							{
								continue;
							}
						}
						else if(readCount == 0)
						{
							LOG_ERROR_S << " ip:" << m_ip << " port:" << m_port << " closed ";
							return -3;
						}
						else
						{
							if ( (EAGAIN == errno) || (EWOULDBLOCK == errno) || (EINTR == errno) )
							{
								continue;
							}
							
							if (EPIPE == errno)
							{
								LOG_ERROR("readCount:%d socket:%d ip:%s port:%u error EPIPE", readCount, m_socket.GetSocketId(), m_ip.c_str(), m_port);
								return -4;
							}
							
							LOG_ERROR_S << " ip:" << m_ip << " port:" << m_port << " error readCount:" << readCount;
							return -5;
						}
					}
				}
			}
		}

		if( startMTime + timeoutVal < Utility::CurMSec() )
		{	
			LOG_ERROR_S << " ip:" << m_ip << " port:" << m_port << " read timeout ";
			return -6;
		}
	}
	return -7;
}

void TcpSysConnection::Close()
{
	m_socket.Close();
}

int TcpSysConnection::Connect(const std::string& ip, uint32_t port)
{
	m_ip   = ip;
	m_port = port;

	m_socket.Init();
	int ret = m_socket.SetNoBlocking(true);
	if( 0 != ret )
	{
		LOG_ERROR_S << " fail to connect " << m_ip << ":" << m_port <<" SetNoBlocking ret:" << ret;
		return ret;
	}
  
	bool isConnecting = false;
	ret = m_socket.ConnectTo(m_ip, m_port);
	if( 0 != ret )
	{
		if ( errno == EINPROGRESS && ret == -1 )
		{
			isConnecting = true;
		}
		else
		{
			LOG_ERROR_S << "fail to connect " << m_ip << ":" << m_port <<" ret:" << ret << " errno:" << (int)errno;
			return ret;
		}
	}
	
	if(isConnecting)
	{
		LOG_INFO_S << "connecting " << m_ip << ":" << m_port << " ...";
	  
		fd_set readfds;
		fd_set writefds;
      
		int sockfd = m_socket.GetSocketId();
		FD_ZERO( &readfds );
		FD_SET( sockfd, &writefds );
 
		struct timeval timeout;
		timeout.tv_sec  = AgileGlobal::Instance().sys_client_connect_timeout;
		timeout.tv_usec = 0;
 
		ret = select( sockfd + 1, NULL, &writefds, NULL, &timeout );
		if ( ret <= 0 )
		{
			LOG_ERROR_S << "connect fail " << m_ip << ":" << m_port << " closed ";
			m_socket.Close();
			return ret;
		}
 
		if ( !FD_ISSET( sockfd, &writefds ) )
		{
			LOG_ERROR_S << "connect fail " << m_ip << ":" << m_port << " no events ";
			m_socket.Close();
			return -1;
		}
 
		int errorNum = 0;
		socklen_t length = sizeof( errorNum );
		if( getsockopt( sockfd, SOL_SOCKET, SO_ERROR, &errorNum, &length ) < 0 )
		{
			LOG_ERROR_S << "connect fail " << m_ip << ":" << m_port << " get socket option failed";
			m_socket.Close();
			return -1;
		}
 
		if( errorNum != 0 )
		{
			LOG_ERROR_S << "connect fail " << m_ip << ":" << m_port << " errorNum:" << errorNum;
			m_socket.Close();
			return -1;
		}
	}
  
	m_socket.CommonSetWithoutSetNoBlocking(AgileGlobal::Instance().sys_client_buffer_size);
	LOG_INFO_S << "connect " << m_ip << ":" << m_port << " successfully";
	return 0;
}

}

