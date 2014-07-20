#pragma once

//Using this little hack to enforce readability. 
//Anonymous enum within a namespace enforces something close enough to enum classes, which haven't been implemented in vs2010.
//It means that instead of just writing "MainMenuScreen", you have to qualify it like this: "SystemStates::MainMenuScreen".
//To qualify the actual type for storing/parameters, you use SystemStates::State. Which might actually be pretty silly.
namespace SystemStates
{
	//Typedef an int to State so that I can specify SystemStates::State as a parameter/variable.
	typedef int State;

	//Anonymous enum
	enum
	{
		MainMenuScreen = 0,
		ServerScreen,
		ClientScreen,
		OptionsScreen,
		Quit //It's here so that we can do SwitchState(Quit);
	};
}
