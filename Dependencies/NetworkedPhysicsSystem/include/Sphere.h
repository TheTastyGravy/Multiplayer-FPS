#pragma once
#include "Collider.h"

class Sphere : public Collider
{
public:
	Sphere(float radius) : radius(radius)
	{
		shapeID = 0;
	}


	raylib::Matrix calculateInertiaTensor(float mass) const
	{
		// The moment of inertia for a solid sphere
		float inertia = 2.f / 5.f * mass * radius * radius;

		return {
			inertia, 0, 0, 0,
			0, inertia, 0, 0,
			0, 0, inertia, 0,
			0, 0, 0, 1
		};
	}

	float getRadius() const { return radius; }

protected:
	virtual void serialize(RakNet::BitStream& bsIn) const
	{
		bsIn.Write(shapeID);
		bsIn.Write(radius);
	}


private:
	float radius;
};