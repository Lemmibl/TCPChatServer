#include "ServerScreenNoGUI.h"

#include "../../Network/TCPServer.h"
#include "../Systems/ServerMessageParser.h"
#include "../Systems/UserManager.h"
#include "../../CEGUI/GameConsoleWindow.h"
#include "../../CEGUI/ServerSidebarWindow.h"

ServerScreenNoGUI::ServerScreenNoGUI()
	: ScreenBase(),
	SettingsDependent()
{
	//Load server settings amongst other things
	InitializeSettings(this);
}


ServerScreenNoGUI::~ServerScreenNoGUI()
{
	if(server != nullptr)
	{
		server->Shutdown();
	}
}

bool ServerScreenNoGUI::Enter()
{
	//Only call initialize the first time we enter this class. Yes, this means that everything in this class will live on until the program closes.
	if(!isInitialized)
	{
		//If initialization fails we exit early with a false
		if(!Initialize())
		{
			return false;
		}

		isInitialized = true;
	}

	server->StartHosting();

	return true;
}

bool ServerScreenNoGUI::Initialize()
{
	bool result;

	//Create user manager
	userManager.reset(new UserManager());

	//Create message handler
	messageHandler = std::unique_ptr<ServerMessageParser>(new ServerMessageParser(userManager.get()));

	//Create TCP server
	server = std::unique_ptr<TCPServer>(new TCPServer(userManager.get()));

	//Initialize TCP server with loaded settings
	result = server->Initialize(serverSettings);
	if(!result)
	{
		return false;
	}

	return true;
}

bool ServerScreenNoGUI::Update(double deltaTime)
{
	//Server receives new data
	if(!server->ReceiveData(messageHandler->GetInMessages()))
	{
		return false;
	}

	//Message handler processes external data
	messageHandler->Update();

	//Update usermanager to remove old userdata and close old connections
	userManager->Update();

	//Server redistributes new data
	if(!server->DistributeData(messageHandler->GetOutMessages()))
	{
		return false;
	}

	return true;
}

void ServerScreenNoGUI::Render()
{
}

void ServerScreenNoGUI::Exit()
{
	server->StopHosting();
}

void ServerScreenNoGUI::OnSettingsReload( Config* cfg )
{
}