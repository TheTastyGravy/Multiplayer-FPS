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

void PlayerObject::onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal)
{
	// If colliding with a kill zone, respawn
	if (other->getTypeID() == 2 && server != nullptr)
	{
		((CustomServer*)server)->respawnPlayer(objectID);
		return;
	}


	// If the collision normal is close to down, we are on ground
	if (normal.y < -0.85f)
	{
		groundTimmer = 0.1f;
	}
}