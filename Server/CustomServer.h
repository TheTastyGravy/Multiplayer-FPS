#pragma once
#include "Server.h"


class CustomServer : public Server
{
public:


	void startup(const char* ip, unsigned short port);

	void update();


	void createExplosion(const raylib::Vector3& center, float radius, float damage);


protected:
	virtual GameObject* gameObjectFactory(unsigned int typeID, unsigned int objectID, const PhysicsState& state, RakNet::BitStream& bsIn) override;
	virtual ClientObject* clientObjectFactory(unsigned int clientID) override;



	// Posible positions that a player can be spawned
	std::vector<raylib::Vector3> spawnPositions;

};