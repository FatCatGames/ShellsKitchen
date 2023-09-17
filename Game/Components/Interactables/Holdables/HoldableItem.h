#pragma once

class HoldableItem : public Interactable
{
public:
	~HoldableItem();
	void Interact(Player* aPlayer) override;
	virtual bool GetIsThrowable() { return false; }
	virtual void OnPickup() {};
	void Awake() override;
	virtual void SetFoodTrailState(bool aShouldPlay) {}

	void Update() override;
	void FixedUpdate() override;
	void OnCollisionEnter(Collider* aCollider);
	void SetPlayerForward(const Vector3f& aForward);

	const float& GetMass() const { return myMass; }

	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	void OnTriggerEnter(Collider* aCollider) override;
	void OnTriggerExit(Collider* aCollider) override;

	void ResetCatch();
	void ResetTriggerStillActive();
	void Reset();
	void ResetAll();

	void ResetTeleportTimer() { myTeleportTimer = 1.f; }
	bool GetIsBeingTeleported() { return myTeleportTimer > 0; }

	void SetmyCanTriggerCollide(const bool& aBool);


private:
	//Throwing Variabels
	bool myInteractableCollision = false;
	bool myTriggerCollision = false;

	float myPushDownStartValue = 0;
	float myAirResistenceStartValue = 0;

	float myAirResistence = 20.0f;
	float myAirResistenceAdditionSpeed = 40.f;
	float myPushDown = 12.5f;
	float myPushDownReductionSpeed = 60.f;

	float myThrowingTimer = 0;
	float myTimeUntilStopBeingThrown = 5;

	float myMass = 1.0f;

	bool myTriggerStillActiveAfterCollision = false;
	float myTriggerStillActiveTime = 0.5f;
	float myTriggerStillActiveTimer = 0.f;
	//std::vector<Interactable*> myInteractablesInRange;
	Interactable* mySelectedInteractable;
	//Player
	Vector3f myPlayerThrowingForward = { 0,0,0 };

	float myNoCollisionTime = 0.04f;
	float myCollisionTimer = 0.f;

	bool myCanTriggerCollide = true;
	bool myIsSinking = false;
	bool myHasHitAnimal = false;
	float myTeleportTimer = 0;

};