#pragma once
#include "ScreenBase.h"

//TODO...
class OptionsScreen : public ScreenBase
{
public:
	OptionsScreen();
	~OptionsScreen();

	virtual bool Enter();
	virtual bool Initialize();
	virtual bool Update(double deltaTime);
	virtual void Render();
	virtual void Exit();
};

