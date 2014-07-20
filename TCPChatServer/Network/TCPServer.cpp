#include "TCPServer.h"

#include "../Core/Systems/UserManager.h"
#include "../CEGUI/GameConsoleWindow.h"
#include "PacketTypes/Packet.h"
#include "PacketTypes/TextPacket.h"
#include "PacketTypes/ColoredTextPacket.h"
#include "PacketTypes/UserDataPacket.h"

TCPServer::TCPServer(UserManager* const usrMgr)
	: userManager(usrMgr),
	iFlag(0),
	defaultPort("22222"), 	//Port number that is unlikely to be used on client computers. Also easy to remember. http://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers
	defaultBufferLength(1024),
	listenSocket(INVALID_SOCKET), //Some default value
	tempClientSocket(INVALID_SOCKET),
	userID(0),
	serverColour(1.0f, 0.0f, 0.0f, 1.0f) //Red
{
	//Init and zero all containers. 
	memset(network_data, 0, MAX_PACKET_SIZE);
}

TCPServer::~TCPServer()
{
	userManager = nullptr;
	consoleWindow = nullptr;
	Shutdown();
}

bool TCPServer::Initialize(const ServerSettings settings, GameConsoleWindow *const console)
{
	consoleWindow = console;
	settingsObject = settings;

	if(!OpenWSA())
	{
		return false;
	}

	return true;
}

void TCPServer::Shutdown()
{
	StopHosting();

	CloseWSA();
}

bool TCPServer::AddClient()
{
	//If we get a new client waiting, accept the connection and save the socket
	//http://msdn.microsoft.com/en-us/library/windows/desktop/ms737526(v=vs.85).aspx
	tempClientSocket = accept(listenSocket.GetSocket(), NULL, NULL);

	//No new clients to accept, we return immediately.
	if(tempClientSocket == WSAEWOULDBLOCK)
	{
		return false;
	}
	else if(tempClientSocket != INVALID_SOCKET) 
	{
		//disable nagle algorithm on the client's socket
		char val = 1;
		setsockopt(tempClientSocket, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));

		//For now, just insert a valid socket and some default values
		userManager->AddUser(SocketWrapper(tempClientSocket), "User #"+(userID++), 0xFFFFFFFF, 0);

		return true;
	}

	return false;
}

bool TCPServer::ReceiveData(std::vector<std::unique_ptr<Packet>>& outData)
{
	//See if we've gotten any new client connection requests
	if(AddClient())
	{
		consoleWindow->PrintText("Client has been connected to the server. Client ID is: " + std::to_string((unsigned long long)userID), serverColour); 
	}

	//Get begin() and end() iterators for the user map
	auto& userIterators = userManager->GetAllUsers();

	//Iterate over each user that our userManager contains
	for(auto it = userIterators.first; it != userIterators.second; ++it)
	{
		//Null all data .... necessary? 
		//memset(network_data, 0, MAX_PACKET_SIZE); //Let's try not nulling it.

		//See if this user has sent any data
		int packetSize = it->second.clientSocket.ReceiveData(network_data, MAX_PACKET_SIZE);

		//Sanity check
		if(packetSize > MAX_PACKET_SIZE)
		{
			return false;
		}

		//If they have, we handle it
		if(packetSize > 0)
		{
			ExtractClientData(it->second.id, outData, packetSize);
		}
	}

	return true;
}

//Recursively extract all data that a client has sent
void TCPServer::ExtractClientData(UserID id, std::vector<std::unique_ptr<Packet>>& outData, int packetSize, int packetIndex)
{
	//Create header
	PacketHeader packetHeader(network_data+packetIndex);

	//Variables to hold data extracted when we deserialize packet header
	DataPacketType dataType;
	int dataSize;

	//Extract data from header into the above declared variables
	packetHeader.Deserialize(&dataType, &dataSize);

	//Move packetIndex forward
	packetIndex += PacketHeader::SizeOfStruct();

	//And decrease packetSize
	packetSize -= PacketHeader::SizeOfStruct();

	switch(dataType)
	{
		case STRINGDATA:		outData.push_back(std::unique_ptr<TextPacket>(new TextPacket(network_data+packetIndex, dataSize, 0)));					break;
		case COLOREDSTRINGDATA: outData.push_back(std::unique_ptr<ColoredTextPacket>(new ColoredTextPacket(network_data+packetIndex, dataSize, 0)));	break;
		case USERDATA:			outData.push_back(std::unique_ptr<UserDataPacket>(new UserDataPacket(network_data+packetIndex, dataSize, 0)));			break;
		//Event packet or a packet without any data body (only header)
		default:				outData.push_back(std::unique_ptr<Packet>(new Packet(packetHeader, 0)));												break;
	}

	//Move index forward again and subtract size again
	packetIndex += dataSize;
	packetSize -= dataSize;

	//Call again if there's more data to be extracted
	if(packetSize > 0)
	{
		ExtractClientData(id, outData, packetSize, packetIndex);
	}
}

