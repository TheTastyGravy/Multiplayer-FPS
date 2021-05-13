#include "CustomServer.h"
#include "../Shared/PlayerObject.h"
#include "../Shared/Rocket.h"
#include "../Shared/WorldObject.h"

#include <iostream>



void CustomServer::startup(const char* ip, unsigned short port)
{
	// Startup the server and start it listening to clients
	std::cout << "Starting up the server..." << std::endl;

	// Create a socket descriptor for this connection
	RakNet::SocketDescriptor sd(port, ip);

	// Startup the interface with a max of 32 connections
	peerInterface->Startup(32, &sd, 1);
	peerInterface->SetMaximumIncomingConnections(32);
	// Automatic pinging for timestamping
	peerInterface->SetOccasionalPing(true);

	// Output state of server
	if (peerInterface->IsActive())
	{
		std::cout << "Server setup sucessful" << std::endl;
		std::cout << "Server address: " << peerInterface->GetMyBoundAddress().ToString() << std::endl;
	}
	else
	{
		std::cout << "Server failed to setup" << std::endl;
	}



	//create static objects

	//floor
	staticObjects.push_back(new WorldObject({ 0,-20,0 }, Vector3Zero(), GREEN, new OBB({ 100, 2, 100 })));

	staticObjects.push_back(new WorldObject({ 5,0,5 }, Vector3Zero(), BLUE, new Sphere(3)));
	staticObjects.push_back(new WorldObject({ -5,0,5 }, Vector3Zero(), RED, new Sphere(3)));
	staticObjects.push_back(new WorldObject({ 5,0,-5 }, Vector3Zero(), YELLOW, new Sphere(3)));
	staticObjects.push_back(new WorldObject({ -5,0,-5 }, Vector3Zero(), PURPLE, new Sphere(3)));



	spawnPositions.push_back({ 30, -10, 30 });
	spawnPositions.push_back({ -30, -10, 30 });
	spawnPositions.push_back({ 30, -10, -30 });
	spawnPositions.push_back({ -30, -10, -30 });
}

void CustomServer::update()
{
	for (RakNet::Packet* packet = peerInterface->Receive(); packet; peerInterface->DeallocatePacket(packet),
		packet = peerInterface->Receive())
	{
		Server::processSystemMessage(packet);


		//custom messages
	}


	float deltaTime = (RakNet::GetTime() - Server::getTime()) * 0.001f;

	//update client objects. this needs to be done before system update because this resets contact points
	//because client objects are only updated when input is receved, if a client stalls for a couple seconds the velocity due to gravity will build up
	for (auto& it : clientObjects)
	{
		((PlayerObject*)it.second)->update(deltaTime);
	}


	Server::systemUpdate();


	//custom logic

	

}


void CustomServer::createExplosion(const raylib::Vector3& center, float radius, float damage)
{
	for (auto& it : clientObjects)
	{
		raylib::Vector3 dir = it.second->getPosition() - center;
		float dist = dir.Length();

		// Players should all have spheres, so subtract the radius to get dist to closest point
		Sphere* col = dynamic_cast<Sphere*>(it.second->getCollider());
		if (col)
		{
			dist -= col->getRadius();
		}


		if (dist < radius)
		{
			it.second->applyForce(dir.Normalize() * 10, Vector3Zero());

			//damage
		}
	}
}



GameObject* CustomServer::gameObjectFactory(unsigned int typeID, unsigned int objectID, const PhysicsState& state, RakNet::BitStream& bsIn)
{
	switch (typeID)
	{
	case 1000:
	{
		float radius, damage;
		bsIn.Read(radius);
		bsIn.Read(damage);

		Rocket* obj = new Rocket(state, objectID, new Sphere(.5f), radius, damage);
		obj->setServer(this);
		return obj;
	}




		//invalid ID
	default:
		return nullptr;
	}
}

ClientObject* CustomServer::clientObjectFactory(unsigned int clientID)
{
	// Get a color for the client
	raylib::Color color;
	switch (clientID % 4)
	{
	case 1:
		color = RED;
		break;
	case 2:
		color = BLUE;
		break;
	case 3:
		color = PINK;
		break;
	case 0:
		color = YELLOW;
		break;
	}

	// Get a spawn position
	raylib::Vector3 spawnPos;
	for (auto& pos : spawnPositions)
	{
		// Check if any other clients are close to the spawn point
		bool isFree = true;
		for (auto& obj : clientObjects)
		{
			if (pos.Distance(obj.second->getPosition()) < 15)
			{
				isFree = false;
				break;
			}
		}

		// No clients are close, so use it
		if (isFree)
		{
			spawnPos = pos;
			break;
		}
	}


	PlayerObject* obj = new PlayerObject(PhysicsState(spawnPos, { 0,0,0 }), clientID, new Sphere(5), 100, color, .7f);
	obj->setServer(this);

	return obj;
}
