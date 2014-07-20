#pragma once
#include <unordered_map>
#include <string>

#include "../../Network/ChatUserData.h"
#include "../../Network/SocketWrapper.h"
#include "CEGUI/CEGUI.h"

class UserManager
{
public:
	UserManager();
	~UserManager();

	ChatUserData& GetHostData() { return hostData; }

	void Update();

	//Insert user into the internal unordered map if we don't find a duplicate, in which case we return that ID instead.
	//If we don't find a duplicate, we create a new UserData and fill it with the parameters that the function received. 
	// Finally we return the handle that is used internally to access the userData from the map.
	UserID AddUser(const SocketWrapper userSocket, const CEGUI::String userName, const unsigned int userColor, const unsigned int ip);
	
	//Should be self explanatory. If there is no user by that name, it fails silently.
	void RemoveUser(const UserID id);

	//Returns result of a map.Find();
	bool DoesUserExist(const UserID id);
	
	//If the UserID is valid, it fills up the outUserInfo struct and returns a true. If it returns false, it means we didn't find any user with that UserID.
	bool GetUser(const UserID id, ChatUserData* outUserData);

	//Returns a pair of iterators. First slot is begin() and second slot is end()
	std::pair<std::unordered_map<UserID, ChatUserData>::iterator, std::unordered_map<UserID, ChatUserData>::iterator> GetAllUsers();

	std::list<CEGUI::String>& GetDisconnectedUserNames();
	std::list<UserID>& GetDisconnectedUserIDs();

private:
	ChatUserData hostData;
	std::unordered_map<UserID, ChatUserData> userDatabase;

	//Okay, so when RemoveUser is called, I don't actually remove the user immediately. That would screw up all iterators and usage for all other classes.
	//Instead I gather them all up and do a cleanup during update, at the end of the main loop.
	std::list<UserID> cleanUpList;

	//This list is added to every time a player disconnects. 
	// We add the player's name and the MessageHandler uses this data to send a message to every other client telling them who has disconnected.
	std::list<CEGUI::String> disconnectedUsers;

};

