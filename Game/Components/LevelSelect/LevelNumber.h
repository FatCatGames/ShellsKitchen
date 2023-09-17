#pragma once

class LevelNumber : public Component
{
public:
	void Awake() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
private:
	int myLevelNumber = 0;
};