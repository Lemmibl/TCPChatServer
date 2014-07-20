#include "Packet.h"


Packet::Packet(DataPacketType type, size_t dataSize, UserID id)
	: header(type, dataSize),
	userID(id)
{
}

Packet::Packet(PacketHeader packetHeader, UserID id )
	: header(packetHeader),
	userID(id)
{
}

Packet::~Packet()
{
}

const UserID Packet::GetSenderID()
{
	return userID;
}

PacketHeader* const Packet::GetHeader()
{
	return &header;
}

std::vector<char>& Packet::GetData()
{
	return dataVector;
}
