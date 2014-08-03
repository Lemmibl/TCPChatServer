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
	//Save a ptr to handler to save a few instructions on all those ::GetInstance() calls
	inputHandler = &InputHandler::GetInstance(); 

	if(!InitializeGUI())
	{
		return false;
	}

	stateMachine = std::unique_ptr<StateMachine>(new StateMachine);

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
	glfwTime = glfwGetTime();
	deltaTime = glfwTime - deltaTime;

	inputHandler->Update(deltaTime);

	//Update all CEGUI elements. Inject delta time.
	ceguiWrapper->Update(deltaTime);

	//Update currently active screen/state	
	if(!stateMachine->Update())
	{
		return false;
	}

	//See if we should shut down application
	if(glfwWindowShouldClose(glfwWindow))
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

	stateMachine->Render();

	glfwSwapBuffers(glfwWindow);	
	glfwPollEvents();
}