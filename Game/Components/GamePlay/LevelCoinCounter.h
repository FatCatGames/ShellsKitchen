#pragma once

class LevelCoinCounter : public Component
{
public:
	void Awake() override;
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

private:
	int myCurrentScore = 0;

	int myBronzeScore = 10;
	int mySilverScore = 100;
	int myGoldScore = 200;

};