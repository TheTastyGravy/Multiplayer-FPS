#include "CustomServer.h"
#include "../Shared/PlayerObject.h"
#include "../Shared/Rocket.h"

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
	staticObjects.push_back(new StaticObject({ 0,-20,0 }, Vector3Zero(), new OBB({ 100, 2, 100 })));
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

	return new PlayerObject(PhysicsState({ 0,10,0 }, { 0,0,0 }), clientID, new Sphere(5), 0.3f);
}
