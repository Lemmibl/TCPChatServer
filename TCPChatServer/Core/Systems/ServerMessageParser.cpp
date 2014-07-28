#include "ServerMessageParser.h"

#include "UserManager.h"
#include "../../CEGUI/GameConsoleWindow.h"
#include <iterator>

#include "../../Network/PacketTypes/Packet.h"
#include "../../Network/PacketTypes/TextPacket.h"
#include "../../Network/PacketTypes/ColoredTextPacket.h"
#include "../../Network/PacketTypes/UserDataPacket.h"

	//TODO: Change local instances to normal ptrs and deconstruct them properly (= nullptr)
ServerMessageParser::ServerMessageParser(UserManager* const usrMgr)
:	userManager(usrMgr)
{
	serverColor = 0xFFFFFFFF;
	inMessages.reserve(100); //Preallocate 100 slots.
	outMessages.reserve(100); //Same here
	messageLog.reserve(20);

	/********  Or do it this way if you want a more user friendly way to insert colors (range of 0-255 for each channel) *********/
	//for(int i = 0; i < 4; ++i)
	//{
	//	Fill the first 8 bits with 255 to represent max, aka white/opaque.
	//	serverColor |= 255 & 0xFF;

	//	Shift this value 8 bits to prepare for the next color channel
	//	serverColor <<= 8;
	//}
}

ServerMessageParser::~ServerMessageParser()
{
	userManager = nullptr;
}

void ServerMessageParser::HandleLocalMessages(std::vector<CEGUI::String>& messages)
{
	ChatUserData user = userManager->GetHostData();
	
	for(unsigned int i = 0; i < messages.size(); ++i)
	{
		//Create new string with our username and privilege level appended to the message.
		CEGUI::String text("[Host] " + user.userName + ": " + messages[i]);

		//Print text through text console.
		PrintText(text, user.textColor);

		//Prepare packet. This means serializing the header and resizing the packet body vector to outMessage.size().
		std::unique_ptr<ColoredTextPacket> outPacket(new ColoredTextPacket(text, user.textColor, 0));

		outMessages.push_back(std::move(outPacket));
	}

	messages.clear();
}


void ServerMessageParser::Update()
{
	//Clear out queues first before we add anything to it this update.
	outMessages.clear();
	messageLog.clear();

	for(unsigned int i = 0; i < inMessages.size(); ++i)
	{
		DataPacketType type = inMessages[i]->GetHeader()->GetType();
		switch(type)
		{
			case STRINGDATA:		ReadStringData(std::move(inMessages[i]), false); break;
			case COLOREDSTRINGDATA: ReadStringData(std::move(inMessages[i]), true); break;
			case DISCONNECT:		SendDisconnectMessage(inMessages[i]->GetSenderID()); break;
			case USERDATA:			ReadUserData(std::move(inMessages[i])); break;
			default:				break;
		}
	}

	//We've processed everything. Clear up for next time.
	inMessages.clear();
}

//¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
//¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ Read functions ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
//¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

void ServerMessageParser::ReadStringData(std::unique_ptr<Packet> packet, bool containsColorData)
{
	ChatUserData user;
	const UserID sender = packet->GetSenderID();

	if(userManager->GetUser(sender, &user))
	{
		CEGUI::String packetString;
		CEGUI::argb_t textColor = 0xFFFFFFFF;

		if(containsColorData)
		{
			static_cast<ColoredTextPacket*>(packet.get())->Deserialize(&packetString, &textColor);
		}
		else
		{
			//Get text from packet
			packetString = static_cast<TextPacket*>(packet.get())->Deserialize();

			//If packet doesn't contain color data, we just fetch it from the stored userColor from usermanager
			//textColor = user.textColor;
		}

		//We know that this packet is text, and that the body's data vector is going to contain the text in char format. So we simply initialize a string with this data.
		//We also append user's username before the message.
		CEGUI::String text(user.userName + ": " + packetString);

		//Print text locally. Move by value? Idk if best way to solve this. Or if there's even a problem to begin with.
		PrintText(text, textColor);

		//So now that we're done with the data, create a new packet and send it out to the clients.
		if(containsColorData)
		{
			outMessages.push_back(std::unique_ptr<ColoredTextPacket>(new ColoredTextPacket(text, user.textColor.getARGB(), sender)));
		}
		else
		{
			//Get text from packet
			outMessages.push_back(std::unique_ptr<TextPacket>(new TextPacket(text, sender)));
		}	
	}
}

