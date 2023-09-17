#pragma once
#include "../GameObjects/Component.h"
class RotateAroundParent : public Component
{
public:
	RotateAroundParent() = default;
	~RotateAroundParent() = default;

	void Awake() override;
	void Init() override;

	void Update() override;

	void RenderInProperties(std::vector<Component*>& aComponentList) override;
private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	float myXRotationRadians;
	float myYRotationRadians;

	float myXSpeed;
	float myYSpeed;

	bool myShouldLookAtTarget;

	Vector3f myOffset;
};