bool TCPServer::DistributeData(std::vector<std::unique_ptr<Packet>>& inData)
{
	int iSendResult;

	//Return early when we have nothing to process
	if(inData.size() == 0)
	{
		return true;
	}

	//Get pair of iterators. Beginning and end.
	auto& iteratorPair = userManager->GetAllUsers();
	int dataSize = 0;

	//For each user connected to the server
	for(auto sessionIter = iteratorPair.first; sessionIter != iteratorPair.second; ++sessionIter)
	{
		SocketWrapper* const clientSocket = &sessionIter->second.clientSocket;

		//For each outbound message
		for(unsigned int i = 0; i < inData.size(); ++i)
		{
			dataSize = inData[i]->GetHeader()->GetSize();

			//Send data header
			iSendResult = clientSocket->SendData(inData[i]->GetHeader()->dataArray, dataSize);

			if(iSendResult == SOCKET_ERROR) 
			{
				CEGUI::String errorMsg = "Failed to send data header to client nr: " + sessionIter->first;
				errorMsg += ". Error code: " + WSAGetLastError();

				consoleWindow->PrintText(errorMsg, serverColour);
				userManager->RemoveUser(sessionIter->first);
			}

			//If we're actually sending a data packet and not just an event packet
			if(dataSize > 0)
			{
				//Send data body
				iSendResult = clientSocket->SendData(inData[i]->GetData().data(), dataSize);

				if(iSendResult == SOCKET_ERROR) 
				{
					CEGUI::String errorMsg = "Failed to send data to client nr: " + sessionIter->first;
					errorMsg += ". Error code: " + WSAGetLastError();

					consoleWindow->PrintText(errorMsg, serverColour);
					userManager->RemoveUser(sessionIter->first);
				}
			}
		}	
	}

	return true;
}

bool TCPServer::StartHosting()
{
	// address info for the server to listen to
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// set address information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, defaultPort.data(), &hints, &result);

	if(iResult != 0) 
	{
		consoleWindow->PrintText("getaddrinfo failed with error: " + iResult, serverColour);
		Shutdown();
		return false;
	}

	// Create a SOCKET for connecting to server
	listenSocket = SocketWrapper(socket(result->ai_family, result->ai_socktype, result->ai_protocol));

	if (listenSocket.GetSocket() == INVALID_SOCKET) 
	{
		consoleWindow->PrintText("socket failed with error: " + WSAGetLastError(), serverColour);
		freeaddrinfo(result);
		Shutdown();
		return false;
	}

	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;
	iResult = ioctlsocket(listenSocket.GetSocket(), FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR) 
	{
		consoleWindow->PrintText("ioctlsocket failed with error: " + WSAGetLastError(), serverColour);
		Shutdown();
		return false;
	}

	// Setup the TCP listening socket
	iResult = bind( listenSocket.GetSocket(), result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) 
	{
		consoleWindow->PrintText("bind failed with error: " + WSAGetLastError(), serverColour);
		freeaddrinfo(result);
		Shutdown();
		return false;
	}

	// no longer need address information
	freeaddrinfo(result);

	// start listening for new clients attempting to connect
	iResult = listen(listenSocket.GetSocket(), SOMAXCONN);

	if (iResult == SOCKET_ERROR) 
	{
		consoleWindow->PrintText("listen failed with error: " + WSAGetLastError(), serverColour);
		Shutdown();
		return false;
	}

	return true;
}

bool TCPServer::StopHosting()
{
	//If socket is currently active...
	if(listenSocket.GetSocket() != INVALID_SOCKET)
	{	
		listenSocket.CloseSocket();

		return true;
	}
	else
	{
		return false;
	}
}

bool TCPServer::OpenWSA()
{
#ifdef _WIN32
	WSADATA wsaData;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult < 0) 
	{
		consoleWindow->PrintText("WSAStartup failed. Error code: " + iResult);
		return false;
	}
#endif

	return true;
}

void TCPServer::CloseWSA()
{
#ifdef _WIN32
	WSACleanup();
#endif
}
