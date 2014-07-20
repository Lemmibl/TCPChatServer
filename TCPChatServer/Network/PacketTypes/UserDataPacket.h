#pragma once
#include "Packet.h"
#include "CEGUI/CEGUI.h"

class UserDataPacket : public Packet
{
public:
	UserDataPacket(CEGUI::String text, CEGUI::argb_t color,  UserID senderID);
	UserDataPacket(const char* inRawData, int dataSize, UserID senderID);
	~UserDataPacket();

	void Serialize(CEGUI::String text, CEGUI::argb_t color);
	void Deserialize(const char* inRawData, int dataSize, CEGUI::String *const outName, CEGUI::argb_t *const outUserTextColor);
	void Deserialize(CEGUI::String *const outName, CEGUI::argb_t *const outUserTextColor);

private:
	//Declare but don't define. We don't want people to be able to construct an empty packet.
	UserDataPacket();
};

