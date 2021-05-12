#pragma once
#include <MessageIdentifiers.h>

enum GameMessages
{
	ID_SERVER_CREATE_STATIC_OBJECTS = ID_USER_PACKET_ENUM + 1,	// Used when client connects to send static objects
	ID_SERVER_CREATE_CLIENT_OBJECT,		// Used when client connects to create their client object, containing their client ID

	ID_SERVER_CREATE_GAME_OBJECT,	// Used to instantiate a new game object
	ID_SERVER_DESTROY_GAME_OBJECT,	// Used to destroy a game object
	ID_SERVER_UPDATE_GAME_OBJECT,	// Used to update the rigidbody values of a game object

	ID_CLIENT_INPUT,		// Used to send player input to the server


	ID_USER_CUSTOM_ID		// Start your custom packet IDs here
};