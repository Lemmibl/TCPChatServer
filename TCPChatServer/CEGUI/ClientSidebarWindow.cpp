#include "ClientSidebarWindow.h"
#include "CEGUI/CommonDialogs/ColourPicker/ColourPicker.h"
#include "CEGUI/CommonDialogs/ColourPicker/Controls.h"

ClientSidebarWindow::ClientSidebarWindow()
{
	shouldQuit = false;
	connectionActive = false;
}


ClientSidebarWindow::~ClientSidebarWindow()
{
}

void ClientSidebarWindow::Initialize(CEGUI::String filepath, CEGUI::Window* rootwindow )
{
	// Get a local pointer to the CEGUI Window Manager, Purely for convenience to reduce typing
	CEGUI::WindowManager* pWindowManager = CEGUI::WindowManager::getSingletonPtr();

	// Now that we can ensure that we have a safe prefix, and won't have any naming conflicts lets create the window
	// and assign it to our member window pointer m_ConsoleWindow
	// inLayoutName is the name of your layout file (for example "console.layout"), don't forget to rename inLayoutName by our layout file
	sidebarWindow = pWindowManager->loadLayoutFromFile(filepath);

	//If there's a legit root window higher up
	if(rootwindow != nullptr)
	{
		//Then we add this window as child to that window instead
		rootwindow->addChild(sidebarWindow);
	}

	//Create and configure the colour picker
	colorPicker = static_cast<CEGUI::ColourPicker*>(pWindowManager->createWindow("Vanilla/ColourPicker"));
	colorPicker->setPosition(CEGUI::UVector2(CEGUI::UDim(0.38f, 0.0f), CEGUI::UDim(0.1f, 0.0f)));
	colorPicker->setSize(CEGUI::USize(CEGUI::UDim(0.5f, 0.0f), CEGUI::UDim(0.04f, 0.0f)));
	colorPicker->setName("ClientColorpicker");
	static_cast<CEGUI::ColourPicker*>(colorPicker)->setColour(CEGUI::Colour(1.0f, 1.0f, 1.0f));

	//Finally add colour picker as child to this window
	sidebarWindow->addChild(colorPicker);

	//Hook up various events to their respective callback functions
	sidebarWindow->getChild("ConnectButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ClientSidebarWindow::EventCallback_ConnectButtonPressed, this));
	sidebarWindow->getChild("ShutdownButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ClientSidebarWindow::EventCallback_ShutdownButtonPressed, this));
	sidebarWindow->getChild("NameEditbox")->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&ClientSidebarWindow::EventCallback_NameChanged, this));
	sidebarWindow->getChild("IPEditbox")->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&ClientSidebarWindow::EventCallback_IPChanged, this));

	//Subscribe to colour accepted event, call Handle_TextColourChanged when event is thrown.
	colorPicker->subscribeEvent(CEGUI::ColourPicker::EventAcceptedColour, CEGUI::Event::Subscriber(&ClientSidebarWindow::EventCallback_ColorChanged, this));

	sidebarWindow->setClippedByParent(false);
	sidebarWindow->setAlwaysOnTop(true);

	//Set up default values.
	userColor = 0xFFFFFFFF;
	userName = sidebarWindow->getChild("NameEditbox")->getText();
	targetIP = sidebarWindow->getChild("IPEditbox")->getText();
}

bool ClientSidebarWindow::EventCallback_ConnectButtonPressed( const CEGUI::EventArgs &e )
{
	//Toggle
	connectionActive = !connectionActive;

	//Depending on state, adjust the button text
	if(connectionActive == true)
	{
		sidebarWindow->getChild("ConnectButton")->setText("Disconnect");
	}
	else
	{
		sidebarWindow->getChild("ConnectButton")->setText("Connect");
	}

	return true;
}

bool ClientSidebarWindow::EventCallback_IPChanged( const CEGUI::EventArgs &e )
{
	unsigned int textSize = sidebarWindow->getChild("IPEditbox")->getText().size();

	if(textSize > 0)
	{
		targetIP.resize(textSize);

		//Manually copy shit by value
		for(unsigned int i = 0; i < textSize; ++i)
		{
			targetIP[i] = sidebarWindow->getChild("IPEditbox")->getText()[i];
		}
	}

	//Reset text.
	//sidebarWindow->getChild("IPEditbox")->setText(""); //Maybe we shouldnt change name.
	return true;
}

bool ClientSidebarWindow::EventCallback_NameChanged( const CEGUI::EventArgs &e )
{
	unsigned int textSize = sidebarWindow->getChild("NameEditbox")->getText().size();

	if(textSize > 0)
	{
		userName.resize(textSize);

		//Manually copy shit by value
		for(unsigned int i = 0; i < textSize; ++i)
		{
			userName[i] = sidebarWindow->getChild("NameEditbox")->getText()[i];
		}
	}

	//Reset text.
	//sidebarWindow->getChild("NameEditbox")->setText(""); //Doesn't feel right when using it this way.
	return true;
}

