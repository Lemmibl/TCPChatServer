#include "ClientScreen.h"

#include "GLFW/glfw3.h"

#include "../Systems/ClientMessageHandler.h"
#include "../../Network/TCPClient.h"
#include "../../CEGUI/GameConsoleWindow.h"
#include "../../CEGUI/ClientSidebarWindow.h"

#include "../../CEGUI/CEGUIMessageBox.h"

ClientScreen::ClientScreen() : ScreenBase()
{
}

ClientScreen::~ClientScreen()
{
	if(chatClient != nullptr)
	{
		chatClient->Shutdown();
	}
}

bool ClientScreen::Enter()
{
	if(!isInitialized)
	{
		if(!Initialize())
		{
			return false;
		}

		isInitialized = true;
	}
	else
	{
		ResetGUI();
	}

	//Show mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);

	//Set this base window as root.
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWindow);

	rootWindow->activate();
	rootWindow->show();

	return true;
}


bool ClientScreen::InitializeGUI()
{
	rootWindow = CEGUI::WindowManager::getSingletonPtr()->loadLayoutFromFile("CustomLayouts/ColoredClientBackground.layout");

	//This makes sure that we don't get collisions if we try to load the coloredbackground layout somewhere else.
	rootWindow->setName("ClientBackground");
	rootWindow->activate();
	rootWindow->setAlwaysOnTop(false);

	//Create console window
	consoleWindow.reset(new GameConsoleWindow);

	//Load the layout and send in a root handle when initializing the window
	consoleWindow->CreateCEGUIWindow("CustomLayouts/Console.layout", rootWindow);

	//Create the sidebar window (it'll handle stuff like changing your user name or entering target ip address)
	sidebarWindow.reset(new ClientSidebarWindow);
	sidebarWindow->Initialize("CustomLayouts/ClientSidebar.layout", rootWindow);

	return true;
}


bool ClientScreen::Initialize()
{
	connectionActive = false;
	glfwWindow = glfwGetCurrentContext();
	rootWindow = nullptr;

	userData.textColor = 0xFFFF0000;
	userData.userName = "Burt";

	//Make sure to call before initializing classes that use consoleWindow
	InitializeGUI();

	//Create the system that will manage all the data we receive/send through the network
	messageHandler.reset(new ClientMessageHandler(consoleWindow.get()));

	//Create our client but don't connect or start anything yet
	chatClient.reset(new TCPClient(consoleWindow.get()));

	return true;
}

bool ClientScreen::Update( double deltaTime )
{
	//If escape was pressed or we've pressed the shutdown button on the sidebar, go back to main menu
	if(glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) || sidebarWindow->Quit())
	{
		stateChangeEvent(SystemStates::MainMenuScreen);
	}

	//If something has changed
	if(connectionActive != sidebarWindow->Connected())
	{
		//Save results for next time
		connectionActive = sidebarWindow->Connected();

		//Use results
		UpdateConnectionStatus(connectionActive);
	}

	if(connectionActive)
	{
		//Receive data from the server. If function returns false, it means something went wrong or that we were disconnected from the host (on purpose).
		connectionActive = chatClient->ReceiveData(messageHandler->GetInMessageQueue());

		//Get any messages we've written to the console window
		auto& localMessages = consoleWindow->GetNewMessages();

		//Process all local messages
		for(unsigned int i = 0; i < localMessages.size(); ++i)
		{
			messageHandler->SendTextPacket(localMessages[i], true, sidebarWindow->GetUserColor());
		}

		//Process external messages from server through inMessageQueue
		messageHandler->Update();

		//Send any data we've queued up
		connectionActive = chatClient->SendDataToServer(messageHandler->GetOutMessageQueue());
	}

	return true;
}

void ClientScreen::Render()
{
	//rootWindow->render();
}

void ClientScreen::Exit()
{
	//Hide mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);

	//When exiting, hide and deactivate window
	rootWindow->hide();

	//Close connection
	if(connectionActive)
	{
		chatClient->Disconnect();
	}
}

void ClientScreen::UpdateConnectionStatus(bool active)
{
	//Can't get much simpler than this
	if(active)
	{
		//If we've just connected, there might be a bunch of messages that have been queued up. 
		//Let's get rid of those. Why? I don't really know, but it would surprise me as a user if that didn't happen.
		consoleWindow->GetNewMessages().clear();

		if(chatClient->Connect(sidebarWindow->GetTargetIP()))
		{
			userData.userName = sidebarWindow->GetUserName();
			userData.textColor = sidebarWindow->GetUserColor();

			messageHandler->SendUserDataPacket(userData);
		}
		else
		{
			CEGUIMessageBox::CreateMessageBox("Couldn't connect to the server. Did you write the right IP?");

			active = false;
			sidebarWindow->Disconnect();
		}
	}
	else
	{
		chatClient->Disconnect();
	}
}

void ClientScreen::ResetGUI()
{
	sidebarWindow->Reset();
	consoleWindow->Reset();
}
