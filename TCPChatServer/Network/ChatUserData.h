#pragma once
#include "NetworkData.h"
#include "CEGUI/CEGUI.h"
#include "../../Network/SocketWrapper.h"

struct ChatUserData
{
	SocketWrapper clientSocket;
	UserPermission permissions;
	UserID id;
	CEGUI::Colour textColor;
	CEGUI::String userName;
};