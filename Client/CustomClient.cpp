#include "CustomClient.h"
#include <GetTime.h>
#include <sstream>	// Used to format strings for drawing text
#include "../Shared/PlayerObject.h"
#include "../Shared/Rocket.h"
#include "../Shared/WorldObject.h"
#include "../Shared/CustomGameMessages.h"
#include "rlgl.h"

#include <iostream>


CustomClient::CustomClient() : 
	Client()
{
	raylib::Image image1 = GenImagePerlinNoise(100, 100, 0, 0, 10);
	ImageAlphaMask(&image1, GenImageGradientRadial(100, 100, 0.2f, WHITE, BLANK));

	tex = LoadTextureFromImage(image1);
	image1.Unload();
}

CustomClient::~CustomClient()
{
	tex.Unload();
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


	// Lock the cursor
	DisableCursor();
}

void CustomClient::update()
{
	for (RakNet::Packet* packet = peerInterface->Receive(); packet; peerInterface->DeallocatePacket(packet),
		packet = peerInterface->Receive())
	{
		// Pass all packets through the system
		Client::processSystemMessage(packet);


		RakNet::BitStream bsIn(packet->data, packet->length, false);
		RakNet::MessageID messageID;
		bsIn.Read(messageID);

		// Check for custom packets
		switch (messageID)
		{
		case ID_SERVER_UPDATE_HEALTH:
			updateHealth(bsIn);
			break;
		case ID_SERVER_RESPAWN_PLAYER:
			respawnPlayer(bsIn);
			break;
		case ID_SERVER_CREATE_EXPLOSION:
			createExplosionEffect(bsIn);
			break;


		default:
			break;
		}
	}


	// Use the systems time before its updated
	float deltaTime = (RakNet::GetTime() - Client::getTime()) * 0.001f;
	Client::systemUpdate();


	if (myClientObject)
	{
		((PlayerObject*)myClientObject)->update(deltaTime);
	}


	// Update particles
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		particles[i].lifeRemaining -= deltaTime;
		particles[i].position += particles[i].velocity * deltaTime;
		particles[i].velocity -= raylib::Vector3(0, 0.5f, 0) * deltaTime;

		if (particles[i].lifeRemaining <= 0)
		{
			particles.erase(particles.begin() + i);
		}
	}
}


void CustomClient::draw3D(raylib::Camera3D& cam)
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


	// Draw particles
	for (auto& it : particles)
	{
		rlSetBlendMode(1, 1, BLEND_MULTIPLIED);
		DrawBillboard(cam, tex, it.position, 2, it.color);
		rlSetBlendMode(1, 0, BLEND_ALPHA);
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

void CustomClient::draw2D()
{
	// Draw FPS and ping
	DrawFPS(10, 10);
	std::ostringstream os;
	os << peerInterface->GetAveragePing(peerInterface->GetSystemAddressFromIndex(0)) << " Ping";
	RayDrawText(os.str().c_str(), 10, 30, 20, DARKGREEN);

	if (myClientObject)
	{
		PlayerObject* playerObj = ((PlayerObject*)myClientObject);

		if (playerObj->isDead())
		{
			// Display respawn prompt
			RayDrawText("Fire to respawn", GetScreenWidth()*0.5f - MeasureText("Fire to respawn", 40)*.5f, 350, 40, BLACK);
		}
		else
		{
			// Display health
			std::ostringstream os;
			os << "Health: " << playerObj->getHealth();
			RayDrawText(os.str().c_str(), GetScreenWidth() * 0.5f - MeasureText(os.str().c_str(), 40) * .5f, 350, 40, BLACK);
		}
	}
}


void CustomClient::updateHealth(RakNet::BitStream& bsIn)
{
	unsigned int id;
	float damage;
	bsIn.Read(id);
	bsIn.Read(damage);

	// Deal damage to a client object
	if (id == getClientID())
	{
		((PlayerObject*)myClientObject)->dealDamage(damage);
	}
	else if (gameObjects.count(id) > 0)
	{
		PlayerObject* obj = dynamic_cast<PlayerObject*>(gameObjects[id]);
		if (obj)
		{
			obj->dealDamage(damage);
		}
	}
}

void CustomClient::respawnPlayer(RakNet::BitStream& bsIn)
{
	unsigned int id;
	raylib::Vector3 spawnPos;
	bsIn.Read(id);
	bsIn.Read(spawnPos);

	// Respawn a client object
	if (id == getClientID())
	{
		((PlayerObject*)myClientObject)->respawn(spawnPos);
	}
	else if (gameObjects.count(id) > 0)
	{
		PlayerObject* obj = dynamic_cast<PlayerObject*>(gameObjects[id]);
		if (obj)
		{
			obj->respawn(spawnPos);
		}
	}
}

void CustomClient::createExplosionEffect(RakNet::BitStream& bsIn)
{
	raylib::Vector3 position;
	float radius;
	bsIn.Read(position);
	bsIn.Read(radius);


	// Create explosion using particles
	for (int i = 0; i < 50; i++)
	{
		raylib::Vector3 offset;
		offset.x = GetRandomValue(-100, 100) * 0.01f;
		offset.y = GetRandomValue(-100, 100) * 0.01f;
		offset.z = GetRandomValue(-100, 100) * 0.01f;
		offset = offset.Normalize() * GetRandomValue(0, 100) * 0.05f;

		raylib::Vector3 velocity;
		velocity.x = GetRandomValue(-100, 100) * 0.01f;
		velocity.y = GetRandomValue(-100, 0) * 0.01f;
		velocity.z = GetRandomValue(-100, 100) * 0.01f;
		velocity = velocity.Normalize() * GetRandomValue(0, 100) * 0.05f;

		particles.push_back({ position + offset + raylib::Vector3(0,1,0), velocity, {255, 100, 0, 150}, .5f + GetRandomValue(-50, 50) * 0.005f });
	}
}


void CustomClient::createExplosion(raylib::Vector3 center, float radius)
{
	// Only check with the player, as this is for prediction
	raylib::Vector3 dir = myClientObject->getPosition() - center;
	float dist = dir.Length();

	// Players should all have spheres, so subtract the radius to get dist to closest point
	Sphere* col = dynamic_cast<Sphere*>(myClientObject->getCollider());
	if (col)
	{
		dist -= col->getRadius();
	}


	// If within the explosion radius, apply force
	if (dist < radius)
	{
		myClientObject->applyForce(dir.Normalize() * 20, Vector3Zero());
	}
}


Input CustomClient::getInput()
{
	Input input;

	input.fire = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
	input.jump = IsKeyPressed(KEY_SPACE);

	input.mousePos = GetMousePosition();

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
		// Custom static object class used for drawing
		raylib::Color color;
		bsIn.Read(color);
		return new WorldObject(objectInfo.state.position, objectInfo.state.rotation, color, objectInfo.collider);
	}



		// Invalid ID
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
		unsigned int ownerID;
		bsIn.Read(radius);
		bsIn.Read(damage);
		bsIn.Read(ownerID);

		Rocket* obj = new Rocket(objectInfo.state, objectID, objectInfo.collider, radius, damage, ownerID);
		obj->setClient(this);
		return obj;
	}

	case 2000:
	{
		// Create a player object
		float health;
		raylib::Color color;
		bsIn.Read(health);
		bsIn.Read(color);
		return new PlayerObject(objectInfo.state, objectID, objectInfo.collider, health, color, objectInfo.friction);
	}


		// Invalid ID
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
