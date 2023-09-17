#pragma once
class Customer : public Component
{
public:
	void Awake() override;
	void Update() override;
	void GenerateAppearance();
	void OnCollisionEnter(Collider* aCollider) override;

protected:
	bool myReachedTarget = true;
	float myStunTimer = 0;
	int myType = 0;
	Animator* myAnimator;

	RigidBody* myRigidBody;
	physx::PxRigidDynamic* myCastedActor;
};