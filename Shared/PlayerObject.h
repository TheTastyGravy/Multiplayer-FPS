#pragma once
#include "ClientObject.h"
#include "IDrawable.h"


class PlayerObject : public ClientObject, public IDrawable
{
public:
	PlayerObject(unsigned int clientID);
	PlayerObject(PhysicsState initState, unsigned int clientID, Collider* collider = nullptr, float health = 100, raylib::Color color = RED, float friction = 1);
	virtual ~PlayerObject();


	virtual void serialize(RakNet::BitStream& bsInOut) const override;


	virtual PhysicsState processInputMovement(const Input& input) const override;
	virtual void processInputAction(const Input& input, RakNet::Time timeStamp) override;


	void update(float deltaTime);
	virtual void draw() const override;

protected:
	virtual void server_onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal) override;
	virtual void client_onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal) override;



protected:
	float health;
	raylib::Color color;

	unsigned int groundContacts;


	float moveSpeedCap = 10;
	float moveAcceleration = 1;
	float mouseAccelX = 0.002f;
	float mouseAccely = 0.002f;
};