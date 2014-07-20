#pragma once
#include "../NetworkData.h"

class PacketHeader
{
public:
	PacketHeader(DataPacketType type, size_t dataSize);
	PacketHeader(const char* const data);
	~PacketHeader();

	static const size_t SizeOfStruct() { return SIZE; }

	void Serialize(DataPacketType dataType, int dataSize);
	void Deserialize(DataPacketType* outType, int* outSize);

	DataPacketType GetType();
	size_t GetSize();

	//Public on purpose
	char dataArray[4];

private:
	//Big ugly letters for a big ugly thing.
	static const size_t SIZE = sizeof(char) * 4; 

private:
	//Declare but don't define. We don't want people to be able to construct an empty packet.
	PacketHeader();

	size_t stepSize;
};

