#include "ColoredTextPacket.h"

#include <string.h> //for memcpy

ColoredTextPacket::ColoredTextPacket(CEGUI::String text, CEGUI::argb_t color,  UserID senderID)
: Packet(COLOREDSTRINGDATA, sizeof(CEGUI::argb_t)+text.size(), senderID)
{
}

ColoredTextPacket::ColoredTextPacket(const char* inData, int dataSize, UserID senderID)
: Packet(COLOREDSTRINGDATA, dataSize, senderID)
{
	dataVector.resize(dataSize);
	memcpy(dataVector.data(), inData, dataSize);
}


ColoredTextPacket::~ColoredTextPacket()
{
}

void ColoredTextPacket::Serialize(CEGUI::String text, CEGUI::argb_t color)
{
	size_t textSize = text.size();
	size_t structSize = textSize+sizeof(CEGUI::argb_t);

	//Setup header too
	header.Serialize(COLOREDSTRINGDATA, structSize);

	dataVector.resize(structSize);

	//First we copy raw text data into our vector
	memcpy(dataVector.data(), text.data(), textSize);

	//Then we fill the last four bytes with color data
	memcpy(dataVector.data()+textSize, &color, sizeof(CEGUI::argb_t));
}


void ColoredTextPacket::Deserialize( const char* inData, int dataSize, CEGUI::String *const outText, CEGUI::argb_t *const outColor )
{
	size_t colorSize = sizeof(CEGUI::argb_t);
	size_t textSize = dataSize-sizeof(CEGUI::argb_t);

	//Create a new string with data
	*outText = CEGUI::String(inData, textSize);

	//Fill color
	memcpy(outColor, &inData+textSize, colorSize);
}

void ColoredTextPacket::Deserialize(CEGUI::String *const outText, CEGUI::argb_t *const outColor )
{
	size_t colorSize = sizeof(CEGUI::argb_t);
	size_t textSize = dataVector.size()-colorSize;

	//Create a new string with data
	*outText = CEGUI::String(dataVector.data(), textSize);
	
	//Fill color
	memcpy(outColor, dataVector.data()+textSize, colorSize);
}
