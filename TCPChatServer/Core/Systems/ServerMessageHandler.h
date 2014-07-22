#pragma once
#include <vector>
#include <list>
#include "CEGUI/CEGUI.h"
#include "../../Network/ChatUserData.h"
#include "../../CEGUI/TextMessage.h"

class UserManager;
class GameConsoleWindow;
class Packet;
class UserDataPacket;

class ServerMessageHandler
{
public:
	ServerMessageHandler(UserManager* const usrMgr);
	~ServerMessageHandler();

	void Update();

	//Returns the queue that is processed in MessageHandler's update function.
	std::vector<std::unique_ptr<Packet>>& GetInMessages() { return inMessages; }
	std::vector<std::unique_ptr<Packet>>& GetOutMessages() { return outMessages; }

	std::vector<TextMessage>& GetMessageLog() { return messageLog; }

	void HandleLocalMessages(std::vector<CEGUI::String>& messages);

	void SendEventPacket(DataPacketType eventType);
	void SendTextPacket(CEGUI::String text, UserID userID = 0); //UserID can be == 0. That means the server host is sending a message, aka it's local.

	//Sends a packet with size 0. Use purely for events that require no data, such as server shutdowns and things. TODO: think about this and see if there actually are zero-data events...
	void SendDisconnectMessage(UserID client_id);
	void SendDisconnectPacket();

private:
	void ReadStringData(std::unique_ptr<Packet> packet,  bool containsColorData);
	void ReadUserData(std::unique_ptr<Packet> packet);

	void PrintText(CEGUI::String text, CEGUI::Colour textColor = 0xFFFFFFFF);

private:
	//In case we as the server want to write something, this is the color we'll use.
	unsigned int serverColor;

	//List of sessions to terminate
	std::list<UserID> clientsToDisconnect;

	//... I use a preallocated vector because it feels like the cleanest and it's probably the fastest
	std::vector<std::unique_ptr<Packet>> inMessages;
	std::vector<std::unique_ptr<Packet>> outMessages;

	//This is the container that will contain all the logged events/messages that we've processed. These will be sent to the console and printed later.
	std::vector<TextMessage> messageLog;

	//To have access to stuff like user permissions when we parse commands amongst other things.
	UserManager* userManager;
};