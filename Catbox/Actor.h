#pragma once
#include "physX/PxRigidActor.h"
class RigidActor
{
public:
	RigidActor();
	~RigidActor();
	RigidActor(physx::PxRigidActor* aActor) { myActor = aActor; }
private:
	physx::PxRigidActor* myActor;
};

