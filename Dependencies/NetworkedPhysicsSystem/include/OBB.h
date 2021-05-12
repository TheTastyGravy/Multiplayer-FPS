#pragma once
#include "Collider.h"

class OBB : public Collider
{
public:
	OBB(raylib::Vector3 halfExtents);


	raylib::Matrix calculateInertiaTensor(float mass) const;

	raylib::Vector3 getHalfExtents() const;

protected:
	virtual void serialize(RakNet::BitStream& bsIn) const;


private:
	raylib::Vector3 halfExtents;

};