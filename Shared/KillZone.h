#pragma once
#include "StaticObject.h"


class KillZone : public StaticObject
{
public:
	KillZone();
	KillZone(raylib::Vector3 position, raylib::Vector3 rotation, Collider* collider = nullptr);
	virtual ~KillZone();


	virtual void serialize(RakNet::BitStream& bsInOut) const override;
};