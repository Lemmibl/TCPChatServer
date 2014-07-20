#pragma once
#include "ScreenBase.h"
#include "CEGUI/CEGUI.h"

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