#pragma once
#include "stdafx.h"
#include "physX/PxRigidActor.h"
#include "physX/PxForceMode.h"

enum Shape;

class RigidBody: public Component
{
public:
	struct RigidBodyData 
	{
		float mass = 5;
		float gravity = 9.81f;
		float drag = 50.0f;
		bool useGravity = true;
		bool isStatic = true;
	};

	enum class Axis
	{
		x,
		y,
		z
	};

	RigidBody() = default;
	~RigidBody();
	RigidBody(physx::PxRigidActor* aActor);

	void Awake() override;
	void RenderInProperties(std::vector<Component*>& aComponentList);
	//void SetRigidShape(Shape aShape);

	void Update() override;
	void FixedUpdate() override;
	void RunInEditor() override;
	void SetDamping(float aValue);
	RigidBodyData& GetData();
	Vector3f& GetVelocity();
	void Rotate(const Vector3f& angles);
	void SetActorPosition(const Vector3f& aPos);
	Vector3f GetActorPosition();
	void SetActorRotation(const Vector3f& aRot);
	void SetVelocity(Vector3f aDirection, float aForce);
	void SetVelocity(Vector3f aVelocity);
	void SetAngularVelocity(Vector3f aDirection, float aForce);
	void SetMaximumLinearVelocity(float aMaxVelocity);
	void SetMaximumAngularVelocity(float aMaxVelocity);
	Vector3f AddForce(Vector3f aDirection, float aForce, physx::PxForceMode::Enum aMode = physx::PxForceMode::eFORCE);
	Vector3f AddForceAtPos(const float aForce, const Vector3f aPosition, physx::PxForceMode::Enum aMode = physx::PxForceMode::eFORCE);
	void ScheduleForce(const Vector3f aPosition, physx::PxForceMode::Enum aMode = physx::PxForceMode::eFORCE);
	void ChangeGravityScale(bool aGravityScale);
	void ChangeMass(float aMass);
	void SetTranslationAxisLock(bool x, bool y, bool z, bool shouldWakeUp = true);
	void SetRotationAxisLock(bool x, bool y, bool z, bool shouldWakeUp = true);
	bool GetTranslationAxisLock(Axis anAxis);
	bool GetRotationAxisLock(Axis anAxis);
	void WakeUp();
	void PutToSleep();
	void SetLayer(const int aLayerID, const int aShapeID = 0);
	physx::PxRigidActor* GetActor() { return myActor; };

	void SetKinematic();

	void OnDisable() override;
	void OnEnable() override;
	void DisableSimulation();
	void EnableSimulation();

private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	physx::PxRigidActor* myActor;
	RigidBodyData myRbData;
	Vector3f myVelocity;
	Vector3f myScheduledForce;
	Vector3f myNewPos;
	Vector3f myNewRot;
	Vector3f myCenterOfMass = { 0,0,0 };
	Vector3f myStartLerpPos = { 0,0,0 };
	physx::PxForceMode::Enum myScheduledForceMode;

	bool myShouldLerp = false;
	bool myDebugMode = false;
	float myMass = 1;
	float myDampening = 0.0f;
	bool myGravity = true;
	bool myLockTranslations[3] = { false, false, false };
	bool myLockRotations[3] = { false, false, false };
	bool myShouldSleep = false;
	bool myHasRunAwake = false;
	Vector3f myPreviousVelocity;
};