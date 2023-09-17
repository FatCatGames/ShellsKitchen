#pragma once
class CloudSpawner : public Component
{
	void Awake() override;
	void Init() override;
	void Update() override;
	void RunInEditor() override;
	void OnDestroy() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

private:

	void ResetClouds();
	void CloudUpdate();

	Transform* myTransform = nullptr;
	int myActiveClouds = 0;
	int myMaxAmountOfClouds = 0;
	int myCloudIndex = 0;
	bool myHasChangedSettings = false;
	float myTimeSinceLastSpawn = 0.f;
	float myCloudRespawnTimer = 1.f;
	float myRadius = 10.f;
	float myCloudMaxSize = 1.f;
	float myWindForce = 10.f;
	float myFadeoutTime = 0.f;

	std::vector<float> myCloudAliveTimes;
	std::vector<GameObject*> myClouds;
	std::unordered_map<GameObject*, float> mySpawnTimes;

};

