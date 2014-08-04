#include "EngineCore.h"

#include "StateMachine.h"
#include "../CEGUI/CEGUIWrapper.h"
#include "Systems/InputHandler.h"
#include "../Settings/SettingsManager.h"

#include <GLFW/glfw3.h>

//Initializing width and height to some values. These will actually be filled later in LoadSettings.
EngineCore::EngineCore()
	: screenWidth(1024), screenHeight(768)
{
	deltaTime = 0.0;
	glfwTime = 0.0;
}

EngineCore::~EngineCore()
{
	Cleanup();
	inputHandler = nullptr;
	glfwWindow = nullptr;
}

void EngineCore::Cleanup()
{
	glfwDestroyWindow(glfwWindow);
	glfwTerminate();
}

bool EngineCore::Initialize()
{
	//Save a ptr to handler to save a few instructions on all those ::GetInstance() calls that happen in the update loop.
	inputHandler = &InputHandler::GetInstance(); 

	if(!InitializeGUI())
	{
		return false;
	}

	stateMachine.reset(new StateMachine);

	if(!stateMachine->Initialize())
	{
		return false;
	}

	return true;
}

bool EngineCore::InitializeGUI()
{
	//Bind error callback function first of all, so that it gets called if anything goes wrong
	glfwSetErrorCallback(InputHandler::GLFWErrorCallback);

	/* Initialize the library */
	if(!glfwInit())
	{
		return false;
	}

	/* Create a windowed mode window and its OpenGL context */
	glfwWindow = glfwCreateWindow(screenWidth, screenHeight, "A window", NULL, NULL);
	if(!glfwWindow)
	{
		glfwTerminate();
		return false;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(glfwWindow);

	//Bind all the callback functions that GLFW calls upon events
	glfwSetKeyCallback(glfwWindow, InputHandler::GLFWKeyCallback);
	glfwSetCharCallback(glfwWindow, InputHandler::GLFWCharCallback);
	glfwSetMouseButtonCallback(glfwWindow, InputHandler::GLFWMouseButtonCallback);
	glfwSetScrollCallback(glfwWindow, InputHandler::GLFWMouseScrollCallback);
	glfwSetCursorPosCallback(glfwWindow, InputHandler::GLFWMouseCursorPositionCallback);
	glfwSetWindowSizeCallback(glfwWindow, InputHandler::GLFWWindowResizeCallback);

	//Make sure inputhandler has a valid ptr to the current window.
	inputHandler->Initialize(glfwWindow);

	//Create CEGUI wrapper object
	ceguiWrapper = std::unique_ptr<CEGUIWrapper>(new CEGUIWrapper());

	//Initialize CEGUI
	if(!ceguiWrapper->Initialize())
	{
		return false;
	}

	return true;
}

void EngineCore::MainLoop()
{
	for(;;)
	{
		if(!Update())
		{
			return;
		}

		Render();
	}
}

bool EngineCore::Update()
{
	/*
	http://www.glfw.org/GLFWReference276.pdf
	 
	This function is used for polling for events, such as user input and window resize events. Upon calling
	this function, all window states, keyboard states and mouse states are updated. If any related callback
	functions are registered, these are called during the call to glfwPollEvents.
	*/
	glfwPollEvents();

	//See if we should shut down application
	if(glfwWindowShouldClose(glfwWindow))
	{
		return false;
	}

	glfwTime = glfwGetTime();
	deltaTime = glfwTime - deltaTime;

	//Inject delta time, used for things like input cooldowns (making sure we don't register every click/press 500 times per second).
	inputHandler->Update(deltaTime);

	//Update all CEGUI elements. Inject delta time.
	ceguiWrapper->Update(deltaTime);

	//Update currently active screen/state	
	if(!stateMachine->Update())
	{
		return false;
	}

	//Save for next time
	deltaTime = glfwTime;

	return true;
}


void EngineCore::Render()
{
	//Render all GUI stuff
	ceguiWrapper->Render();

	// Render whatever state is currently active. 
	// Not even sure if this is needed because all the states are just different menus, and they should get rendered when ceguiwrapper::render is called.
	// I keep it in for future proofing.
	stateMachine->Render();

	//glfw stuff
	glfwSwapBuffers(glfwWindow);	
}