void ServerMessageParser::ReadUserData(std::unique_ptr<Packet> packet)
{
	ChatUserData user;

	//If this goes through, variable: <userPtr> is pointing to an existing user that exists in userManager
	if(userManager->GetUser(packet->GetSenderID(), &user))
	{
		//Temporary variable to hold the data that we'll get when deserializing the packet
		CEGUI::argb_t userTextColor;

		//Extract data into local variables
		static_cast<UserDataPacket*>(packet.get())->Deserialize(&user.userName, &userTextColor);

		//Update user color
		user.textColor.setARGB(userTextColor);

		//Let everyone know that this user has joined the server.
		CEGUI::String text(user.userName);
		text += " has joined the server.";

		//Print locally to let the host know
		PrintText(text, user.textColor);

		//Make an outpacket
		std::unique_ptr<TextPacket> outPacket(new TextPacket(text, packet->GetSenderID()));

		//Insert outpacket
		outMessages.push_back(std::move(outPacket));
	}
}

//¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
//¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ Send functions ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
//¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

void ServerMessageParser::SendEventPacket(DataPacketType eventType)
{
	std::unique_ptr<Packet> packet(new Packet(eventType, 0, UserID(0)));

	//Dont even bother with initializing the body beyond what the constructor already does

	outMessages.push_back(std::move(packet));
}

//For our disconnect message, we just append the disconnected user's name. 
//Each client will then locally have to append some " has disconnected from the server." string to the end of the name
void ServerMessageParser::SendDisconnectMessage(UserID client_id)
{
	ChatUserData user;

	//If there is a client
	if(userManager->GetUser(client_id, &user))
	{
		CEGUI::String msg(user.userName + " has disconnected from the server.");

		PrintText(msg);

		//Create a text packet to tell everyone else about what has happened
		std::unique_ptr<TextPacket> outPacket(new TextPacket(msg, client_id));
	
		//and move outpacket to queue, because it won't be used here again
		outMessages.push_back(std::move(outPacket));
	}
}

//Process message, then append it in packet form to outQueue
void ServerMessageParser::SendTextPacket(CEGUI::String text, UserID userID)
{
	//temporary var to hold whatever we fetch from GetUser
	ChatUserData user;

	//True if it's the host(== 0) or we find a user inside usermanager
	if(userID == 0 || userManager->GetUser(userID, &user))
	{
		//Create message
		CEGUI::String outMessage; 

		//Super elegant solution. Might make a better solution at some point. If I feel the need.
		if(userID == 0)
		{
			user = userManager->GetHostData();

			outMessage = "[Host] "+user.userName + ": " + text;
		}
		else if(user.permissions == SuperAdmin)
		{
			outMessage = "[SuperAdmin] "+user.userName + ": " + text;
		}
		else if(user.permissions == Admin)
		{
			outMessage = "[Admin] "+user.userName + ": " + text;
		}
		else
		{
			outMessage = user.userName + ": " + text;
		}

		PrintText(outMessage, user.textColor);

		//Prepare packet. This means serializing the header and resizing the packet body vector to outMessage.size().
		std::unique_ptr<ColoredTextPacket> outPacket(new ColoredTextPacket(outMessage, user.textColor, userID));

		outMessages.push_back(std::move(outPacket));
	}
}

void ServerMessageParser::PrintText(CEGUI::String text, CEGUI::Colour textColor)
{
	messageLog.push_back(std::move(TextMessage(text, textColor)));
}
