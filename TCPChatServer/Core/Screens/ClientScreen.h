#pragma once
#include "ScreenBase.h"
#include "CEGUI/CEGUI.h"

//For UserData definition
#include "../../Network/ChatUserData.h"

class TCPClient;
class GameConsoleWindow;
class ClientMessageParser;
class ClientSidebarWindow;
struct GLFWwindow;

class ClientScreen : public ScreenBase
{
public:
	ClientScreen();
	~ClientScreen();

	virtual bool Enter();
	virtual bool Initialize();
	virtual bool Update(double deltaTime);
	virtual void Render();
	virtual void Exit();

	GameConsoleWindow* GetConsole() { return consoleWindow.get(); }

private:
	void UpdateConnectionStatus(bool active);
	bool InitializeGUI();
	void ResetGUI();

private:
	//These are just handles whose lifetime is handled elsewhere. Hence normal ptrs.
	GLFWwindow* glfwWindow;
	CEGUI::Window* rootWindow;

	ChatUserData userData;

	std::unique_ptr<TCPClient> chatClient;
	std::unique_ptr<GameConsoleWindow> consoleWindow;
	std::unique_ptr<ClientSidebarWindow> sidebarWindow;
	std::unique_ptr<ClientMessageParser> messageParser;

	//Don't need to call update on things when we don't have a connection open :)
	bool connectionActive;
};