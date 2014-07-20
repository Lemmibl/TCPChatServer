#pragma once
#include <memory>


class CEGUIWrapper;
class StateMachine;
struct GLFWwindow;

/*
This is the core of the engine. It runs and manages the lifetime of all the sub systems, like the networking system, message parsing system, user handling system etc.
Also contains all the CEGUI initialization stuff like loading layouts, imagesets, looknfeels etc.
*/
class CoreEngine
{
public:
	CoreEngine();
	~CoreEngine();

	void Cleanup();

	//Returns false on failure
	bool Initialize();

	void MainLoop();

	//Returns false on failure.
	bool Update();

	//Draw all our GUI elements and text.
	void Render();

	bool Timer(double durationInSeconds, bool reset);

private:
	void UpdateInput();
	bool InitializeGUI();

private:
	int screenWidth, screenHeight;
	double timeSinceLastInput;
	double deltaTime, elapsedTime, glfwTime;

	//This is how often we'll read for input
	const double inputCooldown;
	bool cursorLocked;

	GLFWwindow* glfwWindow;
	std::unique_ptr<StateMachine> stateMachine;
	std::unique_ptr<CEGUIWrapper> ceguiWrapper;
};