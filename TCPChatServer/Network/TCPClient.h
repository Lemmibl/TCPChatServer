#pragma once
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <ctime>
#include <vector>

#include "CEGUI/CEGUI.h"
#include "NetworkData.h"
#include "SocketWrapper.h"

class NetworkServer;
class GameConsoleWindow;
class Packet;

class TCPClient
{
private:
//Port number that is unlikely to be used on client computers. Also easy to remember.
//http://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers

public:
TCPClient(GameConsoleWindow* const console);
~TCPClient();

//Connect function, with default ip and port in case nothing is entered
bool Connect(CEGUI::String ip, CEGUI::String port = "22222");

//If we receive any data, it's put into outData to be processed by client message handler
bool ReceiveData(std::vector<std::unique_ptr<Packet>>& outData);

//Send all the data that is contained within inData
bool SendDataToServer(std::vector<std::unique_ptr<Packet>>& inData);

void Disconnect();
void Shutdown();

private:
//Recursively extracts data until everything has been added to Outpackets in an ordered fashion
void ExtractDataFromServer(std::vector<std::unique_ptr<Packet>>& outPackets, int packetSize, int packetIndex = 0);

private:
//Just for debugging purposes...?
GameConsoleWindow* const consoleWindow;

//Socket that we use to communicate with server...
SocketWrapper connectionSocket;

//Buffer we use to receive data
char network_data[MAX_PACKET_SIZE];

int outFlags, iResult;

const CEGUI::String defaultPort;
const unsigned int defaultBufferLength;
};