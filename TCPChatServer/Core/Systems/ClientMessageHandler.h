#pragma once
#include "CEGUI/CEGUI.h"
#include "../../Network/ChatUserData.h"

class Packet;
class GameConsoleWindow;

class ClientMessageHandler
{
public:
	ClientMessageHandler(GameConsoleWindow* const console);
	~ClientMessageHandler();

	//Returns the queue that is processed in MessageHandler's update function.
	std::vector<std::unique_ptr<Packet>>& GetInMessageQueue() { return inMessageQueue; }
	std::vector<std::unique_ptr<Packet>>& GetOutMessageQueue() { return outMessageQueue; }

	//Process any messages that could be in the inMessageQueue
	void Update();

	void SendTextPacket(CEGUI::String text, bool sendColor, CEGUI::argb_t color = 0);
	void SendUserDataPacket(const ChatUserData& userData);

	private:
		void ReadStringData(std::unique_ptr<Packet> packet,  bool containsColorData);
		void ReadUserData(std::unique_ptr<Packet> packet);
		void ReadDisconnectData(std::unique_ptr<Packet> packet);

private:
	GameConsoleWindow* consoleWindow;

	//... I use a preallocated vector because it feels like the cleanest and it's probably the fastest
	std::vector<std::unique_ptr<Packet>> inMessageQueue;
	std::vector<std::unique_ptr<Packet>> outMessageQueue;
};

