#pragma once
#include "Client.h"


class CustomClient : public Client
{
public:
	CustomClient();
	virtual ~CustomClient();


	void startup(const char* ip, unsigned short port);

	void update();
	void draw(raylib::Camera3D& cam);


protected:
	virtual Input getInput() override;

	virtual StaticObject* staticObjectFactory(unsigned int typeID, ObjectInfo& objectInfo, RakNet::BitStream& bsIn) override;
	virtual GameObject* gameObjectFactory(unsigned int typeID, unsigned int objectID, ObjectInfo& objectInfo, RakNet::BitStream& bsIn) override;
	virtual ClientObject* clientObjectFactory(unsigned int typeID, ObjectInfo& objectInfo, RakNet::BitStream& bsIn) override;



	raylib::Vector2 lastMousePos;
};