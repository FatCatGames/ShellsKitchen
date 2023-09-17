#pragma once
class DistanceBasedZoomCamera : public Component
{
private:
	void Awake() override;
	void Update() override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	void RenderInProperties(std::vector<Component*>& someComponents) override;

	std::vector<Transform*> myTargets;
	
	float myMovementSpeed = 10.0f;
	float myZoomSpeed = 1.0f;
	float myStartingZoom = -1;
	float myMinFov = 70.0f;
	float myMaxFov = 90.0f;
	Vector2f myOffsetMax = { 0,0 };
	Vector2f myOffsetMin = { 0,0 };
	float myStartMinDistance = 5.0f;
	float myMinDistance = 2.0f;
	float myStartMaxDistance = 20.0f;
	float myMaxDistance = 22.0f;
	float myTargetFov = 0.0f;
	Camera* myCamera;
	GameManager* myManagerRefrence = nullptr;
	Vector3f myTargetsStartMiddlePoint = Vector3f::zero();
	Vector3f myStartPosition = Vector3f::zero();
	Vector3f myTargetLookPosition = Vector3f::zero();

	std::array<Vector3f, 2> myFurthestTargets;

};

