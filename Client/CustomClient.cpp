#include "CustomClient.h"
#include <GetTime.h>
#include "../Shared/PlayerObject.h"
#include "../Shared/Rocket.h"
#include "../Shared/WorldObject.h"

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
	DisableCursor();
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

	float deltaTime = (RakNet::GetTime() - Client::getTime()) * 0.001f;

	Client::systemUpdate();


	//client side logic

	if (myClientObject)
		((PlayerObject*)myClientObject)->update(deltaTime);
}

void CustomClient::draw(raylib::Camera3D& cam)
{
	// Draw static and game objects through the IDrawable interface
	for (auto& it : staticObjects)
	{
		IDrawable* drawObj = dynamic_cast<IDrawable*>(it);
		if (drawObj)
		{
			drawObj->draw();
		}
	}
	for (auto& it : gameObjects)
	{
		IDrawable* drawObj = dynamic_cast<IDrawable*>(it.second);
		if (drawObj)
		{
			drawObj->draw();
		}
	}


	// If we have a client object, update the camera for first person
	if (myClientObject)
	{
		cam.SetPosition(myClientObject->getPosition());

		float x, y;
		x = myClientObject->getRotation().y - PI * 0.5f;
		y = -myClientObject->getRotation().z;
		raylib::Vector3 forward(cos(y) * cos(x), sin(y), cos(y) * sin(x));
		cam.SetTarget(myClientObject->getPosition() + forward);
	}
}



Input CustomClient::getInput()
{
	//fill with basic input values
	Input input;

	input.fire = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
	input.jump = IsKeyPressed(KEY_SPACE);

	input.mousePos = GetMousePosition();
	input.mouseDelta = (-lastMousePos) + input.mousePos;
	lastMousePos = input.mousePos;

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
	case 1:
	{
		raylib::Color color;
		bsIn.Read(color);
		return new WorldObject(objectInfo.state.position, objectInfo.state.rotation, color, objectInfo.collider);
	}



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
		raylib::Color color;
		bsIn.Read(health);
		bsIn.Read(color);
		return new PlayerObject(objectInfo.state, objectID, objectInfo.collider, health, color, objectInfo.friction);
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
	raylib::Color color;
	bsIn.Read(health);
	bsIn.Read(color);
	return new PlayerObject(objectInfo.state, getClientID(), objectInfo.collider, health, color, objectInfo.friction);
}
