#include "SocketWrapper.h"

SocketWrapper::SocketWrapper()
	: socket(INVALID_SOCKET)
{
}

SocketWrapper::SocketWrapper(sock_t externalSocket)
	: socket(externalSocket)
{
}

SocketWrapper::~SocketWrapper()
{
	CloseSocket();
}

int SocketWrapper::CloseSocket()
{
	int result(0);

	//Release socket...
	if(socket != 0)
	{
#ifdef WIN32
		result = closesocket(socket);
#else
		result = close(socket);
#endif
	}

	socket = INVALID_SOCKET;

	return result;
}


int SocketWrapper::SendData(const char* sendingBuffer, const int bufferSize, const int flag)
{
	return send(socket, sendingBuffer, bufferSize, flag);
}

int SocketWrapper::ReceiveData(char* receivingBuffer, const int bufferSize, const int flag)
{
	return recv(socket, receivingBuffer, bufferSize, flag);
}