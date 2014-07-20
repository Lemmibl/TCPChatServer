#pragma once
#include "Packet.h"
#include "CEGUI/CEGUI.h"

class ColoredTextPacket : public Packet
{
public:
	ColoredTextPacket(CEGUI::String text, CEGUI::argb_t color,  UserID senderID);
	ColoredTextPacket(const char* inData, int dataSize, UserID senderID);
	~ColoredTextPacket();

	void Serialize(CEGUI::String text, CEGUI::argb_t color);
	void Deserialize(const char* inData, int dataSize, CEGUI::String *const outText, CEGUI::argb_t *const outColor);
	void Deserialize(CEGUI::String *const outText, CEGUI::argb_t *const outColor);

private:
	//Declare but don't define. We don't want people to be able to construct an empty packet.
	ColoredTextPacket();
};

