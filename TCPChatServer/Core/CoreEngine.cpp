#include "CoreEngine.h"

#include "StateMachine.h"
#include "../CEGUI/CEGUIWrapper.h"
#include "Systems/InputSingleton.h"
#include <ctime>

#include <GLFW/glfw3.h>

//Initializing width and height to some values. These will actually be filled later in LoadSettings.
CoreEngine::CoreEngine()
	: screenWidth(1024), screenHeight(768), inputCooldown(0.2)
{
	cursorLocked = false;
	deltaTime = 0.0;
	glfwTime = 0.0;

	//Arbitrary value to make sure we poll for input immediately
	timeSinceLastInput = 1000.0;
}

CoreEngine::~CoreEngine()
{
}

void CoreEngine::Cleanup()
{

	glfwDestroyWindow(glfwWindow);

	glfwTerminate();
}

bool CoreEngine::Initialize()
{
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

bool CoreEngine::InitializeGUI()
{
	//Bind error callback function first of all, so that it gets called if anything goes wrong
	glfwSetErrorCallback(InputSingleton::GLFWErrorCallback);

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
	glfwSetKeyCallback(glfwWindow, InputSingleton::GLFWKeyCallback);
	glfwSetCharCallback(glfwWindow, InputSingleton::GLFWCharCallback);
	glfwSetMouseButtonCallback(glfwWindow, InputSingleton::GLFWMouseButtonCallback);
	glfwSetScrollCallback(glfwWindow, InputSingleton::GLFWMouseScrollCallback);
	glfwSetCursorPosCallback(glfwWindow, InputSingleton::GLFWMouseCursorPositionCallback);
	glfwSetWindowSizeCallback(glfwWindow, InputSingleton::GLFWWindowResizeCallback);

	InputSingleton::GetInstance().SetGLFWWindow(glfwWindow);

	//Create CEGUI wrapper object
	ceguiWrapper = std::unique_ptr<CEGUIWrapper>(new CEGUIWrapper());

	//Initialize CEGUI
	if(!ceguiWrapper->Initialize())
	{
		return false;
	}

	return true;
}

void CoreEngine::MainLoop()
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

bool CoreEngine::Update()
{
	//Update currently active screen/state	
	if(!stateMachine->Update())
	{
		return false;
	}

	UpdateInput();

	//Update all CEGUI elements. Inject delta time.
	ceguiWrapper->Update(deltaTime);

	//See if we should shut down application
	if(glfwWindowShouldClose(glfwWindow))
	{
		return false;
	}

	return true;
}

void CoreEngine::UpdateInput()
{
	timeSinceLastInput += deltaTime;

	//Toggle if RMB was pressed
	if(timeSinceLastInput >= inputCooldown && glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		timeSinceLastInput = 0.0;
		cursorLocked = !cursorLocked;

		InputSingleton::GetInstance().LockMouse(cursorLocked);
	}
}

void CoreEngine::Render()
{
	//Render all GUI stuff
	ceguiWrapper->Render();

	stateMachine->Render();

	glfwSwapBuffers(glfwWindow);	
	glfwPollEvents();
}