
#include "BufferNet.h"
#include "Logger.h"
#include "AgileGlobal.h"

namespace agile
{

BufferNet::BufferNet()
{
	m_size = AgileGlobal::Instance().GetBufferComSize();
	if(m_size < 12)
	{
		m_size = 12;
	}
	m_buffer = new BChar[m_size];
}

BufferNet::BufferNet(uint32_t size)
{
	m_size = size < 12 ? 12 : size;
	m_buffer = new BChar[m_size];
}

BufferNet::~BufferNet()
{
	delete [] m_buffer;
}

void BufferNet::Clear()
{
	m_rIndex = 0;
	m_wIndex = 0;
}

void BufferNet::SetWriteIndex(uint32_t wIndex)
{ 
	m_wIndex = wIndex; 
}

uint32_t BufferNet::GetWriteIndex()
{ 
	return m_wIndex;
}

void BufferNet::SetReadIndex(uint32_t rIndex)
{ 
	m_rIndex = rIndex;
}

uint32_t BufferNet::GetReadIndex()
{
	return m_rIndex; 
}

void BufferNet::FinishReadPacket()
{
	if(m_rIndex == m_wIndex)
	{
		m_rIndex = 0;
		m_wIndex = 0;
	}
	else
	{
		uint32_t pSize = GetPacketSize();
		uint32_t index = 0;
		for(uint32_t i = pSize; i < m_wIndex; i++)
		{
			m_buffer[index++] = m_buffer[i];
		}
		m_rIndex = 0;
		m_wIndex = index;
	}
}

void BufferNet::IncrWriteIndex(uint32_t val)
{
	m_wIndex += val;
}

bool BufferNet::IsFullPacket()
{
	std::copy(m_buffer, m_buffer + sizeof(uint32_t), (BChar*)&m_packetSize);
	return m_packetSize <= m_size;
}

uint32_t BufferNet::GetPacketSize()
{ 
	std::copy(m_buffer, m_buffer + sizeof(uint32_t), (BChar*)&m_packetSize);
	return m_packetSize;
}

uint32_t BufferNet::GetBufferSize()
{
	return m_size;
}

uint32_t BufferNet::GetWriteAbleSize()
{
	return m_size > m_wIndex ? (m_size - m_wIndex) : 0;
}

BChar* BufferNet::GetBuffer()
{ 
	return m_buffer; 
}

BChar* BufferNet::CurRead()
{ 
	return m_buffer + m_rIndex; 
}

BChar* BufferNet::CurWrite()
{ 
	return m_buffer + m_wIndex;
}

BufferNet& BufferNet::operator >> (std::string& val)
{
    int32_t strLen = 0;
    Read(&strLen, sizeof(int32_t));

    BChar buffer[512];
    int32_t rLen = 0;
    while(strLen > 0)
    {
        rLen = strLen > 512 ? 512 : strLen;
        if( !Read(buffer, rLen) )
        {
            break;
        }
        val.append(buffer, rLen);
        strLen -= rLen;
    }
    return *this;
}

void BufferNet::Write(const void* buffer, uint32_t len)
{
	if (len > 0 && m_wIndex + len <= m_size )
	{		
		std::copy( (BChar*)buffer, (BChar*)buffer + len, m_buffer + m_wIndex);
		m_wIndex += len;
	}
}

bool BufferNet::Read(void* buffer, uint32_t len)
{
    if (len > 0 && m_rIndex + len <= m_wIndex)
    {
        std::copy(m_buffer + m_rIndex, m_buffer + m_rIndex + len, (BChar*)buffer );
        m_rIndex += len;
        return true;
    }
	
    return false;
}

}
