#pragma once

#ifdef _DEBUG
#include <vld.h>
#endif

#include <memory>

class GameConsoleWindow;
class ServerScreenNoGUI;

//This server is exclusively for debugging my client/server interaction. It'll be removed later.
class DebugServer
{
public:
	DebugServer(GameConsoleWindow* console);
	~DebugServer();

	bool Update(double deltaTime);

private:
	std::unique_ptr<ServerScreenNoGUI> serverNoGUI;
};

