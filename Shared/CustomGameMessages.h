#pragma once
#include "GameMessages.h"

enum CustomGameMessages
{
	ID_SERVER_UPDATE_HEALTH = ID_USER_CUSTOM_ID + 1,	// Used to change the health of the client object owned by a client
	ID_SERVER_RESPAWN_PLAYER,		// Used to respawn a dead player
	ID_SERVER_CREATE_EXPLOSION		// Used to create an explosion effect
};
