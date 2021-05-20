#include "CustomServer.h"
#include "../Shared/PlayerObject.h"
#include "../Shared/Rocket.h"
#include "../Shared/WorldObject.h"
#include "../Shared/CustomGameMessages.h"
#include "../Shared/KillZone.h"

#include <iostream>


CustomServer::CustomServer() :
	Server(0.02f)
{}

CustomServer::~CustomServer()
{
}


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



	// Ground and walls
	for (int x = -11; x <= 11; x++)
	{
		for (int y = -11; y <= 11; y++)
		{
			staticObjects.push_back(new WorldObject({ x*10.f, (abs(x) == 11 || abs(y) == 11 ? -15.f : -20.f), y*10.f }, Vector3Zero(), (x+y) % 2 == 0 ? GREEN : DARKGREEN, new OBB({ 5, 5, 5 })));
		}
	}

	// Random spheres in the sky
	staticObjects.push_back(new WorldObject({ 5,0,5 }, Vector3Zero(), BLUE, new Sphere(3)));
	staticObjects.push_back(new WorldObject({ -5,0,5 }, Vector3Zero(), RED, new Sphere(3)));
	staticObjects.push_back(new WorldObject({ 5,0,-5 }, Vector3Zero(), YELLOW, new Sphere(3)));
	staticObjects.push_back(new WorldObject({ -5,0,-5 }, Vector3Zero(), PURPLE, new Sphere(3)));
	// Box under them
	staticObjects.push_back(new WorldObject({ 0,-10,0 }, Vector3Zero(), PINK, new OBB({ 4, 4, 4 })));
	// Kill zone below level
	staticObjects.push_back(new KillZone({ 0,-40,0 }, Vector3Zero(), new OBB({ 500, 5, 500 })));
	

	// Spawn points for players
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
		// Pass all packets through the system
		Server::processSystemMessage(packet);
	}

	// Get system time before it gets updated
	float deltaTime = (RakNet::GetTime() - Server::getTime()) * 0.001f;
	Server::systemUpdate();

	// Update client objects
	for (auto& it : clientObjects)
	{
		((PlayerObject*)it.second)->update(deltaTime);
	}
}


void CustomServer::respawnPlayer(unsigned int clientID)
{
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


	// Respawn the player, and message clients
	((PlayerObject*)clientObjects[clientID])->respawn(spawnPos);
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)ID_SERVER_RESPAWN_PLAYER);
	bs.Write(clientID);
	bs.Write(spawnPos);
	peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void CustomServer::createExplosion(const raylib::Vector3& center, float radius, float damage, unsigned int ownerID)
{
	// Send message to create an explosion effect. Unreliable because its not essential
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)ID_SERVER_CREATE_EXPLOSION);
	bs.Write(center);
	bs.Write(radius);
	peerInterface->Send(&bs, MEDIUM_PRIORITY, UNRELIABLE, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

	for (auto& obj : clientObjects)
	{
		raylib::Vector3 dir = obj.second->getPosition() - center;
		float dist = dir.Length();

		// Players should all have spheres, so subtract the radius to get dist to closest point
		Sphere* col = dynamic_cast<Sphere*>(obj.second->getCollider());
		if (col)
		{
			dist -= col->getRadius();
		}


		if (dist < radius)
		{
			// Apply force to the object away from the explosion
			obj.second->applyForce(dir.Normalize() * 20, Vector3Zero());

			// Dont damage the player that fired the rocket
			if (obj.second->getID() == ownerID)
			{
				continue;
			}


			// Damage the object, and message clients
			((PlayerObject*)obj.second)->dealDamage(damage);
			RakNet::BitStream bs;
			bs.Write((RakNet::MessageID)ID_SERVER_UPDATE_HEALTH);
			bs.Write(obj.first);
			bs.Write(damage);
			peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		}
	}
}


GameObject* CustomServer::gameObjectFactory(unsigned int typeID, unsigned int objectID, const PhysicsState& state, RakNet::BitStream& bsIn)
{
	switch (typeID)
	{
	case 1000:
	{
		// Create rocket
		float radius, damage;
		unsigned int ownerID;
		bsIn.Read(radius);
		bsIn.Read(damage);
		bsIn.Read(ownerID);

		Rocket* obj = new Rocket(state, objectID, new Sphere(.5f), radius, damage, ownerID);
		obj->setServer(this);
		return obj;
	}


		// Invalid ID
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
