#pragma once
#include "GameObject.h"


class Rocket : public GameObject
{
public:
	Rocket(PhysicsState initState, unsigned int objectID, Collider* collider = nullptr, float explosionRadius = 10, float damage = 1);
	virtual ~Rocket();


	virtual void serialize(RakNet::BitStream& bsInOut) const override;


protected:
	virtual void server_onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal) override;
	virtual void client_onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal) override;



protected:
	float explosionRadius;
	float damage;
};