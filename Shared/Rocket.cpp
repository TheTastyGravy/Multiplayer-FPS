#include "Rocket.h"
#include "../Server/CustomServer.h"


Rocket::Rocket(PhysicsState initState, unsigned int objectID, Collider* collider, float explosionRadius, float damage, unsigned int ownerID) : 
	GameObject(initState, objectID, 1, 0, collider), explosionRadius(explosionRadius), damage(damage), ownerID(ownerID), server(nullptr), client(nullptr)
{
	typeID = 1000;
}

Rocket::~Rocket()
{}


void Rocket::serialize(RakNet::BitStream & bsInOut) const
{
	GameObject::serialize(bsInOut);
	bsInOut.Write(explosionRadius);
	bsInOut.Write(damage);
	bsInOut.Write(ownerID);
}


void Rocket::draw() const
{
	// Draw using the rockets sphere collider
	Sphere* sphere = dynamic_cast<Sphere*>(getCollider());
	if (!sphere)
	{
		return;
	}
	
	DrawSphere(position, sphere->getRadius(), RED);
}
