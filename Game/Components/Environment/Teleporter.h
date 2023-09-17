#pragma once
#include "Components/Physics/RigidBody.h"

//Copy of player bullet for now
class Teleporter : public Component
{
public:
	void Awake() override;
	void FixedUpdate() override;
	void Update() override;
	virtual void OnTriggerEnter(Collider* aCollider);
	virtual void OnTriggerExit(Collider* aCollider);
	virtual void RenderInProperties(std::vector<Component*>& aComponentList);
	virtual void OnObjectFinishedLoading();
	void setOtherTelporterCoolDown();

private:
	virtual void Save(rapidjson::Value& aComponentData);
	virtual void Load(rapidjson::Value& aComponentData);
	bool myHasSavedPos = false;
	bool myHasLoadedPos = false;
	void StartTeleporting(GameObject* anObject);
	void FinishTeleporting();
	void StartCooldown();

	float timer = 0;
	float myWaitTime = 0.0f;
	float myWaitTimeBase = 0.0f;
	float myGoOutWaitTime = 0.3f;
	float myDistanceToPlayers = 0.f;
	float myVortexSlider = 1.f;

	Transform* teleportPos;     //teleporter pointer
	GameObject* object; //obj pointer
	Transform* myOtherTeleporter;

	std::vector<GameObject*> myObjectsInRange;

	bool goOutCoolDown = false;
	bool coolDown = false;
	bool teleport = false;

	float myPosAddition = 0.5f;

	float myPlayerForce = 1.f;
	float myThrowingForce = 1.f;

	float myAnimTime = 0.f;
	float myAnimTimeStandard = 0.4f;
	float myAnimTimeDash = 0.29f;
	bool myDoAnimeOnce = true;

};