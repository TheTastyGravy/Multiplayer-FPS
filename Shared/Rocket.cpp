#include "Rocket.h"


Rocket::Rocket(PhysicsState initState, unsigned int objectID, Collider* collider, float explosionRadius, float damage) : 
	GameObject(initState, objectID, 1, 0, collider), explosionRadius(explosionRadius), damage(damage)
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
}



void Rocket::draw() const
{
	DrawSphere(position, 1, RED);
}



void Rocket::server_onCollision(StaticObject * other, raylib::Vector3 contact, raylib::Vector3 normal)
{
	//find client objects within radius
	//	do damage with falloff
	//	apply force away from current pos, with falloff

	//destroy this instance
}

void Rocket::client_onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal)
{
	//create effect or something

	//maybe predict force on player from owned rockets (if rockets get owners)
}
