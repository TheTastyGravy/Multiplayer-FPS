#pragma once
#include "ClientObject.h"
#include "IDrawable.h"
#include "Server.h"


class PlayerObject : public ClientObject, public IDrawable
{
public:
	PlayerObject(unsigned int clientID);
	PlayerObject(PhysicsState initState, unsigned int clientID, Collider* collider = nullptr, float health = 100, raylib::Color color = RED, float friction = 1);
	virtual ~PlayerObject();


	virtual void serialize(RakNet::BitStream& bsInOut) const override;


	virtual PhysicsState processInputMovement(const Input& input) const override;
	virtual void processInputAction(const Input& input, RakNet::Time timeStamp) override;	// Defined seperatly for client and server


	void update(float deltaTime);
	virtual void draw() const override;

	void dealDamage(float damage);
	void respawn(raylib::Vector3 position);

	
	void setServer(Server* server) { this->server = server; }

	float getHealth() const { return health; }
	float isDead() const { return health <= 0; }

protected:
	virtual void onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal) override;	// Defined seperatly for client and server



protected:
	float health;
	raylib::Color color;

	// A timmer is used to determine when we are grounded. On contact with the ground, it is reset.
	float groundTimmer = 0;


	float moveSpeedCap = 25;
	float moveAcceleration = 2;
	float mouseAccelX = 0.002f;
	float mouseAccelY = 0.002f;

	Server* server;
};