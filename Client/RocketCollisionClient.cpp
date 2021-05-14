#include "../Shared/Rocket.h"
#include "CustomClient.h"


void Rocket::server_onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal)
{}

void Rocket::client_onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal)
{
	if (!client)
	{
		return;
	}

	// Check if we are colliding with our owner, and if so, do nothing
	GameObject* obj = dynamic_cast<GameObject*>(other);
	if (obj && obj->getID() == ownerID)
	{
		return;
	}


	// Client side explosion will try to apply force to player for prediction
	((CustomClient*)client)->createExplosion(contact, explosionRadius);
}