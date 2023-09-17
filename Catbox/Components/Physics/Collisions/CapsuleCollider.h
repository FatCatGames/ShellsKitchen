#pragma once
#include "Components/Physics/Collisions/Collider.h"
#include "Physics/PhysicsEngine.h"
class CapsuleCollider : public Collider
{
public:
	void Awake() override;
	bool IsInside(const Vector3f& aPoint) const override;
	void DebugDraw() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void SetActorPosition(Vector3f aPosition);
	void SetActorRotation(Vector3f aRotation);
	float GetRadius() const { return myRadius; }
	float GetHeight() const { return myHeight; }
private:
	void Update() override;
	void RunInEditor() override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	physx::PxRigidStatic* myStatic = nullptr;
	float myRadius = 1.0f;
	float myHeight = 1.0f;
	bool myShouldWaitForRigidbody = false;
};

