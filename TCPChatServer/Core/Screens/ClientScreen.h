#pragma once
#include "ScreenBase.h"
#include "CEGUI/CEGUI.h"

//For UserData definition
#include "../../Network/ChatUserData.h"

class TCPClient;
class GameConsoleWindow;
class ClientMessageHandler;
class ClientSidebarWindow;
struct GLFWwindow;

class ClientScreen : public ScreenBase
{
public:
	ClientScreen();
	~ClientScreen();

	bool Enter();
	bool Initialize();
	bool Update(double deltaTime);
	void Render();
	void Exit();

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
	std::unique_ptr<ClientMessageHandler> messageHandler;

	//Don't need to call update on things when we don't have a connection open :)
	bool connectionActive;
};