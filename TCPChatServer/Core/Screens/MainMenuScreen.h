#pragma once
#include "ScreenBase.h"
#include "CEGUI/CEGUI.h"
#include "../../CEGUI/CEGUIMessageBox.h"

class MainMenuScreen : public ScreenBase
{
public:
	MainMenuScreen();
	~MainMenuScreen();

	virtual bool Enter();
	virtual bool Initialize();
	virtual bool Update(double deltaTime);
	virtual void Render();
	virtual void Exit();

private:
	CEGUI::Window* rootWindow;
};