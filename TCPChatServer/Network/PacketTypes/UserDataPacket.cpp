#include "UserDataPacket.h"

UserDataPacket::UserDataPacket(CEGUI::String text, CEGUI::argb_t color,  UserID senderID)
	: Packet(USERDATA, (sizeof(color)+text.size()), senderID)
{
	Serialize(text, color);
}

UserDataPacket::UserDataPacket(char* inData, int dataSize, UserID senderID)
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
	size_t structSize = textSize+sizeof(CEGUI::argb_t);

	unsigned int colorVal = static_cast<unsigned int>(color);
	colorVal = ntohl(colorVal);

	//Setup header too...?
	//header.Serialize(USERDATA, structSize); //I dont think I've identified any case where I need to do this

	dataVector.resize(structSize);

	//First we copy raw text data into our vector
	memcpy(dataVector.data(),			text.data(),	textSize);

	//Then we fill the last four bytes with color data
	memcpy(dataVector.data()+textSize,	&colorVal,			sizeof(unsigned int));
}


void UserDataPacket::Deserialize(char* inData, int dataSize, CEGUI::String* outUserName, CEGUI::argb_t* outUserTextColor )
{
	size_t colorSize = sizeof(CEGUI::argb_t);
	size_t textSize = dataSize-sizeof(CEGUI::argb_t);

	unsigned int colorVal(0);

	//Create a new string with data
	*outUserName = CEGUI::String(inData, textSize);

	//Fill color
	memcpy(&colorVal, inData+textSize, colorSize);

	*outUserTextColor = ntohl(colorVal);
}

void UserDataPacket::Deserialize(CEGUI::String* outUserName, CEGUI::argb_t* outUserTextColor)
{
	size_t colorSize = sizeof(CEGUI::argb_t);
	size_t textSize = dataVector.size()-colorSize;

	unsigned int colorVal(0);

	//Create a new string with data
	*outUserName = CEGUI::String(dataVector.data(), textSize);

	//Fill color
	memcpy(&colorVal, dataVector.data()+textSize, colorSize);

	*outUserTextColor = ntohl(colorVal);
}
