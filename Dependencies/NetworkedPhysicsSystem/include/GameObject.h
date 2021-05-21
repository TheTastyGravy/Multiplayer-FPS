#pragma once
#include "StaticObject.h"


/// <summary>
/// Contains the variables that can be used to change a game objects physics state
/// </summary>
struct PhysicsState
{
	PhysicsState() :
		position(0, 0, 0), rotation(0, 0, 0), velocity(0, 0, 0), angularVelocity(0, 0, 0)
	{}
	PhysicsState(raylib::Vector3 position, raylib::Vector3 rotation, raylib::Vector3 velocity = { 0,0,0 }, raylib::Vector3 angularVelocity = { 0,0,0 }) :
		position(position), rotation(rotation), velocity(velocity), angularVelocity(angularVelocity)
	{}


	raylib::Vector3 position;
	raylib::Vector3 rotation;

	raylib::Vector3 velocity;
	raylib::Vector3 angularVelocity;
};

/// <summary>
/// An object that has physics and can be syncronised between the server and a client
/// </summary>
class GameObject : public StaticObject
{
public:
	GameObject();
	GameObject(raylib::Vector3 position, raylib::Vector3 rotation, unsigned int objectID, float mass, float elasticity, Collider* collider = nullptr, float linearDrag = 0, float angularDrag = 0, float friction = 1, bool lockRotation = false);
	GameObject(PhysicsState initState, unsigned int objectID, float mass, float elasticity, Collider* collider = nullptr, float linearDrag = 0, float angularDrag = 0, float friction = 1, bool lockRotation = false);

	// Appends serialization data to bsInOut, used to create game objects on clients
	virtual void serialize(RakNet::BitStream& bsInOut) const override;


	// Apply a physics step to the object
	void physicsStep(float timeStep);

	/// <summary>
	/// Apply a force at a point on the object
	/// </summary>
	/// <param name="relitivePosition">Where, in local space, the force should be applied. 
	///								   If set to 0,0,0 then only linear velocity will be affected</param>
	void applyForce(const raylib::Vector3& force, const raylib::Vector3& relitivePosition);
	/// <summary>
	/// Resolve a collision with another object, applying normal and friction forces
	/// </summary>
	/// <param name="otherObject">The object this one has collided with</param>
	/// <param name="contact">The world space contact point where the collision occured</param>
	/// <param name="collisionNormal">The normal in world space relitive to this object</param>
	/// <param name="isOnServer">Used to determine which collision callbacks to use</param>
	/// <param name="shouldAffectOther">Should the other object have forces applied?</param>
	void resolveCollision(StaticObject* otherObject, const raylib::Vector3& contact, const raylib::Vector3& collisionNormal, bool shouldAffectOther = true);

	/// <summary>
	/// Update this objects physics state, then extrapolate to the current time
	/// </summary>
	/// <param name="state">The new state</param>
	/// <param name="stateTime">The time in the past that the state belongs to</param>
	/// <param name="currentTime">The time that should be extrapolated up to</param>
	/// <param name="useSmoothing">Should the change be applied with Exponentialy Smoothed Moving Average, or set directly?</param>
	void updateState(const PhysicsState& state, RakNet::Time stateTime, RakNet::Time currentTime, bool useSmoothing = false);
	/// <summary>
	/// Apply a diff state to this object, then extrapolate to the current time
	/// </summary>
	/// <param name="diffState">A physics state representing the difference between the current state and the new one</param>
	/// <param name="stateTime">The time in the past that the state belongs to</param>
	/// <param name="currentTime">The time that should be extrapolated up to</param>
	/// <param name="useSmoothing">Should the change be applied with Exponentialy Smoothed Moving Average, or set directly?</param>
	/// <param name="shouldUpdateObjectTime">Should this object's packet time be updated?</param>
	void applyStateDiff(const PhysicsState& diffState, RakNet::Time stateTime, RakNet::Time currentTime, bool useSmoothing = false, bool shouldUpdateObjectTime = false);


	unsigned int getID() const { return objectID; }
	// Returns the timestamp of the last update packet applied
	RakNet::Time getTime() const { return lastPacketTime; }

	// Returns the current PhysicsState of the object
	PhysicsState getCurrentState() const { return { position, rotation, velocity, angularVelocity }; }

	raylib::Vector3 getVelocity() const { return velocity; }
	raylib::Vector3 getAngularVelocity() const { return angularVelocity; }

	float getMass() const { return mass; }
	raylib::Matrix getMoment() const { return moment; }
	float getElasticity() const { return elasticity; }

	float getlinearDrag() const { return linearDrag; }
	float getAngularDrag() const { return angularDrag; }
	float getFriction() const { return friction; }


protected:
	/// <summary>
	/// Event called after resolving a collision
	/// </summary>
	/// <param name="contact">The contact point in world space</param>
	/// <param name="normal">The collision normal relitive to this object</param>
	virtual void onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal) {}

	/// <summary>
	/// Called at the start of every physicsStep. Should not rely on external game state, as physicsStep is used in prediction
	/// </summary>
	virtual void fixedUpdate(float timeStep) {};



protected:
	// How far away from the servers position we can be before being snapped to it when using smoothing
	const float smooth_snapDistance = 10;
	// How far from the servers position we can be and not be updated
	const float smooth_threshold = 0.75f;
	// How much to move toward the servers position when using smoothing
	const float smooth_moveFraction = 0.1f;

	// A unique identifier for this object, nessesary for sending updates over the network
	const unsigned int objectID;

	// The timestamp of the last packet receved for this object
	// Time in milliseconds. Multiply by 0.001 for seconds
	RakNet::Time lastPacketTime;

	
	raylib::Vector3 velocity;
	raylib::Vector3 angularVelocity;

	float mass;
	raylib::Matrix moment;
	float elasticity;

	float linearDrag;
	float angularDrag;
	float friction;

	bool lockRotation;
};