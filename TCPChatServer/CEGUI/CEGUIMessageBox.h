#pragma once
#include "CEGUI/CEGUI.h"

//Self-destructing and self-contained messagebox using the CEGUI system.
class CEGUIMessageBox
{
public:
	CEGUIMessageBox(CEGUI::String message);
	~CEGUIMessageBox();

	bool Close(const CEGUI::EventArgs &e);

private:
	CEGUI::Window* backgroundWindow;
};

