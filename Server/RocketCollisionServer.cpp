#include "../Shared/Rocket.h"
#include "CustomServer.h"


void Rocket::server_onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal)
{
	if (!server)
	{
		return;
	}


	// Tell the server to create an explosion
	((CustomServer*)server)->createExplosion(contact, explosionRadius, damage);

	// Destroy this object
	server->destroyObject(objectID);
}