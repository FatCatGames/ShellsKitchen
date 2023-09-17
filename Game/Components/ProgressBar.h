#pragma once

//class ProgressBarPS;
class ProgressBar : public Component
{
public:
	void Awake() override;
	void Update() override;
	void OnEnable() override;
	void SetProgress(float aPercent);

private:
	Camera* myCamera = nullptr;
	RayPickerComponent* myRayPickerComponent = nullptr;

	Transform* myFirstChild = nullptr;
	Transform* mySecondChild = nullptr;

	Vector3f myParentPos;
	Vector3f myOffset = { 0.0f, 0.1f, 0.0f };

	//ProgressBarPS* myProgressBarPS = nullptr;
	//Material* myMaterial = nullptr;
	float myProgress = 0.0f;
};