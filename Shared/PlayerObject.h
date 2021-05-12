#pragma once
#include "ClientObject.h"


class PlayerObject : public ClientObject
{
public:
	PlayerObject(unsigned int clientID);
	PlayerObject(PhysicsState initState, unsigned int clientID, Collider* collider = nullptr, float health = 100);
	virtual ~PlayerObject();


	virtual void serialize(RakNet::BitStream& bsInOut) const override;


	virtual PhysicsState processInputMovement(const Input& input) const override;
	virtual void processInputAction(const Input& input, RakNet::Time timeStamp) override;


	void update(float deltaTime);
	void draw();

protected:
	virtual void server_onCollision(StaticObject* other) override;
	virtual void client_onCollision(StaticObject* other) override;



protected:
	float health;

	unsigned int groundContacts;


	float moveSpeedCap = 10;
	float moveAcceleration = 1;
	float mouseAccelX = 0.002f;
	float mouseAccely = 0.002f;
};