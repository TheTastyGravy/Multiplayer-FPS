#pragma once
#include "raylib-cpp.hpp"
#include <RakPeerInterface.h>
#include "ClientObject.h"
#include "RingBuffer.h"
#include <vector>
#include <unordered_map>


/// <summary>
/// Used by a player to connect to a server and play the game. 
/// Handles functionality such as getting input, drawing, and client side logic (i.e. Things that dont need to be syncronised)
/// </summary>
class Client
{
public:
	Client();
	virtual ~Client();

protected:
	// Perfrom an update on the system, including prediction and input
	void systemUpdate();
	// Process packets that are used by the system
	void processSystemMessage(const RakNet::Packet* packet);


	// Used by systemUpdate for prediction and to send it to the server
	virtual Input getInput() = 0;

	// Holds information used to create objects. Static objects will only need part or it
	struct ObjectInfo
	{
		PhysicsState state;
		// The objects collider, or null pointer if it doesnt have one
		Collider* collider = nullptr;
		float mass = 1, elasticity = 1, friction = 1;
	};
	
	/// <summary>
	/// Factory method used to create custom static objects
	/// </summary>
	/// <param name="objectInfo">System defined information for the object. Not all information is nessesary fro static objects</param>
	/// <param name="bsIn">Custom paramiters. Only read as much is nessesary for the object</param>
	/// <returns>A pointer to the new static object</returns>
	virtual StaticObject* staticObjectFactory(unsigned int typeID, ObjectInfo& objectInfo, RakNet::BitStream& bsIn) = 0;
	/// <summary>
	/// Factory method used to create custom game objects
	/// </summary>
	/// <param name="objectID">The object ID that needs to be given to the object. If the objects ID does not match, it will be deleted</param>
	/// <param name="objectInfo">System defined information for the object</param>
	/// <param name="bsIn">Custom paramiters</param>
	/// <returns>A pointer to the new game object</returns>
	virtual GameObject* gameObjectFactory(unsigned int typeID, unsigned int objectID, ObjectInfo& objectInfo, RakNet::BitStream& bsIn) = 0;
	/// <summary>
	/// Factory method used to create a custom client object. Use clientID for the objects ID
	/// </summary>
	/// <param name="objectInfo">System defined information for the object</param>
	/// <param name="bsIn">Custom paramiters</param>
	/// <returns>A pointer to the new client object</returns>
	virtual ClientObject* clientObjectFactory(unsigned int typeID, ObjectInfo& objectInfo, RakNet::BitStream& bsIn) = 0;


	unsigned int getClientID() const { return clientID; }
	RakNet::Time getTime() const { return lastUpdateTime; }

private:
	// Read a collider from a bit stream. Instantiated with new
	Collider* readCollider(RakNet::BitStream& bsIn);

	// Create static object instances from data
	void createStaticObjects(RakNet::BitStream& bsIn);
	// Create a game object instance from data
	void createGameObject(RakNet::BitStream& bsIn);
	// Create the client object we own from data
	void createClientObject(RakNet::BitStream& bsIn);

	// Destroy the game object of objectID
	void destroyGameObject(unsigned int objectID);
	// Destroy all staticObjects, gameObjects, and myClientObject
	void destroyAllObjects();

	// Used when an object update is receved from the server
	void applyServerUpdate(RakNet::BitStream& bsIn, const RakNet::Time& timeStamp);



protected:
	RakNet::RakPeerInterface* peerInterface;

	// Static objects are receved from the server after connecting
	std::vector<StaticObject*> staticObjects;
	// All game objects. This includes other players client objects
	// <object ID, game object>
	std::unordered_map<unsigned int, GameObject*> gameObjects;
	// The object owned by this client
	ClientObject* myClientObject;

private:
	// The ID assigned to this client by the server
	// The user should not be able to change this, so give them a getter
	unsigned int clientID;

	// Used to determine delta time
	RakNet::Time lastUpdateTime;

	// Buffer of <time of input, state at time, player input>
	RingBuffer<std::tuple<RakNet::Time, PhysicsState, Input>> inputBuffer;
};