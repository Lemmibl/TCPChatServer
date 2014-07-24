#pragma once
#include <unordered_map>
#include "SystemStates.h"

class ScreenBase;
class ServerScreenNoGUI;

//Simply a state machine that I use to move from one stage of the engine to another.
//Currently we only have main menu and "running", but I want to leave room for other things in the future. For example: options screen.
class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	bool Initialize();

	bool Update();
	void Render();
	void SwitchState(SystemStates::State newState);
	bool IsRunning() { return running; }

private:
	void AddNewState(std::unique_ptr<ScreenBase> screen, SystemStates::State gameState);

private:
	bool running;

	std::unique_ptr<ServerScreenNoGUI> serverNoGUI;

	std::unordered_map<SystemStates::State, std::unique_ptr<ScreenBase>> states;

	//Use raw pointer because it's not supposed to own anything, and thus not be in charge of deleting anything either, as a smart ptr would.
	ScreenBase* currentState;
};

