#pragma once
#include "../NetworkData.h"
#include "PacketHeader.h"
#include <vector>

//Base packet for inheritance
class Packet
{
public:
	//Constructor for serializing the packet (when sending data packets)
	Packet(DataPacketType type, size_t dataSize, UserID id);

	//Constructor for deserializing the packet (when receiving data packets)
	Packet(PacketHeader headerData, UserID id);

	virtual ~Packet();

	const UserID GetSenderID();
	PacketHeader* const GetHeader();
	std::vector<char>& GetData();

	
	// All packets currently have three things in common:
protected:

	//They're all preceded by a header.	
	PacketHeader header; 
	
	//They will... pretty much all contain some form of data. If they don't, we just make an empty vector.
	std::vector<char> dataVector;

private:
	//They were all sent by someone with an ID... Hopefully.
	//If the ID is == 0 it means it's from the host/server.
	UserID userID;

private:
	//Declare but don't define. We don't want people to be able to construct an empty packet.
	Packet();
};

