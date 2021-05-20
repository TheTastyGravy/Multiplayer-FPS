#pragma once
#include "raylib-cpp.hpp"
#include <BitStream.h>

// Forward declaration
class StaticObject;


class Collider
{
public:
	// Calculate the moment of inertia tensor of an object with this colliders shape
	virtual raylib::Matrix calculateInertiaTensor(float mass) const = 0;
	// Get radius of a sphere containing this collider
	virtual float getBoundingSphereRadius() const = 0;

protected:
	// Used by StaticObject.serialize to write its collider. Because 
	// this is its only use, StaticObject can be made a friend to access it
	virtual void serialize(RakNet::BitStream& bsIn) const = 0;
	friend StaticObject;


protected:
	// Used to determine what derived class this collider is
	int shapeID = -1;
public:
	int getShapeID() const { return shapeID; }

	// The total number of shape IDs in use
	static const int SHAPE_COUNT = 2;
};