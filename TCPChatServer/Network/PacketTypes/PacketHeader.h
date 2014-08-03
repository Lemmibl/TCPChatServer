#pragma once
#include "../NetworkData.h"

class PacketHeader
{
private:
	//Big ugly letters for a big ugly thing.
	static const size_t SIZE = sizeof(char) * 4; 

public:
	//Normal, simple constructor. Serializes the parameters and store them in dataArray.
	PacketHeader(PacketType::Type type, size_t dataSize);

	//This is a ..... ReConstructor
	//Constructor that takes generic data that has been sent over the network and recreates it on the other side.
	PacketHeader(char* data);

	~PacketHeader();

	static const size_t SizeOfStruct() { return SIZE; }

	//Prepares data for sending over the network
	void Serialize(PacketType::Type dataType, int dataSize);

	//Converts internal data into the right shape and form and then assigns that data to outType/outSize
	void Deserialize(PacketType::Type* outType, int* outSize);

	PacketType::Type GetType();
	size_t GetSize();

	//Public on purpose
	char dataArray[SIZE];


private:
	//Declare but don't define. We don't want people to be able to construct an empty packet.
	PacketHeader();

	//Just a variable I reuse all over the place to save on typing and make it more consistent.
	//stepSize is always SIZE/2 because this class will always contain two values of equal size.
	size_t stepSize;
};

