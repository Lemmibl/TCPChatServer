#include "PacketHeader.h"
#include <Winsock2.h> //for htons, ntohs
#include <string>    // memcpy


PacketHeader::PacketHeader(DataPacketType type, size_t dataSize)
: stepSize(sizeof(char) * 2)
{
	memset(dataArray, 0, SIZE);
	Serialize(type, dataSize);
}

PacketHeader::PacketHeader( const char* const data )
: stepSize(sizeof(char) * 2)
{
	//Store type in first two bytes
	memcpy(dataArray, data, stepSize);

	//Store size in the rest of the bytes
	memcpy(&dataArray+stepSize, &data+stepSize, stepSize);
}


PacketHeader::~PacketHeader()
{
}

void PacketHeader::Serialize( DataPacketType dataType, int dataSize )
{
	//Prepare data
	unsigned short tempType = htons(dataType);
	unsigned short tempSize = htons(dataSize);

	//Store type in first two bytes
	memcpy(dataArray, &tempType, stepSize);

	//Store size in the rest of the bytes
	memcpy(&dataArray+stepSize, &tempSize, stepSize);
}

void PacketHeader::Deserialize( DataPacketType* outType, int* outSize )
{
	//Read DataPacketType from first two bytes
	memcpy(outType, dataArray, stepSize);

	//Convert it using appropriate function.
	*outType = static_cast<DataPacketType>(ntohs(*outType));

	//Read the size of the next packet that we'll receive from the next two bytes.
	memcpy(outSize, dataArray+stepSize, stepSize);

	//Convert data using appropriate function.
	//Cast it up from ushort to uint. Not really needed but I use uints outside so might as well implicitly cast it.
	*outSize = ntohs(*outSize);
}

DataPacketType PacketHeader::GetType()
{
	DataPacketType outType;
	memcpy(&outType, dataArray, stepSize);
	return outType;
}

size_t PacketHeader::GetSize()
{
	size_t outSize;
	memcpy(&outSize, dataArray+stepSize, stepSize);
	return outSize;
}
