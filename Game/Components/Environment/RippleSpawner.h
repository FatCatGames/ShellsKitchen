#pragma once
class RippleSpawner : public Component
{
	void Awake() override;
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void OnCollisionEnter(Collider* aCollider) override;
	void OnDestroy() override;
	void RunInEditor() override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

private:
	void OnEdited();

	RigidBody* myRigidBody = nullptr;
	float mySpawnRippleTimer = 0.f;
	bool myHasChangedSettings = true;
	bool myIsBobbing = false;
	float myRespawnTimer = 0.2f;
	Vector3f myPrevPos = { 0.f,0.f,0.f };
	std::string myPrefabName = "BoatRipplesVFX";
	GameObject* myFunnyFin = nullptr;

	float myPreviousSpin = 0.2f;
	int myCurrentRipple = 0;
	std::vector<GameObject*> myRipples;

};

