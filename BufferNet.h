
#pragma once
#include "Buffer.h"

namespace agile
{

#define BUFFER_READ_FUNCTION(DataType) \
BufferNet& operator >> (DataType& val) \
{                                   \
    Read(&val, sizeof(DataType));   \
    return *this;                   \
}                                 

class BufferNet
{
public:
    BufferNet();
    explicit BufferNet(uint32_t size);
    virtual ~BufferNet();
	
	void IncrWriteIndex(uint32_t val);
    void SetWriteIndex(uint32_t wIndex);
    uint32_t GetWriteIndex();

    void SetReadIndex(uint32_t rIndex);
    uint32_t GetReadIndex();
	
	uint32_t GetWriteAbleSize();
	
	BChar* GetBuffer();
	BChar* CurRead();
	BChar* CurWrite();
	
	uint32_t GetBufferSize();
	uint32_t GetPacketSize();//开始的四个字节
	bool IsFullPacket();
	
	void FinishReadPacket();

    void Write(const void* buffer, uint32_t len);
    bool Read(void* buffer, uint32_t len);

	BufferNet& operator >> (std::string& val);
	
    BUFFER_READ_FUNCTION(bool)
    BUFFER_READ_FUNCTION(int8_t)
    BUFFER_READ_FUNCTION(uint8_t)
    BUFFER_READ_FUNCTION(int16_t)
    BUFFER_READ_FUNCTION(uint16_t)
    BUFFER_READ_FUNCTION(int32_t)
    BUFFER_READ_FUNCTION(uint32_t)
    BUFFER_READ_FUNCTION(int64_t)
    BUFFER_READ_FUNCTION(uint64_t)
    BUFFER_READ_FUNCTION(float)
	
	void Clear();
	
private:
    uint32_t m_rIndex = 0;
    uint32_t m_wIndex = 0;
	uint32_t m_size = 0;
	uint32_t m_packetSize = 0;
    BChar*   m_buffer = nullptr;
};

}
