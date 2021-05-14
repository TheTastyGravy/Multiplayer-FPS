#include "../Shared/Rocket.h"
#include "CustomServer.h"


void Rocket::server_onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal)
{
	if (!server)
	{
		return;
	}

	// Check if we are colliding with our owner, and if so, do nothing
	GameObject* obj = dynamic_cast<GameObject*>(other);
	if (obj && obj->getID() == ownerID)
	{
		return;
	}


	// Tell the server to create an explosion
	((CustomServer*)server)->createExplosion(contact, explosionRadius, damage, ownerID);

	// Destroy this object
	server->destroyObject(objectID);
}

void Rocket::client_onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal)
{}