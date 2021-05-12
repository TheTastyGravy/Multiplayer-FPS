#pragma once
#include "Collider.h"

class Sphere : public Collider
{
public:
	Sphere(float radius);


	raylib::Matrix calculateInertiaTensor(float mass) const;

	float getRadius() const;

protected:
	virtual void serialize(RakNet::BitStream& bsIn) const;


private:
	float radius;
};