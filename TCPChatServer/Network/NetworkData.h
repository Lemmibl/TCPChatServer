#pragma once
static const int MAX_PACKET_SIZE = 1024;
typedef unsigned int UserID;
typedef unsigned int Color;

//8 bits. TODO: Get it down to 2 bits?
enum UserPermission : unsigned char
{
	Restricted = 0,
	Normal = 1,
	Admin = 2,
	SuperAdmin = 3 //AKA Host.
};

//So this "Workaround" to strongly typed enums in vs2010 might be seen as an abomination, sure. I just think it makes it really clear what I'm doing.
namespace PacketType
{
	//So that if we want to define an object of this enum type, we can declare it to be a DataPacketType::Type.
	typedef int Type;

	//Anomnymous enum to allow DataPacketType::NODATA for example.
	enum
	{
		NODATA = 0,
		STRING,
		COLORED_STRING,
		FLOAT,
		USER_DATA,
		CONNECT,
		DISCONNECT
	};
}