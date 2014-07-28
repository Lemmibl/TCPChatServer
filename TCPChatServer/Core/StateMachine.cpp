#include "StateMachine.h"

#pragma once

#include "Screens/ScreenBase.h"
#include "Screens/MainMenuScreen.h"
#include "Screens/OptionsScreen.h"
#include "Screens/ClientScreen.h"
#include "Screens/ServerScreen.h"
#include "Screens/ServerScreenNoGUI.h"

#include <GLFW/glfw3.h>

StateMachine::StateMachine()
: running(true),
currentState(nullptr)
{
}

StateMachine::~StateMachine()
{
	currentState = nullptr;
}

bool StateMachine::Initialize()
{
	std::unique_ptr<MainMenuScreen> mainMenu(new MainMenuScreen());
	//std::shared_ptr<OptionsScreen> optionsMenu = std::make_shared<OptionsScreen>();
	std::unique_ptr<ServerScreen> serverMenu(new ServerScreen());
	std::unique_ptr<ClientScreen> clientMenu(new ClientScreen());
	clientMenu->Initialize();

	AddNewState(std::move(mainMenu), SystemStates::MainMenuScreen);
	//AddNewState(optionsMenu, SystemStates::OptionsScreen);
	AddNewState(std::move(serverMenu), SystemStates::ServerScreen);
	AddNewState(std::move(clientMenu), SystemStates::ClientScreen);

	
	serverNoGUI.reset(new ServerScreenNoGUI(static_cast<ClientScreen*>(states[SystemStates::ClientScreen].get())->GetConsole()));

	//Initializes and starts server
	if(!serverNoGUI->Enter())
	{
		return false;
	}

	//Set current state to main menu
	SwitchState(SystemStates::MainMenuScreen);

	return true;
}

bool StateMachine::Update()
{
	//Terminate
	if(!running)
	{
		return false;
	}

	double glfwTime = glfwGetTime();

	if(!serverNoGUI->Update(glfwTime))
	{
		return false;
	}

	//I've also made sure to have the option to let the individual states flag that the program should shut down
	if(!currentState->Update(glfwTime))
	{
		return false;
	}

	return true;
}

void StateMachine::Render()
{
	currentState->Render();
}

void StateMachine::SwitchState(SystemStates::State newState)
{
	//Special case...
	if(newState == SystemStates::Quit)
	{
		currentState = nullptr;
		running = false;
		return;
	}

	//Look for the new state in our map
	auto& getState = states.find(newState);

	//If newState has been found in our map
	if(getState != states.cend())
	{
		//Exit old state.. if there was one.
		if(currentState != nullptr)
		{
			currentState->Exit();
		}

		//Change current state to new one
		currentState = getState->second.get();

		//Aaaand... Enter
		if(!currentState->Enter())
		{
			currentState = nullptr;
			running = false;
			return;
		}
	}
	else
	{
		//Something has gone terribly wrong if we've reached this point. Better shut down.
		//std::cerr << "Tried to change to a state that doesn't exist." << std::endl;
		
		running = false;
	}
}

void StateMachine::AddNewState(std::unique_ptr<ScreenBase> screen, SystemStates::State gameState)
{
	//Technically speaking not a key, but an iterator into the map, but for all intents and purposes, I use it as a key here.
	auto& key = states.find(gameState);

	//If this key doesn't already exist in the map
	if(key == states.cend())
	{
		//Append event. This is the event that a state will throw if it wants to start transitioning to another state
		screen->GetStateChangeEvent()->Add(*this, &StateMachine::SwitchState);

		//Insert into ptr map
		states.insert(std::make_pair(gameState, std::move(screen)));
	}
}
