#pragma once
#include "CEGUI/CEGUI.h"

class UserManager;
class ColourPicker;
struct ChatUserData;

class ServerSidebarWindow
{
public:
	ServerSidebarWindow();
	~ServerSidebarWindow();

	bool Initialize(CEGUI::String filepath, CEGUI::Window* const rootWindow, ChatUserData& ourUserData);

	bool EventCallback_HostButtonPressed(const CEGUI::EventArgs &e);
	bool EventCallback_ShutdownPressed(const CEGUI::EventArgs &e);
	bool EventCallback_TextColorChanged(const CEGUI::EventArgs &e);
	bool EventCallback_UserNameChanged(const CEGUI::EventArgs &e);

	bool IsHostingActive()	{ return hostingActive; }
	bool Quit()				{ return shouldQuit; }

	//Call every time we re-enter this window to reset states such as quit/hosting etc.
	void Reset();

private:
	ChatUserData* hostUserData;

	//Our base window
	CEGUI::Window* sidebarWindow;

	//Colourpicker window for some special interactions and because it's currently broken in CEED, so we need to manually create it.
	CEGUI::Window* colorPicker;

	//Connected to our Start Hosting / Stop Hosting button
	bool hostingActive, shouldQuit;
};

