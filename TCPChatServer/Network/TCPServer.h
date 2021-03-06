#pragma once

#ifdef _WIN32   // Windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else                     // Unix/Linux
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include <memory>
#include <unordered_map>


#include "../CEGUI/TextMessage.h"
#include "CEGUI/CEGUI.h"
#include "NetworkData.h"
#include "ServerSettings.h"
#include "SocketWrapper.h"

class GameConsoleWindow;
class UserManager;
class Packet;
class TextPacket;

class TCPServer
{
public:
	TCPServer(UserManager *const usrMgr); //GameConsoleWindow* window
	~TCPServer();

	std::vector<TextMessage>& GetErrorMessages() { return errorMessages; }

	bool StartHosting();
	bool StopHosting();

	//Potentially receive a port? A password? Welcome message?
	bool Initialize(const ServerSettings settings);
	void Shutdown();

	//See if any client has sent any data since last time.
	//If we receive any data, it's put into outData to be processed by messageHandler
	bool ReceiveData(std::vector<std::unique_ptr<Packet>>& outData);

	//Distribute all data in the inData container to all clients
	//The packets are processed and prepared by messageHandler
	bool DistributeData(std::vector<std::unique_ptr<Packet>>& inData);

private:
	void PrintErrorMessage(CEGUI::String text);

	bool OpenWSA();
	void CloseWSA();

	bool AddClient();

	//Recursively extracts data until everything has been added to Outpackets in an ordered fashion
	void ExtractClientData(UserID id, std::vector<std::unique_ptr<Packet>>& outPackets, int packetSize, int packetIndex = 0);

private:
	//This is the container that will contain all the logged events/messages that we've processed. These will be sent to the console and printed later.
	std::vector<TextMessage> errorMessages;

	//Ptr to user manager that keeps track of user data and connections for us, amongst other things.
	UserManager* userManager;

	//Currently not used, but might contain stuff like server password, MOTD and that sort of stuff.
	ServerSettings settingsObject;

	const char* defaultPort;
	unsigned int defaultBufferLength;
	CEGUI::Colour serverColour;

	//Reusable buffer to contain data that we'll be receiving
	char network_data[MAX_PACKET_SIZE];

	//Our socket class
	SocketWrapper listenSocket;
	
	//Just a temporary object that will hold potential new clients. If it's a valid client, it gets used to construct a socketwrapper object that is moved elsewhere.
	sock_t tempClientSocket;

	// for error checking return values
	int iResult, iFlag, userID;
};

