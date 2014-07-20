#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "Core/CoreEngine.h"

int main(int argc, char* argv[])
{
//http://msdn.microsoft.com/en-us/library/ms683150(v=vs.85).aspx
//Fix to close the empty console window that appears when you start glfw on a win32 system... I think.
//http://stackoverflow.com/questions/5995433/removing-console-window-for-glut-freeglut-glfw
#ifdef _WIN32
	FreeConsole();
#endif

	CoreEngine engine;
	bool result;

	// Initialize and run the system object.
	result = engine.Initialize();

	if(result)
	{
		//If initialize went through properly, enter main loop and stay there until the application ends
		engine.MainLoop();

		//After we leave mainloop, do cleanup
		engine.Cleanup();
	}
	else
	{
		engine.Cleanup();
		return -1;
	}
	
	return 0;
}