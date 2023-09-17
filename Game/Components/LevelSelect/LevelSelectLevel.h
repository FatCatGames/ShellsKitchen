#pragma once

class LevelSelectLevel : public Component
{
public:
	void Awake() override;
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void OnTriggerEnter(Collider* aCollider) override;
	void OnTriggerExit(Collider* aCollider) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	void SetFlagStarMaterial();

private:
	string myLevelName = "NoLevel";
	int myLevelNumber = 0;
	bool myCanStartLevel = false;

};