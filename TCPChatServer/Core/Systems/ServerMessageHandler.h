#pragma once
#include <vector>
#include <list>
#include "CEGUI/CEGUI.h"
#include "../../Network/ChatUserData.h"

class UserManager;
class GameConsoleWindow;
class Packet;
class UserDataPacket;

class ServerMessageHandler
{
public:
	ServerMessageHandler(UserManager* const usrMgr,	GameConsoleWindow* const consoleWindow);
	~ServerMessageHandler();

	void Update();

	//Returns the queue that is processed in MessageHandler's update function.
	std::vector<std::unique_ptr<Packet>>& GetInMessageQueue() { return inMessageQueue; }
	std::vector<std::unique_ptr<Packet>>& GetOutMessageQueue() { return outMessageQueue; }

	void HandleLocalMessages(std::vector<CEGUI::String>& messages);

	void SendEventPacket(DataPacketType eventType);
	void SendTextPacket(CEGUI::String text, UserID userID = 0); //UserID can be == 0. That means the server host is sending a message, aka it's local.

	//Sends a packet with size 0. Use purely for events that require no data, such as server shutdowns and things. TODO: think about this and see if there actually are zero-data events...
	void SendDisconnectMessage(UserID client_id);
	void SendDisconnectPacket();

private:
	void ReadStringData(std::unique_ptr<Packet> packet,  bool containsColorData);
	void ReadUserData(std::unique_ptr<Packet> packet);

private:
	//Ptr to the console window class so that we can output text locally
	GameConsoleWindow* textConsole;

	//In case we as the server want to write something, this is the color we'll use.
	unsigned int serverColor;

	//List of sessions to terminate
	std::list<UserID> clientsToDisconnect;

	//... I use a preallocated vector because it feels like the cleanest and it's probably the fastest
	std::vector<std::unique_ptr<Packet>> inMessageQueue;
	std::vector<std::unique_ptr<Packet>> outMessageQueue;

	//To have access to stuff like user permissions when we parse commands amongst other things.
	UserManager* userManager;
};