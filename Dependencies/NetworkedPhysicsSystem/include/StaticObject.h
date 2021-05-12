#pragma once
#include "raylib-cpp.hpp"
#include "Collider.h"
#include <BitStream.h>

// Forward declaration
class CollisionSystem;


/// <summary>
/// The most basic networked object. It is only sent to a client once, has no physics, and is not syncronised. 
/// Should only be used for static geometry
/// </summary>
class StaticObject
{
	// Contact forces need to set the objects position directly. Instead of making a setter, make the class a friend
	friend CollisionSystem;
public:
	StaticObject();
	StaticObject(raylib::Vector3 position, raylib::Vector3 rotation, Collider* collider = nullptr);
	virtual ~StaticObject();

	
	// Appends serialization data to bsInOut, used to create objects on clients
	virtual void serialize(RakNet::BitStream& bsInOut) const;


	// If an object is static, it does not have physics
	bool isStatic() const;

	Collider* getCollider() const;

	raylib::Vector3 getPosition() const;
	raylib::Vector3 getRotation() const;


	
protected:
	// An identifier used for factory methods.
	// Every custom class needs to set this to a unique value
	int typeID = 0;

	// Derived class GameObject sets this to false. Used for physics
	bool bIsStatic = true;

	// Collider used for collision
	Collider* collider;

	raylib::Vector3 position;
	raylib::Vector3 rotation;
};