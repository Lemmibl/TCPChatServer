#include "UserDataPacket.h"

UserDataPacket::UserDataPacket(CEGUI::String text, CEGUI::argb_t color,  UserID senderID)
	: Packet(USERDATA, sizeof(CEGUI::argb_t)+text.size(), senderID)
{
	Serialize(text, color);
}

UserDataPacket::UserDataPacket(const char* inData, int dataSize, UserID senderID)
	: Packet(USERDATA, dataSize, senderID)
{
	dataVector.resize(dataSize);
	memcpy(dataVector.data(), inData, dataSize);
}


UserDataPacket::~UserDataPacket()
{
}

void UserDataPacket::Serialize(CEGUI::String text, CEGUI::argb_t color)
{
	size_t textSize = text.size();
	size_t structSize = textSize+sizeof(color);

	//Setup header too...?
	//header.Serialize(USERDATA, structSize); //I dont think I've identified any case where I need to do this

	dataVector.resize(structSize);

	//First we copy raw text data into our vector
	memcpy(dataVector.data(),			text.data(),	textSize);

	//Then we fill the last four bytes with color data
	memcpy(dataVector.data()+textSize,	&color,			sizeof(CEGUI::argb_t));
}


void UserDataPacket::Deserialize( const char* inData, int dataSize, CEGUI::String *const outUserName, CEGUI::argb_t *const outUserTextColor )
{
	size_t colorSize = sizeof(CEGUI::argb_t);
	size_t textSize = dataSize-sizeof(CEGUI::argb_t);

	//Create a new string with data
	*outUserName = CEGUI::String(inData, textSize);

	//Fill color
	memcpy(outUserTextColor, inData+textSize, colorSize);
}

void UserDataPacket::Deserialize(CEGUI::String *const outUserName, CEGUI::argb_t *const outUserTextColor)
{
	size_t colorSize = sizeof(CEGUI::argb_t);
	size_t textSize = dataVector.size()-colorSize;

	//Create a new string with data
	*outUserName = CEGUI::String(dataVector.data(), textSize);

	//Fill color
	memcpy(outUserTextColor, dataVector.data()+textSize, colorSize);
}
