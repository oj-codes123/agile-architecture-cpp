
#include "Logger.h"
#include "TcpProtocol.h"

namespace agile
{

void TcpProtocol::Encode(Buffer& buffer)
{
	SizeInt packetSize = (SizeInt)buffer.GetWriteIndex();
	buffer.WriteTo(&packetSize, sizeof(SizeInt), 0);
}
	
bool TcpProtocol::Decode(Buffer& buffer, bool& isFinish, uint32_t& dataLen)
{
	uint32_t readIndex = buffer.GetReadIndex();
	if( buffer.ReadTo(&dataLen, sizeof(uint32_t), readIndex) )
	{
		uint32_t writeIndex = buffer.GetWriteIndex();
		if(writeIndex > readIndex)
		{
			isFinish = ( (writeIndex - readIndex) >= dataLen );
			return true;
		}
	}
	
	return false;
}

}
