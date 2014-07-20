#include "CEGUIMessageBox.h"

CEGUIMessageBox::CEGUIMessageBox(CEGUI::String message)
{
	bool isRoot = false;


	//Get external root window
	CEGUI::Window* rootWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

	if(rootWindow == nullptr)
	{
		isRoot = true;
	}

	//Set up the transparent background/overlay window
	backgroundWindow = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/FrameWindow", "MessageBoxBackground");
	backgroundWindow->setSize(CEGUI::USize(CEGUI::UDim(1.1f, 0.0f), CEGUI::UDim(1.1f, 0.0f)));
	backgroundWindow->setAlpha(0.5f);
	backgroundWindow->setProperty("ClientAreaColour", "tl:FFAAAAAA tr:FFAAAAAA bl:FFAAAAAA br:FFAAAAAA");
	backgroundWindow->setProperty("FrameEnabled", "False");
	backgroundWindow->setProperty("TitlebarEnabled", "False");
	backgroundWindow->setProperty("CloseButtonEnabled", "False");
	backgroundWindow->setProperty("DragMovingEnabled", "False");

	//Make sure it's active and shit
	backgroundWindow->activate();
	backgroundWindow->show();
	backgroundWindow->setAlwaysOnTop(true);

	//Set up the actual box window
	CEGUI::Window* boxWindow = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/FrameWindow", "MessageBoxWindow");
	boxWindow->setHorizontalAlignment(CEGUI::HA_CENTRE);
	boxWindow->setVerticalAlignment(CEGUI::VA_CENTRE);
	boxWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0.0f), CEGUI::UDim(0.0f, 0.0f)));
	boxWindow->setSize(CEGUI::USize(CEGUI::UDim(0.0f, 400.0f), CEGUI::UDim(0.0f, 300.0f)));
	boxWindow->setProperty("CloseButtonEnabled", "False");
	boxWindow->setProperty("DragMovingEnabled", "False");
	boxWindow->setProperty("RollUpEnabled", "False");
	boxWindow->setInheritsAlpha(false);
	boxWindow->setText("Error");

	//Set up the text we'll be rendering "in" the box
	CEGUI::Window* errorText = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "MessageBoxText");
	errorText->setHorizontalAlignment(CEGUI::HA_CENTRE);
	errorText->setVerticalAlignment(CEGUI::VA_TOP);
	errorText->setSize(CEGUI::USize(CEGUI::UDim(0.8f, 0.0f), CEGUI::UDim(0.7f, 0.0f)));
	errorText->setProperty("HorzFormatting", "WordWrapLeftAligned");
	errorText->setText(message);

	//Create a confirmation button and put it horizontally centered and furthest down vertically.
	CEGUI::Window* confirmButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "MessageConfirmButton");
	confirmButton->setHorizontalAlignment(CEGUI::HA_CENTRE);
	confirmButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0.0f), CEGUI::UDim(0.75f, 0.0f)));
	confirmButton->setSize(CEGUI::USize(CEGUI::UDim(0.0f, 80.0f), CEGUI::UDim(0.0f, 40.0f)));
	confirmButton->setText("Right.");
	confirmButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CEGUIMessageBox::Close, this));

	//Append children to the box
	boxWindow->addChild(errorText);
	boxWindow->addChild(confirmButton);

	//Append box to the background
	backgroundWindow->addChild(boxWindow);

	//Append background to real root

	if(!isRoot)
	{
		rootWindow->addChild(backgroundWindow);
	}
	else
	{
		CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(backgroundWindow);
		CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->activate();
	}
}

CEGUIMessageBox::~CEGUIMessageBox()
{
	backgroundWindow = nullptr;
}

//This is called when we click the "Ok" button. Hence this class self destructs.
bool CEGUIMessageBox::Close( const CEGUI::EventArgs &e )
{
	if(backgroundWindow != nullptr)
	{
		//If we aren't root
		if(backgroundWindow->getParent() != 0)
		{
			//Remove ourselves from parent...
			backgroundWindow->getParent()->removeChild(backgroundWindow);
		}

		//Then destroy.
		backgroundWindow->destroy();	
	}

	//Commit suicide.
	delete this;

	return true;
}
