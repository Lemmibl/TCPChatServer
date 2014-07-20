#include "ServerSidebarWindow.h"
#include "CEGUI/CommonDialogs/ColourPicker/ColourPicker.h"
#include "CEGUI/CommonDialogs/ColourPicker/Controls.h"
#include "../Network/ChatUserData.h"

ServerSidebarWindow::ServerSidebarWindow()
{
	hostingActive = false;
	shouldQuit = false;
}


ServerSidebarWindow::~ServerSidebarWindow()
{
	hostingActive = false;
	shouldQuit = false;
}

bool ServerSidebarWindow::Initialize(CEGUI::String filepath, CEGUI::Window* const rootWindow, ChatUserData& ourUserData)
{
	hostUserData = &ourUserData;

	// Get a local pointer to the CEGUI Window Manager, Purely for convenience to reduce typing
	CEGUI::WindowManager* pWindowManager = CEGUI::WindowManager::getSingletonPtr();

	// Now that we can ensure that we have a safe prefix, and won't have any naming conflicts lets create the window
	// and assign it to our member window pointer m_ConsoleWindow
	// inLayoutName is the name of your layout file (for example "console.layout"), don't forget to rename inLayoutName by our layout file
	sidebarWindow = pWindowManager->loadLayoutFromFile(filepath);

	//If there's a legit root window higher up
	if(rootWindow != nullptr)
	{
		//Then we add this window as child to that window instead
		rootWindow->addChild(sidebarWindow);
	}

	//Create and configure the colour picker
	colorPicker = static_cast<CEGUI::ColourPicker*>(pWindowManager->createWindow("Vanilla/ColourPicker"));
	colorPicker->setPosition(CEGUI::UVector2(CEGUI::UDim(0.38f, 0.0f), CEGUI::UDim(0.1f, 0.0f)));
	colorPicker->setSize(CEGUI::USize(CEGUI::UDim(0.5f, 0.0f), CEGUI::UDim(0.04f, 0.0f)));
	colorPicker->setName("ServerColorpicker");
	static_cast<CEGUI::ColourPicker*>(colorPicker)->setColour(CEGUI::Colour(1.0f, 1.0f, 1.0f));

	//Finally add colour picker as child to this window
	sidebarWindow->addChild(colorPicker);

	//Subscribe to colour accepted event, call Handle_TextColourChanged when event is thrown.
	colorPicker->subscribeEvent(CEGUI::ColourPicker::EventAcceptedColour, CEGUI::Event::Subscriber(&ServerSidebarWindow::EventCallback_TextColorChanged, this));

	sidebarWindow->getChild("HostButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(&ServerSidebarWindow::EventCallback_HostButtonPressed, this));
	sidebarWindow->getChild("ShutdownButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ServerSidebarWindow::EventCallback_ShutdownPressed, this));
	sidebarWindow->getChild("NameEditbox")->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&ServerSidebarWindow::EventCallback_UserNameChanged, this));

	//Set default color
	hostUserData->textColor = static_cast<CEGUI::ColourPicker*>(colorPicker)->getColour().getARGB();

	//Set default username to.... Whatever is in the EditBox at start.
	hostUserData->userName = sidebarWindow->getChild("NameEditbox")->getText();
	
	return true;
}

bool ServerSidebarWindow::EventCallback_HostButtonPressed(const CEGUI::EventArgs &e)
{
	//If server isn't active
	if(!hostingActive)
	{
		//It is now
		hostingActive = true;

		//And we change connect button text to disconnect instead.
		sidebarWindow->getChild("HostButton")->setText("Stop hosting");
	}
	else
	{
		hostingActive = false;

		//If we've pressed our Disconnect button, we've deactivated the server
		sidebarWindow->getChild("HostButton")->setText("Start hosting");
	}

	return true;
}

bool ServerSidebarWindow::EventCallback_ShutdownPressed( const CEGUI::EventArgs &e )
{
	shouldQuit = true;

	return true;
}

