#include "stdafx.h"
#include "Actor.h"

RigidActor::RigidActor()
{
	//myActor = physx::PxCreateDynamic()
}

RigidActor::~RigidActor()
{
	myActor->release();
}
