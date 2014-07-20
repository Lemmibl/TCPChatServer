#pragma once
#include "Packet.h"
#include "CEGUI/CEGUI.h"

class TextPacket : public Packet
{
public:
	TextPacket(CEGUI::String text, UserID senderID);
	TextPacket(const char* rawData, int dataSize, UserID senderID);
	~TextPacket();

	void Serialize(CEGUI::String text);
	CEGUI::String Deserialize(const char* inData, int dataSize);
	CEGUI::String Deserialize();

private:
	//Declare but don't define. We don't want people to be able to construct an empty packet.
	TextPacket();
};