bool ServerSidebarWindow::EventCallback_TextColorChanged(const CEGUI::EventArgs &e)
{
	hostUserData->textColor = static_cast<CEGUI::ColourPicker*>(colorPicker)->getColour().getARGB();

	return true;
}

bool ServerSidebarWindow::EventCallback_UserNameChanged(const CEGUI::EventArgs &e)
{
	const CEGUI::WindowEventArgs* args = static_cast<const CEGUI::WindowEventArgs*>(&e);

	//Retrieve text that was just entered
	hostUserData->userName = args->window->getText();

	//To prevent "empty" names.
	if(hostUserData->userName.size() == 0)
	{
		hostUserData->userName = "Admin";
	}

	return true;
}

void ServerSidebarWindow::Reset()
{
	hostingActive = false;
	shouldQuit = false;

	sidebarWindow->getChild("HostButton")->setText("Start hosting");

	sidebarWindow->getChild("NameEditbox")->setText("Admin");
	hostUserData->userName = sidebarWindow->getChild("NameEditbox")->getText();

	static_cast<CEGUI::ColourPicker*>(colorPicker)->setColour(CEGUI::Colour(1.0f, 1.0f, 1.0f));
	hostUserData->textColor = static_cast<CEGUI::ColourPicker*>(colorPicker)->getColour().getARGB();
}

/* This is what the layout file looks like: */

/*
<?xml version="1.0" encoding="UTF-8"?>

<GUILayout version="4" >
<Window type="TaharezLook/FrameWindow" name="Sidebar" >
<Property name="Area" value="{{0,0},{0,0},{0.4,0},{1,0}}" />
<Property name="FrameEnabled" value="False" />
<Property name="SizingEnabled" value="False" />
<Property name="TitlebarEnabled" value="False" />
<Property name="DragMovingEnabled" value="False" />
<Property name="CloseButtonEnabled" value="False" />
<AutoWindow namePath="__auto_titlebar__" >
<Property name="DraggingEnabled" value="False" />
</AutoWindow>
<Window type="TaharezLook/Label" name="ColorSelectionWidget" >
<Property name="Area" value="{{0,0},{0.1,0},{0.35,0},{0.14,0}}" />
<Property name="Text" value="Your text color:" />
<Property name="MaxSize" value="{{1,0},{1,0}}" />
<Property name="HorzFormatting" value="RightAligned" />
</Window>
<Window type="TaharezLook/Editbox" name="NameEditbox" >
<Property name="Area" value="{{0.38,0},{0.03,0},{0.88,0},{0.07,0}}" />
<Property name="Text" value="Admin" />
<Property name="BlinkCaret" value="True" />
</Window>
<Window type="TaharezLook/Button" name="ShutdownButton" >
<Property name="Area" value="{{-0.32,0},{0.38,0},{-0.02,0},{0.44,0}}" />
<Property name="Text" value="Shutdown" />
<Property name="VerticalAlignment" value="Centre" />
<Property name="HorizontalAlignment" value="Centre" />
</Window>
<Window type="TaharezLook/Button" name="HostButton" >
<Property name="Area" value="{{0.18,0},{0.38,0},{0.78,0},{0.44,0}}" />
<Property name="Text" value="Start Hosting" />
<Property name="VerticalAlignment" value="Centre" />
<Property name="HorizontalAlignment" value="Centre" />
</Window>
<Window type="TaharezLook/Label" name="NameLabel" >
<Property name="Area" value="{{0,0},{0.03,0},{0.35,0},{0.07,0}}" />
<Property name="Text" value="Your name:" />
<Property name="MaxSize" value="{{1,0},{1,0}}" />
<Property name="HorzFormatting" value="RightAligned" />
</Window>
<Window type="TaharezLook/Label" name="PortDescriptionText" >
<Property name="Area" value="{{0,0},{0.78,13},{0.75,0},{0.82,13}}" />
<Property name="Text" value="Port is currently locked to: 22222" />
<Property name="MaxSize" value="{{1,0},{1,0}}" />
<Property name="HorizontalAlignment" value="Centre" />
</Window>
</Window>
</GUILayout>

*/