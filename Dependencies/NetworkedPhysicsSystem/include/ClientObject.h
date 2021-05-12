#pragma once
#include "GameObject.h"

// Forward declaration
template<class T>
class RingBuffer;

/// <summary>
/// Contains player input processed by client objects
/// </summary>
struct Input
{
	Input();


	raylib::Vector2 movement, mouseDelta, mousePos;
	bool jump, fire;
	
	bool bool1, bool2, bool3, bool4, bool5, bool6, bool7, bool8;
	float float1, float2, float3, float4;
	raylib::Vector3 vec1, vec2, vec3, vec4;
};

/// <summary>
/// A game object that is owned and can be controlled by a client
/// </summary>
class ClientObject : public GameObject
{
public:
	ClientObject();
	ClientObject(raylib::Vector3 position, raylib::Vector3 rotation, unsigned int clientID, float mass, float elasticity, Collider* collider = nullptr);
	ClientObject(PhysicsState initState, unsigned int clientID, float mass, float elasticity, Collider* collider = nullptr);


	// Appends serialization data to bsInOut, used to create game and client objects on clients
	virtual void serialize(RakNet::BitStream& bsInOut) const;


	// Returns a diference in physics state
	virtual PhysicsState processInputMovement(const Input& input) const = 0;
	// Process actions, i.e. things that do not cause movement
	virtual void processInputAction(const Input& input, RakNet::Time timeStamp) = 0;


	// Used internally by client to apply a server update, then reapply input predictions
	void updateStateWithInputBuffer(const PhysicsState& state, RakNet::Time stateTime, RakNet::Time currentTime, const RingBuffer<std::tuple<RakNet::Time, PhysicsState, Input>>& inputBuffer, bool useSmoothing = false);
};