#pragma once
#include "Components/Physics/RigidBody.h"

class TrashcanIcon : public Component
{
public:
	virtual void OnTriggerEnter(Collider* aCollider);
	virtual void OnTriggerExit(Collider* aCollider);

private:

};