#include "TextPacket.h"


TextPacket::TextPacket(CEGUI::String text, UserID senderID)
: Packet(STRINGDATA, text.size(), senderID)
{
	dataVector.resize(text.size());
	memcpy(dataVector.data(), text.data(), text.size());
}

TextPacket::TextPacket(const char* rawData, int dataSize, UserID senderID)
	: Packet(STRINGDATA, dataSize, senderID)
{
	dataVector.resize(dataSize);
	memcpy(dataVector.data(), rawData, dataSize);
}

TextPacket::~TextPacket()
{
}

void TextPacket::Serialize(CEGUI::String text)
{
	header.Serialize(STRINGDATA, text.size());
	dataVector.resize(text.size());
	memcpy(dataVector.data(), text.data(), text.size());
}

//Deserialize external data
CEGUI::String TextPacket::Deserialize(const char* inData, int dataSize)
{
	return std::move(CEGUI::String(inData, dataSize));
}

//Deserialize using internal data
CEGUI::String TextPacket::Deserialize()
{
	return std::move(CEGUI::String(dataVector.data(), dataVector.size()));
}