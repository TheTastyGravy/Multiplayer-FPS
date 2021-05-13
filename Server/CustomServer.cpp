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



GameObject* CustomServer::gameObjectFactory(unsigned int typeID, unsigned int objectID, const PhysicsState& state, RakNet::BitStream& bsIn)
{
	switch (typeID)
	{
	case 1000:
	{
		float radius, damage;
		bsIn.Read(radius);
		bsIn.Read(damage);

		Sphere* col = new Sphere(.5f);
		return new Rocket(state, objectID, col, radius, damage);
	}




		//invalid ID
	default:
		return nullptr;
	}
}

ClientObject* CustomServer::clientObjectFactory(unsigned int clientID)
{
	//this should eventualy use a random point that is not close to any other players

	//give each client a diferent color
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

	return new PlayerObject(PhysicsState({ 0,10,0 }, { 0,0,0 }), clientID, new Sphere(5), 100, color, 0.3f);
}
