#include "UserManager.h"

UserManager::UserManager()
{
	//We don't actually use the host socket for anything interesting.
	hostData.clientSocket = 0;

	//Fill with various default values.
	hostData.id = 0;
	hostData.permissions = SuperAdmin;
	hostData.textColor = 0xFF00FFFF;
	hostData.userName = "Admin";
}

UserManager::~UserManager()
{
}

void UserManager::Update()
{
	if(cleanUpList.size() > 0)
	{
		//Cleanup of clients that have disconnected.
		for(auto iter = cleanUpList.begin(); iter != cleanUpList.end(); ++iter)
		{
			userDatabase.erase(*iter);
		}

		//Then clear it.
		cleanUpList.clear();
	}
}

UserID UserManager::AddUser(const SocketWrapper& userSocket, const CEGUI::String userName, const unsigned int userColor, const unsigned int ip)
{
	//Every time we get a new user, we use this value as that user's ID. Then increment by one. Should be fine as long as we don't get more than 4294967295 users.
	static UserID UserIDCounter = 0;

	//Increment it before assigning. I want the first user to start at 1, not 0.
	++UserIDCounter;

	std::pair<UserID, ChatUserData> user;

	//Assign an ID to this user. Implicit UserID <-> uint works just fine, but I want to be clear here.
	user.first = UserIDCounter;

	//Populate this shit.
	user.second.id = UserIDCounter; //Even if we use the userID as a key, it's still useful to contain the ID within the actual user data. For example, when we read packets from sockets in TCPServer.
	user.second.clientSocket = userSocket;
	user.second.userName = userName;
	user.second.textColor = userColor;

	//Assume that per default, the permissions will be that of a Normal user
	user.second.permissions = Normal;

	userDatabase.insert(std::move(user));

	return UserIDCounter;
}

void UserManager::RemoveUser(const UserID id)
{
	auto user = userDatabase.find(id);

	if(user != userDatabase.end())
	{	
		//Save player's name
		disconnectedUsers.push_back(user->second.userName);

		//Remove the user from our user "database"
		cleanUpList.push_back(id);
	}
}

bool UserManager::DoesUserExist(const UserID id )
{
	auto it = userDatabase.find(id);
	if(it != userDatabase.end())
	{
		return true;
	}

	return false;
}

bool UserManager::GetUser(const UserID id, ChatUserData* outUserData )
{
	//Look for a user with this ID.
	auto& it = userDatabase.find(id);

	//If find() actually found something
	if(it != userDatabase.end())
	{
		//Set outUserData to point to this object.
		outUserData = &it->second;

		//Return that we've succeeded.
		return true;
	}

	//If we've reached this point, it means that we didn't find any user with that ID.
	return false;
}

std::pair<std::unordered_map<UserID, ChatUserData>::iterator, std::unordered_map<UserID, ChatUserData>::iterator> UserManager::GetAllUsers()
{
	return std::make_pair(userDatabase.begin(), userDatabase.end());
}

std::list<CEGUI::String>& UserManager::GetDisconnectedUserNames()
{
	return disconnectedUsers;
}

std::list<UserID>& UserManager::GetDisconnectedUserIDs()
{
	return cleanUpList;
}
