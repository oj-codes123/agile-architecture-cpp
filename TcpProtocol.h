
#pragma once

#include "Buffer.h"

namespace agile
{

class TcpProtocol
{
public:
	TcpProtocol(){}
	
    virtual ~TcpProtocol(){}
	
	virtual void Encode(Buffer& buffer);
	
	virtual bool Decode(Buffer& buffer, bool& isFinish, uint32_t& dataLen);

};

}
