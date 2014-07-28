#include "TCPClient.h"

#include "../../CEGUI/GameConsoleWindow.h"
#include "PacketTypes/Packet.h"
#include "PacketTypes/PacketHeader.h"
#include "PacketTypes/TextPacket.h"
#include "PacketTypes/ColoredTextPacket.h"
#include "PacketTypes/UserDataPacket.h"

TCPClient::TCPClient(GameConsoleWindow* const console )
	: outFlags(0),
	iResult(0),
	defaultBufferLength(1024),
	defaultPort("22222"),
	consoleWindow(console),
	connectionSocket(0)
{
}


TCPClient::~TCPClient()
{
	Shutdown();
}

bool TCPClient::Connect(std::string ip, CEGUI::String port)
{
	addrinfo* result = NULL;
	addrinfo* ptr = NULL;
	addrinfo hints;

	if(!OpenWSA())
	{
		return false;
	}

	consoleWindow->PrintText("Now trying to connect to IP: " + ip + " Port: " + port);

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family		= AF_UNSPEC;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_protocol	= IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0) 
	{
		consoleWindow->PrintText("getaddrinfo failed. Error code: " + iResult);
		CloseWSA();

		return false;
	}

	// Attempt to connect to an address until one succeeds
	for(ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		connectionSocket.SetNewSocket(socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol));
		if (*connectionSocket.GetSocket() == INVALID_SOCKET) 
		{
			consoleWindow->PrintText("Socket failed with error code: " + WSAGetLastError());
			CloseWSA();
			return false;
		}

		// Connect to server.
		iResult = connect(*connectionSocket.GetSocket(), ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) 
		{
			iResult = WSAGetLastError();
			connectionSocket.CloseSocket();
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (*connectionSocket.GetSocket() == INVALID_SOCKET) 
	{
		consoleWindow->PrintText("Unable to connect to server!");
		CloseWSA();
		return false;
	}

	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;

	//Here is an important piece of information. We are going to set our socket to be non-blocking so that 
	// it will not wait on send() and receive() functions when there is no data to send/receive. 
	// This is necessary for our multiplayer game since we'd like the game to keep going if there isn't anything to send or receive to or from a client.
	iResult = ioctlsocket(*connectionSocket.GetSocket(), FIONBIO, &iMode);
	if (iResult == SOCKET_ERROR)
	{
		consoleWindow->PrintText("ioctlsocket failed with error: " + WSAGetLastError());
		connectionSocket.CloseSocket();
		CloseWSA();
		exit(1);        
	}

	//Disable Nagle algorithm
	//http://forums.codeguru.com/showthread.php?136908-Winsock-and-NAGLE-algorithm
	//Windows Sockets applications can disable the Nagle algorithm for their connections by setting the TCP_NODELAY socket option.
	char value = 1;
	setsockopt(*connectionSocket.GetSocket(), IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

	return true;
}

void TCPClient::ExtractDataFromServer(std::vector<std::unique_ptr<Packet>>& outPackets, int packetSize, int packetIndex)
{
	if(packetSize > 0)
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
		case STRINGDATA:		outPackets.push_back(std::unique_ptr<TextPacket>(new TextPacket(network_data+packetIndex, dataSize, 0)));					break;
		case COLOREDSTRINGDATA: outPackets.push_back(std::unique_ptr<ColoredTextPacket>(new ColoredTextPacket(network_data+packetIndex, dataSize, 0)));	break;
		case USERDATA:			outPackets.push_back(std::unique_ptr<UserDataPacket>(new UserDataPacket(network_data+packetIndex, dataSize, 0)));			break;
		//Event packet or a packet without any data body (only header)
		default:				outPackets.push_back(std::unique_ptr<Packet>(new Packet(packetHeader, 0)));												break;
		}

		//Move index forward again and subtract size again
		packetIndex += dataSize;
		packetSize -= dataSize;

		//Call again if there's more data to be extracted
		if(packetSize > 0)
		{
			ExtractDataFromServer(outPackets, packetSize, packetIndex);
		}
	}
}

bool TCPClient::SendDataToServer(std::vector<std::unique_ptr<Packet>>& inData)
{
	for(unsigned int i = 0; i < inData.size(); ++i)
	{
		//Send header
		int result = connectionSocket.SendData(inData[i]->GetHeader()->dataArray, PacketHeader::SizeOfStruct());
		if(result <= 0)
		{
			return false;
		}

		//If it's a type of packet that contains a body after the header, we send the body
		if(inData[i]->GetHeader()->GetSize() > 0)
		{
			result = connectionSocket.SendData(inData[i]->GetData().data(), inData[i]->GetData().size());
			if(result <= 0)
			{
				return false;
			}
		}
	}

	inData.clear();

	return true;
}

void TCPClient::Disconnect()
{
	//int result = 1;

	// shutdown the connection since no more data will be sent or read
	//result = 
	shutdown(*connectionSocket.GetSocket(), SD_SEND);

	// cleanup
	connectionSocket.CloseSocket();
}

void TCPClient::Shutdown()
{
	Disconnect();
}

bool TCPClient::ReceiveData( std::vector<std::unique_ptr<Packet>>& outData )
{
	//Read to see if we've received any data
	int packetSize = connectionSocket.ReceiveData(network_data, MAX_PACKET_SIZE);

	//Extract all the data
	ExtractDataFromServer(outData, packetSize, 0);

	return true;
}

bool TCPClient::OpenWSA()
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

void TCPClient::CloseWSA()
{
	#ifdef _WIN32
		WSACleanup();
	#endif
}