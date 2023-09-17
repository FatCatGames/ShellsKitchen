#pragma once
class Sharkfin : public Component
{

public:
	void Awake() override;
	void Update() override;

	void RenderInProperties(std::vector<Component*>& aComponentList) override;	
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
private:
	GameObject* myLevelSelectPlayer = nullptr;
	Transform* myRipples = nullptr;
	float myStartY = 0;
	float myDiveSpeed = 1.50f;
	float myDiveDistance = 10.f;
	float mySplashTimer = 0.f;
public:
	float myDiveDepth = 3.f;

};