#include "Game.pch.h"
#include "WalkingCustomer.h"
#include "Physics/PhysicsEngine.h"
#include "Components\Physics\Collisions\Collider.h"
#include "Components\Animator.h"



void WalkingCustomer::FixedUpdate()
{
	if (myStunTimer > 0) return;
	myAnimator->SetBool("Walking", true);

	physx::PxTransform physXTransform = myCastedActor->getGlobalPose();
	Catbox::Quaternion physXRotation = Catbox::Quaternion({ physXTransform.q.x, physXTransform.q.y, physXTransform.q.z, physXTransform.q.w });
	Vector3f physXVectorRotation = Catbox::Quaternion::ToEulerAngles(physXRotation);
	Vector3f physXPosition = { physXTransform.p.x, physXTransform.p.y, physXTransform.p.z };

	float currentRotation = std::fmod(physXVectorRotation.y, 360.f);
	Vector3f dirToNext = (myWaypoints[myCurrentWaypointIndex + 1] - physXPosition).GetNormalized();
	float targetRotation = (std::atan2(dirToNext.x, dirToNext.z) * 57.3f);

	float angleDiff = targetRotation - currentRotation;
	float rotDir;
	if (abs(angleDiff) < 3) angleDiff = 0;
	else
	{
		if (angleDiff >= 180.f) rotDir = -1;
		else if (angleDiff <= -180.f) rotDir = 1;
		else
		{
			rotDir = (angleDiff > 0) ? 1 : -1;
		}
	}

	myRigidBody->SetActorRotation({ 0, physXVectorRotation.y + (rotDir * myRotateSpeed * fixedDeltaTime), 0 });
	physXTransform = myCastedActor->getGlobalPose();

	Vector3f newVectorTarget = physXPosition + (myTransform->forward() * myWalkSpeed * fixedDeltaTime);
	physXTransform.p = { newVectorTarget.x, newVectorTarget.y, newVectorTarget.z };
	myCastedActor->setKinematicTarget(physXTransform);

	DebugDrawer::DrawSphere(myTurnBoundaries[myCurrentWaypointIndex], 0.3f);
	if (CrossedTurnBoundary())
	{
		myCurrentWaypointIndex++;
		if (myCurrentWaypointIndex == myWaypoints.size() - 1)
		{
			myGameObject->Destroy();
		}
		else
		{
			Vector3f dirToNext = (myWaypoints[myCurrentWaypointIndex + 1] - myWaypoints[myCurrentWaypointIndex]).GetNormalized();
		}
	}
}

void WalkingCustomer::SetWaypoints(const std::vector<Vector3f>& aWaypoints)
{
	myWaypoints = aWaypoints;
	for (size_t i = 1; i < myWaypoints.size() - 1; i++)
	{
		Vector3f dirBack = (myWaypoints[i - 1] - myWaypoints[i]).GetNormalized();
		myTurnBoundaries.push_back(myWaypoints[i] + dirBack * myTurnDist);
	}
	myTurnBoundaries.push_back(myWaypoints.back());

	myRigidBody->SetActorPosition(myWaypoints[0]);

	physx::PxTransform physXTransform = myCastedActor->getGlobalPose();
	Vector3f physXPosition = { physXTransform.p.x, physXTransform.p.y, physXTransform.p.z };
	Vector3f distance = (myWaypoints[1]) - physXPosition;
	float xzDistance = sqrtf(distance.x * distance.x + distance.z * distance.z);
	myRigidBody->SetActorRotation({ Catbox::Rad2Deg(-atan2f(distance.y, xzDistance)), Catbox::Rad2Deg(-atan2f(-distance.x, distance.z)), 0.0f });
}


bool WalkingCustomer::CrossedTurnBoundary()
{
	physx::PxTransform physXTransform = myCastedActor->getGlobalPose();
	Vector3f physXPosition = { physXTransform.p.x, physXTransform.p.y, physXTransform.p.z };
	Vector3f dirToNext = (myTurnBoundaries[myCurrentWaypointIndex] - physXPosition).GetNormalized();
	Vector3f dirBetweenWaypoints = (myTurnBoundaries[myCurrentWaypointIndex] - myWaypoints[myCurrentWaypointIndex]).GetNormalized();
	return (dirToNext.Dot(dirBetweenWaypoints) < 0);
}
