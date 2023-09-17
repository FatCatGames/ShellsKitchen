#pragma once

class BobSpinComponent : public Component
{
public:
	BobSpinComponent();
	~BobSpinComponent();
	void Awake() override;
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList);
	void RunInEditor() override;
	void SetStartPosition(const Vector3f& position);
	void SetShouldBob(bool aShouldBob);

private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	bool myShouldBob = false;
	bool myShouldSpin = false;
	//bool myIsAffectedByCharacterControllers = false;
	float myBobSpeeds[3] = { 0, 0, 0 };
	float myBobHeights[3] = { 0, 0, 0 };
	float mySpinSpeeds[3] = { 0, 0, 0 };
	float myTotalTime = 0;
	Vector3f myOriginalPosition;
};