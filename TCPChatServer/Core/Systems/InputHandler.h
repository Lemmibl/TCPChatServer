#pragma once
#include <vector>
#include "CEGUI/CEGUI.h"

#include <GLFW/glfw3.h>

class InputHandler
{
public:
	//Don't forget to call initialize!
	static InputHandler& GetInstance()
	{
		static InputHandler handler;

		return handler;
	}

	void Initialize(GLFWwindow* window);
	void Update(double deltaTime);

	//Setters
	void SetMouseButtonState(int button, bool state);
	void SetMouseScrollState(double xDelta, double yDelta);
	void SetMousePosition(double xDelta, double yDelta);
	void SetKeyState(int keycode, bool state);
	void InjectChar(unsigned int keycode);
	void LockMouse(bool val);

	//Getters not used. Using glfw instead.
	//bool IsMouseButtonDown(int buttonCode);
	//bool IsKeyDown(int keyCode);

	//Static callback functions
	static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void GLFWMouseCursorPositionCallback(GLFWwindow* window, double xDelta, double yDelta);
	static void GLFWMouseScrollCallback(GLFWwindow* window, double xDelta, double yDelta);
	static void GLFWErrorCallback(int error, const char* description);
	static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void GLFWCharCallback(GLFWwindow* window, unsigned int codePoint);
	static void GLFWWindowResizeCallback(GLFWwindow* window, int width, int height);

private:
	//Private to make sure it's not accessible from anywhere but the GetInstance function
	InputHandler();

	unsigned int GLFWToCEGUIKey(int glfwKey);
	CEGUI::MouseButton GLFWToCEGUIButton(int glfwButton);

	//Stop the compiler from generating ways to copy the object
	InputHandler(InputHandler const& copy);            // Not Implemented
	InputHandler& operator=(InputHandler const& copy); // Not Implemented

private:
	//Ptr to window we're supposed to get updates from
	GLFWwindow* glfwWindow;

	double timeSinceLastInput;

	//This is how often we'll read for input
	const double inputCooldown;
	bool cursorLocked;

	//So, cool fact: a std::vector<bool> is actually a condensed bitfield and it's not considered a standard stl container for this reason
	//True means down, false means up
	//std::vector<bool> keyStates;
	//std::vector<bool> mouseButtonStates;

	//This will be a constantly filled/consumed vector for char injection into CEGUI
	//std::vector<unsigned int> keyEvents;
	
	//X and Y deltas
	std::pair<double, double> centerMousePos, mouseDelta;
	std::pair<double, double> mouseWheelDelta;

	double mouseSensitivity;
};