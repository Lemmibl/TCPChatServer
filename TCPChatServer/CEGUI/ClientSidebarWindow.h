#pragma once
#include "CEGUI/CEGUI.h"

//The sidebar window is where the user can input stuff like username, text color, what IP to connect to etc.
class ClientSidebarWindow
{
public:
	ClientSidebarWindow();
	~ClientSidebarWindow();

	void Initialize(CEGUI::String filepath, CEGUI::Window* rootwindow);

	//Will return true if we should quit
	bool Quit() { return shouldQuit; }

	//Returns true if we're connected
	bool Connected() { return connectionActive; }

	//Call to reset all internal states and menu texts to represent the fact that we're disconnected
	void Disconnect();

	//Call every time we re-enter this window to reset states such as quit/hosting etc.
	void Reset();

	bool EventCallback_ConnectButtonPressed(const CEGUI::EventArgs &e);
	bool EventCallback_IPChanged(const CEGUI::EventArgs &e);
	bool EventCallback_NameChanged(const CEGUI::EventArgs &e);
	bool EventCallback_ColorChanged(const CEGUI::EventArgs &e);
	bool EventCallback_ShutdownButtonPressed(const CEGUI::EventArgs &e);

	const std::string GetTargetIP()	{ return std::string(targetIP.c_str()); } //std::string(targetIP.c_str())
	const CEGUI::String GetClientUserName()	{ return userName; }
	const CEGUI::argb_t  GetUserColor() { return userColor.getARGB(); }

private:
	//Our base window
	CEGUI::Window* sidebarWindow;

	//Colourpicker window for some special interactions and because it's currently broken in CEED, so we need to manually create it.
	CEGUI::Window* colorPicker;

	//Local variables to store data that is changed whenever an event is triggered.
	CEGUI::Colour userColor;
	CEGUI::String targetIP;
	CEGUI::String userName;
	bool connectionActive, shouldQuit;
};

