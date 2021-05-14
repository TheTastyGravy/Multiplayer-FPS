#pragma once
#include "Client.h"


class CustomClient : public Client
{
public:
	CustomClient();
	virtual ~CustomClient();


	void startup(const char* ip, unsigned short port);

	void update();
	void draw3D(raylib::Camera3D& cam);
	void draw2D();


	// Called by rocket collision callback for prediction
	void createExplosion(raylib::Vector3 center, float radius);

protected:
	// Responces for server messages
	void updateHealth(RakNet::BitStream& bsIn);
	void respawnPlayer(RakNet::BitStream& bsIn);
	void createExplosionEffect(RakNet::BitStream& bsIn);


	virtual Input getInput() override;

	virtual StaticObject* staticObjectFactory(unsigned int typeID, ObjectInfo& objectInfo, RakNet::BitStream& bsIn) override;
	virtual GameObject* gameObjectFactory(unsigned int typeID, unsigned int objectID, ObjectInfo& objectInfo, RakNet::BitStream& bsIn) override;
	virtual ClientObject* clientObjectFactory(unsigned int typeID, ObjectInfo& objectInfo, RakNet::BitStream& bsIn) override;



protected:
	struct Particle
	{
		raylib::Vector3 position;
		raylib::Vector3 velocity;
		raylib::Color color;

		float lifeRemaining;
	};
	std::vector<Particle> particles;

	raylib::Texture2D tex;
};