#include "../Shared/PlayerObject.h"
#include "CustomServer.h"


void PlayerObject::processInputAction(const Input& input, RakNet::Time timeStamp)
{
	// Only perform actions when the server can be acessed
	if (!server)
	{
		return;
	}

	// A dead player can only respawn
	if (isDead())
	{
		if (input.fire)
		{
			((CustomServer*)server)->respawnPlayer(objectID);
		}

		return;
	}


	if (input.fire)
	{
		// Find the facing direction
		float x, y;
		x = getRotation().y - PI * 0.5f;
		y = -getRotation().z;
		raylib::Vector3 forward(cos(y) * cos(x), sin(y), cos(y) * sin(x));

		PhysicsState state;
		state.position = position + forward * 3.5f;	// Create the object infront of the player
		state.velocity = forward * 60;


		RakNet::BitStream bs;
		bs.Write(10.f);		//radius
		bs.Write(20.f);		//damage
		bs.Write(objectID);	//owner

		// Create a rocket on the server
		server->createObject(1000, state, timeStamp, &bs);
	}
}