#pragma once

class BurnIndicator : public Component
{
public:
	void Awake() override;
	void Update() override;
	void SetOffset(Vector3f anOffset);
	void Start();
	void Switch();
	void Stop();

private:
	Camera* myCamera = nullptr;
	RayPickerComponent* myRayPickerComponent = nullptr;
	Transform* myBurnSprite = nullptr;
	Vector3f myOffset = {0.0f, 0.015f, 0.0f};

	bool myIsActive = false;
	bool myIndicatorIsVisible = false;
};