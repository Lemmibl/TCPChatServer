#pragma once
#include <vector>
#include "CEGUI/CEGUI.h"

#include <GLFW/glfw3.h>

class InputSingleton
{
public:
	static InputSingleton& GetInstance()
	{
		static InputSingleton inputClass;

		return inputClass;
	}

	void SetGLFWWindow(GLFWwindow* val) { glfwWindow = val; }

	//Getters
	//bool IsMouseButtonDown(int buttonCode);
	//bool IsKeyDown(int keyCode);

	//Setters
	void SetMouseButtonState(int button, bool state);
	void SetMouseScrollState(double xDelta, double yDelta);
	void SetMousePosition(double xDelta, double yDelta);
	void SetKeyState(int keycode, bool state);
	void InjectChar(unsigned int keycode);

	void LockMouse(bool val);

	//Static callback functions
	static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void GLFWMouseCursorPositionCallback(GLFWwindow* window, double xDelta, double yDelta);
	static void GLFWMouseScrollCallback(GLFWwindow* window, double xDelta, double yDelta);
	static void GLFWErrorCallback(int error, const char* description);
	static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void GLFWCharCallback(GLFWwindow* window, unsigned int codePoint);
	static void GLFWWindowResizeCallback(GLFWwindow* window, int width, int height);

private:
	unsigned int GLFWToCEGUIKey(int glfwKey);
	CEGUI::MouseButton GLFWToCEGUIButton(int glfwButton);

	//Private constructor
	InputSingleton();

	//Stop the compiler from generating ways to copy the object
	InputSingleton(InputSingleton const& copy);            // Not Implemented
	InputSingleton& operator=(InputSingleton const& copy); // Not Implemented

private:
	//Ptr to window we're supposed to get updates from
	GLFWwindow* glfwWindow;

	//So, cool fact: a std::vector<bool> is actually a condensed bitfield and it's not considered a standard stl container for this reason
	//True means down, false means up
	//std::vector<bool> keyStates;
	//std::vector<bool> mouseButtonStates;

	//This will be a constantly filled/consumed vector for char injection into CEGUI
	//std::vector<unsigned int> keyEvents;
	
	//X and Y deltas
	std::pair<double, double> centerMousePos, mouseDelta;
	std::pair<double, double> mouseWheelDelta;

	bool mouseLocked;
	double mouseSensitivity;
};