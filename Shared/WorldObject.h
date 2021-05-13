#pragma once
#include "StaticObject.h"
#include "IDrawable.h"


class WorldObject : public StaticObject, public IDrawable
{
public:
	WorldObject();
	WorldObject(raylib::Vector3 position, raylib::Vector3 rotation, raylib::Color color, Collider* collider = nullptr);
	virtual ~WorldObject();


	virtual void serialize(RakNet::BitStream& bsInOut) const override;

	virtual void draw() const override;


protected:
	raylib::Color color;
};