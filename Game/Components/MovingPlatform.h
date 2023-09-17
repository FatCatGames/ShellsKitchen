#pragma once
#include "Physics/PhysicsEngine.h"
#include "Components/Physics/Collisions/BoxCollider.h"

class MovingPlatform : public Component
{
public:
	MovingPlatform();
	~MovingPlatform();
	void Awake() override;
	void FixedUpdate() override;
	void RenderInProperties(std::vector<Component*>& aComponentList);
	void OnCollisionEnter(Collider* aCollider) override;
	void OnCollisionExit(Collider* aCollider) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

private:
	physx::PxRigidDynamic* myCastedActor;
	RigidBody* myRigidBody;
	Animator* myAnimator;
	std::vector<Collider*> myObjectsToCarry;
	std::vector<BoxCollider*> myStaticChildren;
	Vector3f myInitialPosition;
	Vector3f myNextPosition;
	Vector3f myFinalPosition;
	Vector3f myOriginalAngle;
	bool myShouldLoop;
	bool myWaitingForClick = false;
	bool myWaitingForClickDelay = false;
	bool myShouldCarryRigidbodies = true;
	bool myHasHermitCrab = false;
	int myWaitingForClickTargetID = -1;
	float myTotalTime;
	float myBasicBitchUniversalDelay = 0;
	float myElapsedDelayTime;
	float myTravelPrecent;
	float myShellTravelPrecent;

	struct Waypoint
	{
		Vector3f position = {0,0,0};
		float delay = 0;
		float speed = 1;
	};
	std::vector<Waypoint> myWaypoints;
	int myCurrentWaypoint;
	float myAngle = 0;

	GameObject* myCrabContainer;
	GameObject* myCrab;

	bool myIsMovingBack = false;
	int myID = 0;
	float myTest;
};