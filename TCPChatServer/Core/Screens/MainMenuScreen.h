#pragma once
#include "ScreenBase.h"
#include "CEGUI/CEGUI.h"
#include "../../CEGUI/CEGUIMessageBox.h"

class MainMenuScreen : public ScreenBase
{
public:
	MainMenuScreen();
	~MainMenuScreen();

	bool Enter();
	bool Initialize();
	bool Update(double deltaTime);
	void Render();
	void Exit();

private:
	CEGUI::Window* rootWindow;
};