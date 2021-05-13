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
	Input() :
		movement(Vector2Zero()), mouseDelta(Vector2Zero()), mousePos(Vector2Zero()), jump(false), fire(false),
		bool1(false), bool2(false), bool3(false), bool4(false), bool5(false), bool6(false), bool7(false), bool8(false),
		float1(0), float2(0), float3(0), float4(0),
		vec1(Vector3Zero()), vec2(Vector3Zero()), vec3(Vector3Zero()), vec4(Vector3Zero())
	{}


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
	ClientObject(raylib::Vector3 position, raylib::Vector3 rotation, unsigned int clientID, float mass, float elasticity, Collider* collider = nullptr, float linearDrag = 0, float angularDrag = 0, float friction = 1, bool lockRotation = false);
	ClientObject(PhysicsState initState, unsigned int clientID, float mass, float elasticity, Collider* collider = nullptr, float linearDrag = 0, float angularDrag = 0, float friction = 1, bool lockRotation = false);


	// Appends serialization data to bsInOut, used to create game and client objects on clients
	virtual void serialize(RakNet::BitStream& bsInOut) const
	{
		GameObject::serialize(bsInOut);
	}


	// Returns a diference in physics state
	virtual PhysicsState processInputMovement(const Input& input) const = 0;
	// Process actions, i.e. things that do not cause movement
	virtual void processInputAction(const Input& input, RakNet::Time timeStamp) = 0;


	// Used internally by client to apply a server update, then reapply input predictions
	void updateStateWithInputBuffer(const PhysicsState& state, RakNet::Time stateTime, RakNet::Time currentTime, const RingBuffer<std::tuple<RakNet::Time, PhysicsState, Input>>& inputBuffer, bool useSmoothing = false);
};