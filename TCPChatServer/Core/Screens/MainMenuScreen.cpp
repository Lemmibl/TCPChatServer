#include "MainMenuScreen.h"

MainMenuScreen::MainMenuScreen()
: ScreenBase()
{
	rootWindow = nullptr;
}


MainMenuScreen::~MainMenuScreen()
{
}


bool MainMenuScreen::Enter()
{
	if(!isInitialized)
	{
		if(!Initialize())
		{
			return false;
		}
	}

	//Show mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);

	//Set this base window as root.
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWindow);

	rootWindow->activate();
	rootWindow->show();

	return true;
}


bool MainMenuScreen::Initialize()
{
	//Load the layout for this menu
	rootWindow = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("CustomLayouts/MainMenu.layout");
	if(!rootWindow)
	{
		return false;
	}
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWindow);

	//#################################### Button #1 #################################################

	//Set up start game function and bind it to the options button
	auto& startServerFunction = 
		[&](const CEGUI::EventArgs& args) -> bool
	{
		//Throws an event that will be caught by screenManager, signalling to quit the game
		stateChangeEvent(SystemStates::ServerScreen);

		return true;
	};

	//Set up the button for starting the server
	CEGUI::Window* startServer = rootWindow->getChild("Start server");
	if(!startServer)
	{
		return false;
	}

	//Connect the click button event to the proper function (in this case a lambda)
	startServer->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::SubscriberSlot::SubscriberSlot(startServerFunction));

	//#################################### Button #2 #################################################

	//Set up start game function and bind it to the options button
	auto& startClientFunction = 
		[&](const CEGUI::EventArgs& args) -> bool
	{
		//Throws an event that will be caught by screenManager, signalling to quit the game
		stateChangeEvent(SystemStates::ClientScreen);

		return true;
	};

	//Set up the button for starting the server
	CEGUI::Window* joinChat = rootWindow->getChild("Join chatroom");
	if(!joinChat)
	{
		return false;
	}

	//Connect the click button event to the proper function (in this case a lambda)
	joinChat->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::SubscriberSlot::SubscriberSlot(startClientFunction));


	//#################################### Button #3 #################################################

	//Set up open options function and bind it to the Options button
	auto& openOptionsScreenFunction = 
		[&](const CEGUI::EventArgs& args) -> bool
	{
		//Throws an event that will be caught by the state machine, signalling to change to server screen
		stateChangeEvent(SystemStates::OptionsScreen);

		return true;
	};

	CEGUI::Window* optionsScreen = rootWindow->getChild("Options");
	if(!optionsScreen)
	{
		return false;
	}

	//Connect the click button event to the proper function (in this case a lambda)
	optionsScreen->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::SubscriberSlot::SubscriberSlot(openOptionsScreenFunction));

	//#################################### Button #4 #################################################

	//Set up quit function and bind it to Exit button event
	auto& quitFunction = 
		[&](const CEGUI::EventArgs& args) -> bool
	{
		//Throws an event that will be caught by screenManager, signalling to quit the game
		stateChangeEvent(SystemStates::Quit);

		return true;
	};

	//Guess what this button does!
	CEGUI::Window* quitGame = rootWindow->getChild("Exit");
	if(!quitGame)
	{
		return false;
	}

	//Connect the click button event to the proper function (in this case a lambda)
	quitGame->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::SubscriberSlot::SubscriberSlot(quitFunction));


	return true;
}


bool MainMenuScreen::Update(double deltaTime)
{
	return true;
}


void MainMenuScreen::Render()
{
	
}


void MainMenuScreen::Exit()
{
	//Hide mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);

	//When exiting, hide and deactivate window
	rootWindow->deactivate();
	rootWindow->hide();
}
