#pragma once
#include "ScreenBase.h"
#include "SettingsDependent.h"
#include "../../Network/ServerSettings.h"
#include "CEGUI/CEGUI.h"

class TCPServer;
class UserManager;
class ServerMessageHandler;
class GameConsoleWindow;
class ServerSidebarWindow;

class ServerScreen : public ScreenBase, public SettingsDependent
{
public:
	ServerScreen();
	~ServerScreen();

	bool Enter();
	bool Initialize();

	bool Update(double deltaTime);
	void Render();
	void Exit();

	virtual void OnSettingsReload( Config* cfg );

private:
	bool InitializeGUI();
	void UpdateServerStatus(bool active);
	void ResetGUI();
	
private:
	std::unique_ptr<UserManager> userManager;
	std::unique_ptr<TCPServer> server;
	std::unique_ptr<ServerMessageHandler> messageHandler;

	CEGUI::Window* rootWindow;
	std::unique_ptr<ServerSidebarWindow> sidebarWindow;
	std::unique_ptr<GameConsoleWindow> consoleWindow;

	ServerSettings serverSettings;
	bool serverActive;
};

