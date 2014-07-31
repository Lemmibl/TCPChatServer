#pragma once
#include "../NetworkData.h"

class PacketHeader
{
private:
	//Big ugly letters for a big ugly thing.
	static const size_t SIZE = sizeof(char) * 4; 

public:
	PacketHeader(PacketType::Type type, size_t dataSize);
	PacketHeader(char* data);
	~PacketHeader();

	static const size_t SizeOfStruct() { return SIZE; }

	void Serialize(PacketType::Type dataType, int dataSize);
	void Deserialize(PacketType::Type* outType, int* outSize);

	PacketType::Type GetType();
	size_t GetSize();

	//Public on purpose
	char dataArray[SIZE];


private:
	//Declare but don't define. We don't want people to be able to construct an empty packet.
	PacketHeader();

	size_t stepSize;
};

