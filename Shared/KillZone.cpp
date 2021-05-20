#include "KillZone.h"


KillZone::KillZone() :
	StaticObject()
{
	typeID = 2;
}

KillZone::KillZone(raylib::Vector3 position, raylib::Vector3 rotation, Collider* collider) : 
	StaticObject(position, rotation, collider)
{
	typeID = 2;
}

KillZone::~KillZone()
{}


void KillZone::serialize(RakNet::BitStream& bsInOut) const
{
	StaticObject::serialize(bsInOut);
}
