#pragma once
#include "GameObject.h"
#include "IDrawable.h"
#include "Server.h"
#include "Client.h"


class Rocket : public GameObject, public IDrawable
{
public:
	Rocket(PhysicsState initState, unsigned int objectID, Collider* collider = nullptr, float explosionRadius = 10, float damage = 1, unsigned int ownerID = 0);
	virtual ~Rocket();


	virtual void serialize(RakNet::BitStream& bsInOut) const override;


	virtual void draw() const override;


	void setServer(Server* server) { this->server = server; }
	void setClient(Client* client) { this->client = client; }

protected:
	virtual void onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal) override;	// Defined seperatly for client and server



protected:
	float explosionRadius;
	float damage;
	// The ID of the client object that fired this rocket
	unsigned int ownerID;

	Server* server;
	Client* client;
};