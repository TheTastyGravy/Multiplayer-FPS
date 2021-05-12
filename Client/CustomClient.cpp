#include "CustomClient.h"
#include "../Shared/PlayerObject.h"
#include "../Shared/Rocket.h"

#include <iostream>


CustomClient::CustomClient() : 
	Client()
{
}

CustomClient::~CustomClient()
{
}



void CustomClient::startup(const char* ip, unsigned short port)
{
	// No data is needed since we are connecting
	RakNet::SocketDescriptor sd;
	// 1 max connection since connecting
	peerInterface->Startup(1, &sd, 1);
	// Automatic pinging for timestamping
	peerInterface->SetOccasionalPing(true);

	std::cout << "Connecting to server at: " << ip << std::endl;

	// Attempt to connect to the server
	RakNet::ConnectionAttemptResult res = peerInterface->Connect(ip, port, nullptr, 0);
	if (res != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		std::cout << "Unable to start connection, error number: " << res << std::endl;
	}


	//lock the cursor
	//DisableCursor();
	lastMousePos = GetMousePosition();
}

void CustomClient::update()
{
	for (RakNet::Packet* packet = peerInterface->Receive(); packet; peerInterface->DeallocatePacket(packet),
		packet = peerInterface->Receive())
	{
		Client::processSystemMessage(packet);


		//custom messages
	}


	Client::systemUpdate();

	//client side logic

	if (myClientObject)
		((PlayerObject*)myClientObject)->update(1/60.f);
}

void CustomClient::draw()
{
	for (auto& it : staticObjects)
	{
		//draw static object using its collider, or make a custom class to do it
	}

	for (auto& it : gameObjects)
	{
		//game objects include other players, so try to cast to multiple types
	}


	// If we have a client object, draw it
	if (myClientObject)
	{
		//the camera should be moved to the players position and rotated to the direction their facing
		((PlayerObject*)myClientObject)->draw();
	}
}



Input CustomClient::getInput()
{
	//fill with basic input values
	Input input;

	input.fire = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
	input.jump = IsKeyPressed(KEY_SPACE);

	input.mouseDelta = (-lastMousePos) + GetMousePosition();
	lastMousePos = GetMousePosition();

	input.movement.y -= IsKeyDown(KEY_W);
	input.movement.y += IsKeyDown(KEY_S);
	input.movement.x += IsKeyDown(KEY_D);
	input.movement.x -= IsKeyDown(KEY_A);

	return input;
}


StaticObject* CustomClient::staticObjectFactory(unsigned int typeID, ObjectInfo& objectInfo, RakNet::BitStream& bsIn)
{
	switch (typeID)
	{
	case 0:
		// Use default static object
		return new StaticObject(objectInfo.state.position, objectInfo.state.rotation, objectInfo.collider);




		//invalid ID
	default:
		return nullptr;
	}
}

GameObject* CustomClient::gameObjectFactory(unsigned int typeID, unsigned int objectID, ObjectInfo& objectInfo, RakNet::BitStream& bsIn)
{
	switch (typeID)
	{
	case 1000:
	{
		// Create a rocket
		float radius, damage;
		bsIn.Read(radius);
		bsIn.Read(damage);
		return new Rocket(objectInfo.state, objectID, objectInfo.collider, radius, damage);
	}

	case 2000:
	{
		float health;
		bsIn.Read(health);
		return new PlayerObject(objectInfo.state, objectID, objectInfo.collider, health);
	}


		//invalid ID
	default:
		return nullptr;
	}
}

ClientObject* CustomClient::clientObjectFactory(unsigned int typeID, ObjectInfo& objectInfo, RakNet::BitStream& bsIn)
{
	// We only have one client object type, so just use it
	float health;
	bsIn.Read(health);
	return new PlayerObject(objectInfo.state, getClientID(), objectInfo.collider, health);
}
