#pragma once

#include <cstdint>

//http://stackoverflow.com/questions/1953639/is-it-safe-to-cast-socket-to-int-under-win64
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
typedef SOCKET sock_t;
#else
//Unfortunately I don't actually know which of these headers contain the close() function, so I just include all of them.....
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef int sock_t;
#endif

class SocketWrapper
{
public:
	SocketWrapper();

	//RAII
	SocketWrapper(sock_t socket);
	~SocketWrapper();

	void SetNewSocket(sock_t extSocket)
	{
		CloseSocket();
		socket = extSocket;
	}

	//This will be called on deconstruction, but I like to have it available for explicit closing either way.
	int CloseSocket();

	int SendData(const char* sendingBuffer, const int bufferSize,			const int flag = 0);
	int ReceiveData(char* receivingBuffer, const int bufferSize,			const int flag = 0);

	//that's a lot of constness right there.
	const sock_t* const GetSocket() const
	{ 
		return &socket;
	}

private:
	sock_t socket;
};

/*
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <windows.h>
#include <cstdint>
#include "NetworkData.h"

class NetworkServices
{
public:
static int SendData(const sock_t currentSocket, const char* sendingBuffer, const int bufferSize, const int flag = 0);
static int ReceiveData(sock_t currentSocket, char* receivingBuffer, const int bufferSize, const int flag = 0);
};
*/