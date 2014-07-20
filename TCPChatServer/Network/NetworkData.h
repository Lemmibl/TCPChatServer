#pragma once
static const int MAX_PACKET_SIZE = 1024;
typedef unsigned int UserID;
typedef unsigned int Color;

//8 bits. TODO: Get it down to 2 bits?
enum UserPermission : unsigned char
{
	Restricted = 0,
	Normal = 1,
	Admin = 2,
	SuperAdmin = 3 //AKA Host.
};

enum DataPacketType
{
	NODATA = 0,
	STRINGDATA,
	COLOREDSTRINGDATA,
	FLOATDATA,
	USERDATA,
	CONNECT,
	DISCONNECT
};


////This datapacket is supposed to be a precursor to sending the real data.
////It will detail the data type and size of the incoming packet
//class DataPacketHeader
//{
//private:
//	
//	static const size_t SIZE = sizeof(char) * 4; 
//
//public:
//	//public on purpose
//	char dataArray[4];
//
//	static const size_t SizeOfStruct() { return SIZE; }
//
//	DataPacketHeader()
//	{
//		std::memset(dataArray, 0, sizeof(dataArray));
//	}
//
//	DataPacketHeader(DataPacketType dataType, int dataSize)
//	{
//		std::memset(dataArray, 0, SIZE);
//		Serialize(dataType, dataSize);
//	}
//
//	void Serialize(DataPacketType dataType, int dataSize) 
//	{
//		//Prepare data
//		unsigned short tempType = htons(dataType);
//		unsigned short tempSize = htons(dataSize);
//
//		//Store type in first two bytes
//		memcpy(dataArray,	&tempType, 2);
//
//		//Store size in the rest of the bytes
//		memcpy(&dataArray+2, &tempSize, 2);
//	}
//
//	void Deserialize(DataPacketType* outType, int* outSize) 
//	{
//		//Read DataPacketType from first two bytes
//		memcpy(outType, dataArray+0, 2);
//
//		//Convert it using appropriate function.
//		*outType = static_cast<DataPacketType>(ntohs(*outType));
//
//		//Read the size of the next packet that we'll receive from the next two bytes.
//		memcpy(outSize, dataArray+2, 2);
//
//		//Convert data using appropriate function.
//		//Cast it up from ushort to uint. Not really needed but I use uints outside so might as well implicitly cast it.
//		*outSize = ntohs(*outSize);
//	}
//
//	DataPacketType GetType() 
//	{
//		DataPacketType outType;
//		memcpy(&outType, dataArray, 2);
//		return outType;
//	}
//
//	int GetSize() 
//	{
//		int outSize;
//		memcpy(&outSize, dataArray+2, 2);
//		return outSize;
//	}
//};
//
////Struct to contain the actual data that will be sent after a DataPacketHeader
////If I only had packets with a static amount of data, I would use this as a polymorphic base
//// And just overload different sorts of packets out of this, and each of the overloaded packets would just be a char array of varying sizes depending on the data.
////This way, I could treat packet bodies the same was as I treat headers, with a serialize and deserialize function
////The drawback with that approach is that each packet would have a vptr, so that's 4 bytes extra for each packet. That can quickly add up.
////Right now I prefer to just keep a separate function for each sort of packet.
//struct DataPacketBody
//{
//	DataPacketBody(size_t size = 0)
//		: dataVector(size)
//	{
//	}
//
//	size_t GetSize() { return dataVector.size(); }
//
//	//Might as well keep it in a vector. Also contains size of the data that we're going to send, through size().
//	std::vector<char> dataVector;
//};

////Purely organizational
//struct DataPacket
//{
//	DataPacket(UserID id = 0, int size = 0, DataPacketType type = NODATA)
//	: 	id(id),
//		header(type, size)
//	{
//		body.dataVector.resize(size);
//	}
//
//	DataPacketHeader header;
//	DataPacketBody body;
//
//	//ID of who sent this packet
//	UserID id;
//};