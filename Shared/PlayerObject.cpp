#include "PlayerObject.h"
#include "Sphere.h"

#include <iostream>


PlayerObject::PlayerObject(unsigned int clientID) :
	ClientObject(PhysicsState(), clientID, 1, .3f), health(100), color(RED), groundContacts(0), server(nullptr)
{
	typeID = 2000;
}

PlayerObject::PlayerObject(PhysicsState initState, unsigned int clientID, Collider* collider, float health, raylib::Color color, float friction) :
	ClientObject(initState, clientID, 1, .3f, collider, 0, 1, friction), health(health), color(color), groundContacts(0), server(nullptr)
{
	typeID = 2000;
}

PlayerObject::~PlayerObject()
{
}



void PlayerObject::serialize(RakNet::BitStream& bsInOut) const
{
	ClientObject::serialize(bsInOut);
	bsInOut.Write(health);
	bsInOut.Write(color);
}



PhysicsState PlayerObject::processInputMovement(const Input& input) const
{
	PhysicsState diff;

	raylib::Vector3 desieredVel = { input.movement.x, 0, input.movement.y };
	desieredVel *= moveAcceleration;

	//rotate desiered vel to match facing direction
	desieredVel = desieredVel.Transform(MatrixRotateY(rotation.y));

	//get velocity without y component
	raylib::Vector3 planarVel = getVelocity();
	planarVel.y = 0;

	//input not zero and total over speed cap
	if (!(desieredVel.x == 0 && desieredVel.z == 0) && (planarVel + desieredVel).Length() > moveSpeedCap)
	{
		//dont go over the cap, but allow change in direction
		desieredVel = (desieredVel + planarVel).Normalize() * moveSpeedCap - planarVel;
	}


	//jump
	if (groundContacts > 0 && input.jump)
	{
		desieredVel.y = 20 + getVelocity().y;
	}


	diff.velocity = desieredVel;

	//use mouse delta to rotate
	diff.rotation = { 0, input.mouseDelta.x * mouseAccelX, input.mouseDelta.y * mouseAccely };


	return diff;
}

void PlayerObject::processInputAction(const Input& input, RakNet::Time timeStamp)
{
	// Only perform actions when the server can be acessed
	if (!server)
	{
		return;
	}


	if (input.fire)
	{
		//find direction player is facing
		float x, y;
		x = getRotation().y - PI * 0.5f;
		y = -getRotation().z;
		raylib::Vector3 forward(cos(y) * cos(x), sin(y), cos(y) * sin(x));

		PhysicsState state;
		state.position = position + forward * 6;	//create it infront of the player
		state.velocity = forward * 20;


		RakNet::BitStream bs;
		bs.Write(10.f);	//radius
		bs.Write(50.f);	//damage

		server->createObject(1000, state, timeStamp, &bs);
	}
}



void PlayerObject::update(float deltaTime)
{
	//gravity
	applyForce(raylib::Vector3(0, -15, 0) * getMass() * deltaTime, Vector3Zero());

	//do not roll. if we roll, friction will not slow us down	TEMP
	angularVelocity = Vector3Zero();


	//reset contacts
	groundContacts = 0;


	if (health <= 0)
	{
		//die
	}
}

void PlayerObject::draw() const
{
	Sphere* sphere = dynamic_cast<Sphere*>(getCollider());
	if (!sphere)
		return;

	DrawSphere(position, sphere->getRadius(), color);
}



void PlayerObject::server_onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal)
{
	//if the collision normal is close to down, we are on ground
	if (normal.y < -0.85f)
	{
		groundContacts += 1;
	}
}

void PlayerObject::client_onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal)
{
	//if the collision normal is close to down, we are on ground
	if (normal.y < -0.85f)
	{
		groundContacts += 1;
	}
}
