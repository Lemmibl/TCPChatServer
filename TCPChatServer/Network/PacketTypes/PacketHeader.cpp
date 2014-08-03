#include "PacketHeader.h"
#include <Winsock2.h> //for htons, ntohs
#include <string>    // memcpy


PacketHeader::PacketHeader(PacketType::Type type, size_t dataSize)
: stepSize(SIZE/2)
{
	//Reset everything
	memset(dataArray, 0, SIZE);

	Serialize(type, dataSize);
}

PacketHeader::PacketHeader(char* data)
: stepSize(SIZE/2)
{
	//Reset everything
	memset(dataArray, 0, SIZE);

	PacketType::Type tempType;
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

void PacketHeader::Serialize(PacketType::Type dataType, int dataSize )
{
	//Prepare data
	unsigned short tempType = htons(dataType);
	unsigned short tempSize = htons(dataSize);

	//Store type in first two bytes
	memcpy(dataArray, &tempType, stepSize);

	//Store size in the rest of the bytes
	memcpy(dataArray+stepSize, &tempSize, stepSize);
}

void PacketHeader::Deserialize(PacketType::Type* outType, int* outSize)
{
	unsigned short tempType(0);
	unsigned short tempSize(0);

	//Read DataPacketType from first two bytes
	memcpy(&tempType, dataArray, stepSize);

	//Convert it using appropriate function.
	*outType = static_cast<PacketType::Type>(tempType); //static_cast<DataPacketType>(ntohs(tempType));

	//Read the size of the next packet that we'll receive from the next two bytes.
	memcpy(&tempSize, dataArray+stepSize, stepSize);

	//Convert data using appropriate function.
	*outSize = tempSize; //ntohs(tempSize);
}

PacketType::Type PacketHeader::GetType()
{
	unsigned short outType(0);
	memcpy(&outType, dataArray, stepSize);
	
	//Put it back in proper order first. I've extracted the ntohs call to a separate row for easier debugging.
	outType = ntohs(outType);

	return static_cast<PacketType::Type>(outType);
}

size_t PacketHeader::GetSize()
{
	unsigned short outSize;
	memcpy(&outSize, dataArray+stepSize, stepSize);

	outSize = ntohs(outSize);

	return outSize;
}
