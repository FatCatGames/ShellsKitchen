#pragma once


class CountdownComponent : public Component
{
public:
	CountdownComponent();
	~CountdownComponent();
	void Awake() override;
	void Update() override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void StartCountdown();
	void ResetCountdown();
	
private:
	float myTotalTimer = 3.0f;
	float myCurrentTime = 3.0f;
	float myTickTime = 0.0f;
	float myLifeTime = 0.0f;

	float myDelayedStart = 0.25f;

	bool myShouldDoIntroCountdown = false;
	bool myShouldCountdown = false;
};