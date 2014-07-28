#pragma once
#include "Packet.h"
#include "CEGUI/CEGUI.h"

#include <winsock.h>

class UserDataPacket : public Packet
{
public:
	UserDataPacket(CEGUI::String text, CEGUI::argb_t color,  UserID senderID);
	UserDataPacket(char* inRawData, int dataSize, UserID senderID);
	~UserDataPacket();

	void Serialize(CEGUI::String text, CEGUI::argb_t color);
	void Deserialize(char* inRawData, int dataSize, CEGUI::String* outName, CEGUI::argb_t* outUserTextColor);
	void Deserialize(CEGUI::String * outName, CEGUI::argb_t * outUserTextColor);

private:
	//Declare but don't define. We don't want people to be able to construct an empty packet.
	UserDataPacket();
};

