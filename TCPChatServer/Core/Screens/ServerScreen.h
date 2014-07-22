#pragma once
#include "ScreenBase.h"
#include "SettingsDependent.h"
#include "../../Network/ServerSettings.h"
#include "CEGUI/CEGUI.h"
#include "../../CEGUI/TextMessage.h"

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
	void UpdateServerStatus(bool activate);
	void ResetGUI();
	
	void PrintMessageLog(std::vector<TextMessage>& log);

private:
	ServerSettings serverSettings;
	std::unique_ptr<UserManager> userManager;
	std::unique_ptr<TCPServer> server;
	std::unique_ptr<ServerMessageHandler> messageHandler;
	std::unique_ptr<ServerSidebarWindow> sidebarWindow;
	std::unique_ptr<GameConsoleWindow> consoleWindow;
	CEGUI::Window* rootWindow;

	bool serverActive;
};

