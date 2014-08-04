#include "DebugServer.h"
#include "..\Screens\ServerScreenNoGUI.h"

DebugServer::DebugServer(GameConsoleWindow* console)
: serverNoGUI(new ServerScreenNoGUI(console))
{
	serverNoGUI->Enter();
}

DebugServer::~DebugServer()
{
}

bool DebugServer::Update( double deltaTime )
{
	return serverNoGUI->Update(deltaTime);
}
