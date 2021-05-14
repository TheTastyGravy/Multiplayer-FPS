#include "WorldObject.h"
#include "OBB.h"
#include "Sphere.h"


WorldObject::WorldObject() : 
	StaticObject(), color(RED)
{
	typeID = 1;
}

WorldObject::WorldObject(raylib::Vector3 position, raylib::Vector3 rotation, raylib::Color color, Collider* collider) : 
	StaticObject(position, rotation, collider), color(color)
{
	typeID = 1;
}

WorldObject::~WorldObject()
{}


void WorldObject::serialize(RakNet::BitStream & bsInOut) const
{
	StaticObject::serialize(bsInOut);
	bsInOut.Write(color);
}


void WorldObject::draw() const
{
	Sphere* sphere = dynamic_cast<Sphere*>(getCollider());
	OBB* obb = dynamic_cast<OBB*>(getCollider());

	// Draw the object using its collider
	if (sphere)
	{
		DrawSphere(position, sphere->getRadius(), color);
	}
	else if (obb)
	{
		DrawCubeV(position, obb->getHalfExtents() * 2, color);
	}
	else
	{
		DrawPoint3D(position, color);
	}
}
