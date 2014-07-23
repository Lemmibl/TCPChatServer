#include "ClientMessageParser.h"

#include "../../CEGUI/GameConsoleWindow.h"

#include "../../Network/PacketTypes/Packet.h"
#include "../../Network/PacketTypes/TextPacket.h"
#include "../../Network/PacketTypes/ColoredTextPacket.h"
#include "../../Network/PacketTypes/UserDataPacket.h"

ClientMessageParser::ClientMessageParser(GameConsoleWindow* const console)
: consoleWindow(console)
{
}


ClientMessageParser::~ClientMessageParser()
{
	consoleWindow = nullptr;
}

void ClientMessageParser::Update()
{
	for(unsigned int i = 0; i < inMessageQueue.size(); ++i)
	{
		
	}
}

void ClientMessageParser::SendTextPacket(CEGUI::String text, bool sendColor, CEGUI::argb_t color)
{
	size_t len = text.size();

	//We don't send if it's an empty message.....
	if(len > 0)
	{
		std::unique_ptr<Packet> outPacket(nullptr);

		if(sendColor == true)
		{
			//Make colored text packet
			outPacket.reset(new ColoredTextPacket(text, color, 0));
		}
		else
		{	
			//Make normal text packet
			outPacket.reset(new TextPacket(text, 0));
		}

		//And move it into this list
		outMessageQueue.push_back(std::move(outPacket));
	}
}

void ClientMessageParser::SendUserDataPacket(const ChatUserData& userData )
{
	//Prepare our data packet
	std::unique_ptr<UserDataPacket> outPacket(new UserDataPacket(userData.userName, userData.textColor, userData.id));

	//Insert packet
	outMessageQueue.push_back(std::move(outPacket));
}

void ClientMessageParser::ReadStringData(std::unique_ptr<Packet> packet, bool containsColorData )
{
	CEGUI::String packetString;
	CEGUI::argb_t textColor;

	if(containsColorData)
	{
		static_cast<ColoredTextPacket*>(packet.get())->Deserialize(&packetString, &textColor);

		//Print received text.
		consoleWindow->PrintText(packetString, CEGUI::Colour(textColor));
	}
	else
	{
		//Get text from packet
		packetString = static_cast<TextPacket*>(packet.get())->Deserialize();

		//Print received text. With default color.
		consoleWindow->PrintText(packetString);
	}
}

void ClientMessageParser::ReadDisconnectData(std::unique_ptr<Packet> packet)
{
	//TODO... Potentially.
	//Thing is, it's not really used because we disconnect in other ways. But this might be for future stuff like... If you get kicked by the server or having a nice way of saying "The server is shutting down now."
}

void ClientMessageParser::ReadUserData(std::unique_ptr<Packet> packet)
{
	//TODO. It's supposed to be for loading in data about other players. So that we don't have to send stuff like user name + user textcolor every single time someone sends a message. might be necessary? might not be?
}
