#include "ServerScreen.h"

#include "../../Network/TCPServer.h"
#include "../Systems/ServerMessageParser.h"
#include "../Systems/UserManager.h"
#include "../../CEGUI/GameConsoleWindow.h"
#include "../../CEGUI/ServerSidebarWindow.h"

ServerScreen::ServerScreen()
	: ScreenBase(),
	SettingsDependent()
{
	serverActive = false;
}


ServerScreen::~ServerScreen()
{
	if(server != nullptr)
	{
		server->Shutdown();
	}
}

bool ServerScreen::Enter()
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
	else
	{
		serverActive = false;

		//If this class has already been initialized we just do a reset of everything relevant. Essentially the same effect.
		ResetGUI();
	}

	//Show mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);

	//Set this base window as root.
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWindow);

	//Not 100% sure if this is necessary but it can't hurt
	rootWindow->activate();
	rootWindow->show();

	return true;
}

bool ServerScreen::Initialize()
{
	//Load settings and bind event callback function in case we re-load settings during runtime.
	InitializeSettings(this);

	bool result;

	serverActive = false;

	//Create user manager
	userManager.reset(new UserManager());

	if(!InitializeGUI())
	{
		return false;
	}

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

bool ServerScreen::InitializeGUI()
{

	rootWindow = CEGUI::WindowManager::getSingletonPtr()->loadLayoutFromFile("CustomLayouts/ColoredServerBackground.layout");

	sidebarWindow.reset(new ServerSidebarWindow);
	sidebarWindow->Initialize("CustomLayouts/ServerSidebar.layout", rootWindow, userManager->GetHostData());

	consoleWindow.reset(new GameConsoleWindow);
	consoleWindow->CreateCEGUIWindow("CustomLayouts/Console.layout", rootWindow);

	consoleWindow->setVisible(true);

	return true;
}

bool ServerScreen::Update(double deltaTime)
{
	//If we quit hosting, change system state to main menu
	if(sidebarWindow->Quit())
	{
		stateChangeEvent(SystemStates::MainMenuScreen);
	}

	//If someone has toggle the hosting button...
	if(serverActive != sidebarWindow->IsHostingActive())
	{
		//Save results for next time
		serverActive = sidebarWindow->IsHostingActive();

		//Use results
		UpdateServerStatus(serverActive);
	}	

	if(serverActive)
	{
		//Server receives new data
		if(!server->ReceiveData(messageHandler->GetInMessages()))
		{
			return false;
		}

		//Message handler processes external data
		messageHandler->Update();

		//Message handler processes local data
		messageHandler->HandleLocalMessages(consoleWindow->GetNewMessages());

		//Print everything that has happened inside message handler.
		PrintMessageLog(messageHandler->GetMessageLog());

		//Update usermanager to remove old userdata and close old connections
		userManager->Update();

		//Server redistributes new data
		if(!server->DistributeData(messageHandler->GetOutMessages()))
		{
			return false;
		}
	}

	return true;
}

void ServerScreen::Render()
{
	rootWindow->render();
}

void ServerScreen::Exit()
{
	if(serverActive)
	{
		server->StopHosting();
	}

	serverActive = false;


	//Hide mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);

	//When exiting, hide and deactivate window
	rootWindow->hide();

}

void ServerScreen::OnSettingsReload( Config* cfg )
{
}

void ServerScreen::UpdateServerStatus(bool activate)
{
	if(activate)
	{
		//If we've just connected, there might be a bunch of messages that have been queued up. 
		//Let's get rid of those. Why? I don't really know, but it would surprise me as a user if that didn't happen.
		consoleWindow->GetNewMessages().clear();

		server->StartHosting();
	}
	else
	{
		server->StopHosting();
	}
}

void ServerScreen::ResetGUI()
{
	sidebarWindow->Reset();
	consoleWindow->Reset();
}

void ServerScreen::PrintMessageLog( std::vector<TextMessage>& log )
{
	for(unsigned int i = 0; i < log.size(); ++i)
	{
		consoleWindow->PrintText(log[i].text, log[i].textColor);
	}

	log.clear();
}
