#include "PlayerObject.h"

#include <iostream>


PlayerObject::PlayerObject(unsigned int clientID) :
	ClientObject(PhysicsState(), clientID, 1, .3f), health(100), groundContacts(0)
{
	typeID = 2000;
}

PlayerObject::PlayerObject(PhysicsState initState, unsigned int clientID, Collider* collider, float health) : 
	ClientObject(initState, clientID, 1, .3f, collider), health(health), groundContacts(0)
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
		desieredVel.y = 10 + getVelocity().y;
	}


	diff.velocity = desieredVel;

	//use mouse delta to rotate
	diff.rotation = { 0, input.mouseDelta.x * mouseAccelX, input.mouseDelta.y * mouseAccely };


	return diff;
}

void PlayerObject::processInputAction(const Input& input, RakNet::Time timeStamp)
{
	//if fire is true, create rocket on server in the direction the player is facing
}



void PlayerObject::update(float deltaTime)
{
	//gravity
	applyForce(raylib::Vector3(0, -5, 0) * getMass() * deltaTime, Vector3Zero());

	//do not roll. if we roll, friction will not slow us down
	angularVelocity = Vector3Zero();


	//reset contacts
	groundContacts = 0;


	if (health <= 0)
	{
		//die
	}
}

void PlayerObject::draw()
{
	DrawSphere(position, 5, RED);
}



void PlayerObject::server_onCollision(StaticObject* other)
{
	//this does not account for colliding with multiple objects in the same tick

	//if the direction is close to down, we are grounded. this is dumb
	raylib::Vector3 dir = position - other->getPosition();
	if (dir.Normalize().y > 0.75f)
	{
		groundContacts += 1;
	}
}

void PlayerObject::client_onCollision(StaticObject* other)
{
	//this does not account for colliding with multiple objects in the same tick

	//if the direction is close to down, we are grounded. this is dumb
	raylib::Vector3 dir = position - other->getPosition();
	if (dir.Normalize().y > 0.75f)
	{
		groundContacts += 1;
	}
}
