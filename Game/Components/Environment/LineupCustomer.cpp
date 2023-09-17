#include "Game.pch.h"
#include "LineupCustomer.h"
#include "Physics/PhysicsEngine.h"
#include "Assets\AnimationController.h"
#include "Components\Animator.h"
#include "Components\3D\ModelInstance.h"
#include "Assets\Model.h"

void LineupCustomer::Update()
{
	Customer::Update();

	if (myShouldRandomizeFrame)
	{
		myShouldRandomizeFrame = false;
		myAnimator->RandomizeFramze();
	}
}

void LineupCustomer::FixedUpdate()
{
	if (!myIsSetup)
	{
		myIsSetup = true;
		myRigidBody->SetActorPosition(myInitialPosition);
		myRigidBody->SetActorRotation(myInitialRotation);
		return;
	}

	if(myShouldWalkAway && myHasReachedDesk)
	{
		myShouldWalkAway = false;
		StartWalkingAway();
	}

	if (myReachedTarget) return;
	if (myStunTimer > 0) return;

	physx::PxTransform physXTransform = myCastedActor->getGlobalPose();
	Catbox::Quaternion physXRotation = Catbox::Quaternion({ physXTransform.q.x, physXTransform.q.y, physXTransform.q.z, physXTransform.q.w });
	Vector3f physXVectorRotation = Catbox::Quaternion::ToEulerAngles(physXRotation);
	Vector3f physXPosition = { physXTransform.p.x, physXTransform.p.y, physXTransform.p.z };
	Vector3f dirToNext = (myCurrentTarget - physXPosition).GetNormalized();
	float targetRotation = (std::atan2(dirToNext.x, dirToNext.z) * 57.3f);

	if ((myCurrentTarget - physXPosition).LengthSqr() < 0.3f)
	{
		myAnimator->SetBool("Walking", false);
		myHasReachedDesk = true;
		myShouldRandomizeFrame = true;
		myReachedTarget = true;
		if (myIsWalkingAway)
		{
			if (myCurrentTarget == myLeaveTarget) myGameObject->Destroy();
			else SetNextWaypoint(myLeaveTarget);
		}
	}

	
	if (myIsWalkingAway)
	{
		float currentRotation = std::fmod(physXVectorRotation.y, 360.f);

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
	}
	else
	{
		myRigidBody->SetActorRotation({ 0, targetRotation, 0 });
	}
	physXTransform = myCastedActor->getGlobalPose();

	Vector3f newVectorTarget = physXPosition + (myTransform->forward() * myWalkSpeed * fixedDeltaTime);
	physXTransform.p = { newVectorTarget.x, newVectorTarget.y, newVectorTarget.z };
	myCastedActor->setKinematicTarget(physXTransform);
}


void LineupCustomer::SetNextWaypoint(const Vector3f& aWaypoint)
{
	myCurrentTarget = aWaypoint;
	myReachedTarget = false;
	myAnimator->SetBool("Walking", true);
}

void LineupCustomer::SetInitialPosition(const Vector3f& aPosition, const Vector3f& aRotation)
{
	myInitialPosition = aPosition;
	myInitialRotation = aRotation;
}

void LineupCustomer::WalkAway(Vector3f aHorizontalOffsetVector)
{
	myShouldWalkAway = true;
	//myIsWalkingAway = true;
	myTurnTarget = (myCurrentTarget + aHorizontalOffsetVector * 2.f + (myInitialPosition - myCurrentTarget).GetNormalized() * 1.f);
	myLeaveTarget = myInitialPosition + aHorizontalOffsetVector * 2.f;
}

void LineupCustomer::StartWalkingAway()
{
	myIsWalkingAway = true;
	SetNextWaypoint(myTurnTarget);
}

