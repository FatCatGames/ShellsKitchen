#pragma once
#include "Customer.h"

class DeliveryDesk;
class LineupCustomer : public Customer
{

public:
	void Update() override;
	void FixedUpdate() override;
	void SetNextWaypoint(const Vector3f& aWaypoint);
	void SetInitialPosition(const Vector3f& aPosition, const Vector3f& aRotation);
	void SetOrderId(int anOrderId) { myOrderId = anOrderId; }
	int GetOrderId() { return myOrderId; }
	void WalkAway(Vector3f aHorizontalOffsetVector);
	void StartWalkingAway();

private:
	bool myHasReachedDesk = false;
	bool myShouldWalkAway = false;
	bool myIsWalkingAway = false;
	int myOrderId = -1;
	bool myIsSetup = false;
	float myWalkSpeed = 2;
	float myTurnDist = 1;
	float myRotateSpeed = 260;
	bool myShouldRandomizeFrame = false;

	Vector3f myCurrentTarget;
	Vector3f myTurnTarget;
	Vector3f myLeaveTarget;

	
	DeliveryDesk* myDesk;

	Vector3f myInitialPosition;
	Vector3f myInitialRotation;
};