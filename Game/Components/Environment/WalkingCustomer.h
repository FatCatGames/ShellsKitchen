#pragma once
#include "Customer.h"

class Animator;
class WalkingCustomer : public Customer
{

public:
	void FixedUpdate() override;
	void SetWaypoints(const std::vector<Vector3f>& aWaypoints);

private:
	bool CrossedTurnBoundary();

	std::vector<Vector3f> myWaypoints;
	std::vector<Vector3f> myTurnBoundaries;
	int myCurrentWaypointIndex = 0;
	float myWalkSpeed = 2;
	float myTurnDist = 1;
	float myRotateSpeed = 120;
};