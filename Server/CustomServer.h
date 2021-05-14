#pragma once
#include "Server.h"


class CustomServer : public Server
{
public:
	CustomServer();
	virtual ~CustomServer();


	void startup(const char* ip, unsigned short port);

	void update();


	// Called by player object input action
	void respawnPlayer(unsigned int clientID);
	// Called by rocket collision callback
	void createExplosion(const raylib::Vector3& center, float radius, float damage, unsigned int ownerID);

protected:
	virtual GameObject* gameObjectFactory(unsigned int typeID, unsigned int objectID, const PhysicsState& state, RakNet::BitStream& bsIn) override;
	virtual ClientObject* clientObjectFactory(unsigned int clientID) override;



protected:
	// Posible positions that a player can be spawned
	std::vector<raylib::Vector3> spawnPositions;
};