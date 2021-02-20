
#include "Buffer.h"
#include "Logger.h"
#include "AgileGlobal.h"

namespace agile
{

Buffer::Buffer()
{
	m_rIndex = 0;
	m_wIndex = 0;
	m_buffer.resize(AgileGlobal::Instance().GetBufferComSize());
}

Buffer::Buffer(uint32_t size)
{
	m_rIndex = 0;
	m_wIndex = 0;
	m_buffer.resize(size);
}

void Buffer::ReSize(uint32_t size)
{
   	m_buffer.resize(size);
}

Buffer::~Buffer()
{
	Clear();
}

void Buffer::Clear()
{
	m_rIndex = 0;
	m_wIndex = 0;
	
	std::vector<BChar> emptyVal;
	m_buffer.swap(emptyVal);
	emptyVal.clear();
}

void Buffer::ResetIndex()
{
	m_rIndex = 0;
	m_wIndex = 0;
}

void Buffer::SetWriteIndex(uint32_t wIndex)
{ 
	m_wIndex = wIndex; 
}

uint32_t Buffer::GetWriteIndex()
{ 
	return m_wIndex;
}

void Buffer::SetReadIndex(uint32_t rIndex)
{ 
	m_rIndex = rIndex; 
}

uint32_t Buffer::GetReadIndex()
{
	return m_rIndex; 
}

uint32_t Buffer::GetReadAbleSize()
{ 
	return m_wIndex > m_rIndex ? m_wIndex - m_rIndex : 0; 
}

uint32_t Buffer::GetWriteAbleSize()
{ 
	return (uint32_t)m_buffer.size() > m_wIndex ? (uint32_t)m_buffer.size() - m_wIndex : 0; 
}

const BChar* Buffer::GetBuffer()
{ 
	return &*m_buffer.begin(); 
}

uint32_t Buffer::GetBufferSize()
{ 
	return m_buffer.size();
}

BChar* Buffer::Begin()
{ 
	return &*m_buffer.begin(); 
}

BChar* Buffer::CurRead()
{ 
	return &*m_buffer.begin() + m_rIndex; 
}

BChar* Buffer::CurWrite()
{ 
	return &*m_buffer.begin() + m_wIndex;
}
	
bool Buffer::WriteTo(const void* buffer, uint32_t len, uint32_t index)
{
    if(len > 0 && index + len < m_buffer.size() )
    {
		std::copy( (BChar*)buffer, (BChar*)buffer + len, &*m_buffer.begin() + index );
        return true;
    }
    return false;
}

bool Buffer::ReadTo(void* buffer, uint32_t len, uint32_t index)
{
    if ( len > 0 && index + len <= m_buffer.size() )
    {
        std::copy(&*m_buffer.begin() + index, &*m_buffer.begin() + index + len, (BChar*)buffer );
        return true;
    }
    return false;
}

bool Buffer::ReadToCur(void* buffer, uint32_t len)
{
    if ( len > 0 && len <= m_buffer.size() )
    {
        std::copy(&*m_buffer.begin() + m_rIndex, &*m_buffer.begin() + m_rIndex + len, (BChar*)buffer );
        return true;
    }
    return false;
}

Buffer& Buffer::operator << (const std::string& val)
{
    int32_t strLen = val.length();
    Write(&strLen, sizeof(int32_t));
    Write(val.c_str(), strLen);
    return *this;
}

Buffer& Buffer::operator >> (std::string& val)
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

bool Buffer::ExtendSpace(uint32_t len)
{
    uint32_t emptySize = m_buffer.size() - (m_wIndex - m_rIndex);
    if( emptySize < len )
    {
        uint32_t buffSize = m_buffer.size() + len - emptySize + 2;
        m_buffer.resize(buffSize);
    }
    std::copy(&*m_buffer.begin() + m_rIndex, &*m_buffer.begin() + m_wIndex, &*m_buffer.begin());
    m_wIndex -= m_rIndex;
    m_rIndex = 0;
    return true;
}

void Buffer::Write(const void* buffer, uint32_t len)
{
    if(len == 0){
        return;
    }

    if(m_wIndex + len >= m_buffer.size() ){
        if( !ExtendSpace(len) ){
            return;
        }
    }
    
	std::copy((BChar*)buffer, (BChar*)buffer + len, &*m_buffer.begin() + m_wIndex);
	m_wIndex += len;
}

bool Buffer::Read(void* buffer, uint32_t len)
{
    if (len > 0 && m_rIndex + len <= m_wIndex)
    {
        std::copy(&*m_buffer.begin() + m_rIndex, &*m_buffer.begin() + m_rIndex + len, (BChar*)buffer );
        m_rIndex += len;
        return true;
    }
    return false;
}

}
