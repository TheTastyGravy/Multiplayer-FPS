#include "PlayerObject.h"
#include "Sphere.h"


PlayerObject::PlayerObject(unsigned int clientID) :
	ClientObject(PhysicsState(), clientID, 1, .3f), health(100), color(RED), server(nullptr)
{
	typeID = 2000;
}

PlayerObject::PlayerObject(PhysicsState initState, unsigned int clientID, Collider* collider, float health, raylib::Color color, float friction) :
	ClientObject(initState, clientID, 1, .3f, collider, 0, 1, friction, true), health(health), color(color), server(nullptr)
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

	// Use the mouse position to rotate the object, clamping vertical
	diff.rotation = raylib::Vector3(0, input.mousePos.x * mouseAccelX, Clamp(input.mousePos.y * mouseAccelY, -PI * .45f, PI * .45f)) - rotation;
	// A dead player can only look around
	if (isDead())
	{
		return diff;
	}


	// Use input rotated to the direction the player is facing
	raylib::Vector3 desieredVel = { input.movement.x, 0, input.movement.y };
	desieredVel *= moveAcceleration;
	desieredVel = desieredVel.Transform(MatrixRotateY(rotation.y));

	// Ignore the Y component
	raylib::Vector3 planarVel = getVelocity();
	planarVel.y = 0;

	// If there is input and we are over the speed cap
	if (!(desieredVel.x == 0 && desieredVel.z == 0) && (planarVel + desieredVel).Length() > moveSpeedCap)
	{
		// Use the combined direction at the move speed
		desieredVel = (desieredVel + planarVel).Normalize() * moveSpeedCap - planarVel;
	}

	// If on the ground and jumping, jump
	if (groundTimmer > 0 && input.jump)
	{
		desieredVel.y = 13 - velocity.y;
	}


	diff.velocity = desieredVel;
	return diff;
}


void PlayerObject::update(float deltaTime)
{
	// Apply gravity. This is a bad place for it, as if no inputs are receved by the server for a while, delta 
	// time will build up and can cause the player to fall through the ground
	applyForce(raylib::Vector3(0, -30, 0) * getMass() * deltaTime, Vector3Zero());


	// Decrement the timmer
	groundTimmer -= deltaTime;
}

void PlayerObject::draw() const
{
	if (isDead())
	{
		return;
	}


	// Draw the player using their sphere collider
	Sphere* sphere = dynamic_cast<Sphere*>(getCollider());
	if (!sphere)
	{
		return;
	}
	
	DrawSphere(position, sphere->getRadius(), color);
}


void PlayerObject::dealDamage(float damage)
{
	health -= damage;

	// Dont let health go negitive
	if (health < 0)
	{
		health = 0;
	}
}

void PlayerObject::respawn(raylib::Vector3 position)
{
	// Reset the player
	health = 100;
	this->position = position;
	velocity = Vector3Zero();
}
