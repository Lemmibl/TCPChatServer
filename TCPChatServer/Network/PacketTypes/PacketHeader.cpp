#include "PacketHeader.h"
#include <Winsock2.h> //for htons, ntohs
#include <string>    // memcpy


PacketHeader::PacketHeader(DataPacketType type, size_t dataSize)
: stepSize(SIZE/2)
{
	//Reset everything
	memset(dataArray, 0, SIZE);

	Serialize(type, dataSize);
}

PacketHeader::PacketHeader(char* data )
: stepSize(SIZE/2)
{
	//Reset everything
	memset(dataArray, 0, SIZE);

	DataPacketType tempType;
	size_t tempSize;

	//Read DataPacketType from first two bytes
	memcpy(&tempType, data, stepSize);

	//Read the size of the next packet that we'll receive from the next two bytes.
	memcpy(&tempSize, data+stepSize, stepSize);

	Serialize(tempType, tempSize);
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
	memcpy(dataArray+stepSize, &tempSize, stepSize);
}

void PacketHeader::Deserialize( DataPacketType* outType, int* outSize )
{
	unsigned short tempType(0);
	unsigned short tempSize(0);

	//Read DataPacketType from first two bytes
	memcpy(&tempType, dataArray, stepSize);

	//Convert it using appropriate function.
	*outType = static_cast<DataPacketType>(tempType); //static_cast<DataPacketType>(ntohs(tempType));

	//Read the size of the next packet that we'll receive from the next two bytes.
	memcpy(&tempSize, dataArray+stepSize, stepSize);

	//Convert data using appropriate function.
	*outSize = tempSize; //ntohs(tempSize);
}

DataPacketType PacketHeader::GetType()
{
	unsigned short outType(0);
	memcpy(&outType, dataArray, stepSize);
	return static_cast<DataPacketType>(ntohs(outType));
}

size_t PacketHeader::GetSize()
{
	unsigned short outSize;
	memcpy(&outSize, dataArray+stepSize, stepSize);
	return ntohs(outSize);
}