bool ClientSidebarWindow::EventCallback_ShutdownButtonPressed( const CEGUI::EventArgs &e )
{
	//Whenever this button is pressed, we should quit...
	shouldQuit = true;

	return true;
}

bool ClientSidebarWindow::EventCallback_ColorChanged( const CEGUI::EventArgs &e )
{
	userColor = static_cast<CEGUI::ColourPicker*>(colorPicker)->getColour();

	return true;
}

void ClientSidebarWindow::Reset()
{
	connectionActive = false;
	shouldQuit = false;

	sidebarWindow->getChild("NameEditbox")->setText("A noob");
	userName = sidebarWindow->getChild("NameEditbox")->getText();

	sidebarWindow->getChild("IPEditbox")->setText("127.0.0.1");
	targetIP = sidebarWindow->getChild("IPEditbox")->getText();

	static_cast<CEGUI::ColourPicker*>(colorPicker)->setColour(CEGUI::Colour(1.0f, 1.0f, 1.0f));
	userColor = static_cast<CEGUI::ColourPicker*>(colorPicker)->getColour().getARGB();
}

void ClientSidebarWindow::Disconnect()
{
	connectionActive = false;
	sidebarWindow->getChild("ConnectButton")->setText("Connect");
}

/* This is what the layout file looks like: */

/*
<?xml version="1.0" encoding="UTF-8"?>

<GUILayout version="4" >
<Window type="TaharezLook/FrameWindow" name="FrameWindow" >
<Property name="Area" value="{{0,0},{0,0},{0.4,0},{1,0}}" />
<Property name="FrameEnabled" value="False" />
<Property name="SizingEnabled" value="False" />
<Property name="TitlebarEnabled" value="False" />
<Property name="DragMovingEnabled" value="False" />
<Property name="CloseButtonEnabled" value="False" />
<AutoWindow namePath="__auto_titlebar__" >
<Property name="DraggingEnabled" value="False" />
</AutoWindow>
<Window type="TaharezLook/Label" name="PortDescriptionText" >
<Property name="Area" value="{{0,0},{0.7,0},{0.75,0},{0.74,0}}" />
<Property name="Text" value="Port is currently locked to: 22222" />
<Property name="MaxSize" value="{{1,0},{1,0}}" />
<Property name="HorizontalAlignment" value="Centre" />
</Window>
<Window type="TaharezLook/Button" name="ShutdownButton" >
<Property name="Area" value="{{-0.32,0},{0.42,0},{-0.02,0},{0.48,0}}" />
<Property name="Text" value="Shutdown" />
<Property name="VerticalAlignment" value="Centre" />
<Property name="HorizontalAlignment" value="Centre" />
</Window>
<Window type="TaharezLook/Label" name="NameLabel" >
<Property name="Area" value="{{0,0},{0.03,0},{0.35,0},{0.07,0}}" />
<Property name="Text" value="Your name:" />
<Property name="MaxSize" value="{{1,0},{1,0}}" />
<Property name="HorzFormatting" value="RightAligned" />
</Window>
<Window type="TaharezLook/Editbox" name="NameEditbox" >
<Property name="Area" value="{{0.38,0},{0.03,0},{0.88,0},{0.07,0}}" />
<Property name="Text" value="A noob" />
<Property name="BlinkCaret" value="True" />
</Window>
<Window type="TaharezLook/Label" name="ColorSelectionWidget" >
<Property name="Area" value="{{0,0},{0.1,0},{0.35,0},{0.14,0}}" />
<Property name="Text" value="Your text color:" />
<Property name="MaxSize" value="{{1,0},{1,0}}" />
<Property name="HorzFormatting" value="RightAligned" />
</Window>
<Window type="TaharezLook/Button" name="HostButton" >
<Property name="Area" value="{{0.32,0},{0.42,0},{0.62,0},{0.48,0}}" />
<Property name="Text" value="Connect" />
<Property name="VerticalAlignment" value="Centre" />
<Property name="HorizontalAlignment" value="Centre" />
</Window>
<Window type="TaharezLook/Editbox" name="IPEditbox" >
<Property name="Area" value="{{0.38,0},{0.82,0},{0.88,0},{0.86,0}}" />
<Property name="Text" value="127.0.0.1" />
<Property name="BlinkCaret" value="True" />
</Window>
<Window type="TaharezLook/Label" name="IPLabel" >
<Property name="Area" value="{{0,0},{0.82,0},{0.35,0},{0.86,0}}" />
<Property name="Text" value="IP Address" />
<Property name="MaxSize" value="{{1,0},{1,0}}" />
<Property name="HorzFormatting" value="RightAligned" />
</Window>
<Window type="TaharezLook/Label" name="IPDescriptionText" >
<Property name="Area" value="{{0,0},{0.77,0},{0.75,0},{0.81,0}}" />
<Property name="Text" value="Select the IP you want to connect to." />
<Property name="MaxSize" value="{{1,0},{1,0}}" />
<Property name="HorizontalAlignment" value="Centre" />
</Window>
</Window>
</GUILayout>

*/