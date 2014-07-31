#include "TextPacket.h"


TextPacket::TextPacket(CEGUI::String text, UserID senderID)
: Packet(PacketType::STRING, text.size(), senderID)
{
	dataVector.resize(text.size());
	memcpy(dataVector.data(), text.data(), text.size());
}

TextPacket::TextPacket(const char* rawData, int dataSize, UserID senderID)
	: Packet(PacketType::STRING, dataSize, senderID)
{
	dataVector.resize(dataSize);
	memcpy(dataVector.data(), rawData, dataSize);
}

TextPacket::~TextPacket()
{
}

void TextPacket::Serialize(CEGUI::String text)
{
	//Setup the header
	header.Serialize(PacketType::STRING, text.size());

	//Adjust and fill our internal data vector
	dataVector.resize(text.size());
	memcpy(dataVector.data(), text.data(), text.size());
}

//Deserialize external data
CEGUI::String TextPacket::Deserialize(const char* inData, int dataSize)
{
	return CEGUI::String((const CEGUI::utf8*)inData, dataSize);
}

//Deserialize using internal data
CEGUI::String TextPacket::Deserialize()
{
	return CEGUI::String((const CEGUI::utf8*)dataVector.data(), dataVector.size());
}