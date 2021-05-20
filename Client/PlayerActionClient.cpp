#include "../Shared/PlayerObject.h"


void PlayerObject::processInputAction(const Input& input, RakNet::Time timeStamp)
{
	//no actions on client
}

void PlayerObject::onCollision(StaticObject* other, raylib::Vector3 contact, raylib::Vector3 normal)
{
	// If the collision normal is close to down, we are on ground
	if (normal.y < -0.85f)
	{
		groundTimmer = 0.1f;
	}
}