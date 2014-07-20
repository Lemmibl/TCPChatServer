#pragma once
#include "../../Events/SchladetschEvents.h"
#include "../SystemStates.h"

//Just supposed to be an interface class for every class that wants GUI elements loaded/unloaded when entered/exited
class ScreenBase
{
public:
	ScreenBase()
	: isInitialized(false)
	{
	}

	//Virtual because base class
	virtual ~ScreenBase(){};
	
	virtual bool Enter() = 0;
	virtual bool Initialize() = 0;
	virtual bool Update(double deltaTime) = 0;
	virtual void Render() = 0;
	virtual void Exit() = 0;

	Schladetsch::Events::Event<SystemStates::State>* GetStateChangeEvent() { return &stateChangeEvent; }

protected:
	//Event that we throw when we want to exit out of this state
	Schladetsch::Events::Event<SystemStates::State> stateChangeEvent;

	//K, so the deal with this bool is that when we first Enter() a class, we want to call initialize on whatever GUI elements.
	//But subsequent enters shouldn't reinitialize; that would be wasteful. So instead we just do some... activation or resetting or whatever is needed.
	bool isInitialized;
};