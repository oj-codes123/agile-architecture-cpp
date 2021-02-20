
#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace agile
{

typedef char BChar;
typedef uint32_t SizeInt;

#define BUFFER_WRITE_AND_READ(DataType) \
Buffer& operator << (DataType val)  \
{                                   \
    Write(&val, sizeof(DataType));  \
    return *this;                   \
}                                   \
Buffer& operator >> (DataType& val) \
{                                   \
    Read(&val, sizeof(DataType));   \
    return *this;                   \
}

class Buffer
{
public:
    Buffer();
    explicit Buffer(uint32_t size);
    virtual ~Buffer();

    void ReSize(uint32_t size);

    void ResetIndex();
	
    void SetWriteIndex(uint32_t wIndex);
    uint32_t GetWriteIndex();

    void SetReadIndex(uint32_t rIndex);
    uint32_t GetReadIndex();

    uint32_t GetReadAbleSize();
    uint32_t GetWriteAbleSize();
	
	const BChar* GetBuffer();
	uint32_t GetBufferSize();

    BChar* Begin();
    BChar* CurRead();
    BChar* CurWrite();

    bool WriteTo(const void* buffer, uint32_t len, uint32_t index);
    bool ReadTo(void* buffer, uint32_t len, uint32_t index);
	bool ReadToCur(void* buffer, uint32_t len);

    void Write(const void* buffer, uint32_t len);
    bool Read(void* buffer, uint32_t len);

    Buffer& operator << (const std::string& val);
    Buffer& operator >> (std::string& val);
    BUFFER_WRITE_AND_READ(bool)
    BUFFER_WRITE_AND_READ(int8_t)
    BUFFER_WRITE_AND_READ(uint8_t)
    BUFFER_WRITE_AND_READ(int16_t)
    BUFFER_WRITE_AND_READ(uint16_t)
    BUFFER_WRITE_AND_READ(int32_t)
    BUFFER_WRITE_AND_READ(uint32_t)
    BUFFER_WRITE_AND_READ(int64_t)
    BUFFER_WRITE_AND_READ(uint64_t)
    BUFFER_WRITE_AND_READ(float)
	
private:
	void Clear();
    bool ExtendSpace(uint32_t len);
	
private:
    uint32_t m_rIndex;
    uint32_t m_wIndex;
    std::vector<BChar> m_buffer;
};

}
