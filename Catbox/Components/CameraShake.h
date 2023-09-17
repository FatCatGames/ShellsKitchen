#pragma once
#include "GameObjects/Component.h"

class CameraShake : public Component
{
public:

	CameraShake() = default;
	~CameraShake();
	void Awake() override;
	void Init() override;
	void Update() override;
	void Start();
	void Pause();
	void Stop();
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	bool GetCameraShakeActive() { return myCameraShakeActive; };
	float GetCameraShakeIntesity() { return myTempShakeIntensity; };
	void SetCameraShakeIntensity(float anIntensity) { myTempShakeIntensity = anIntensity; };

	float GetShakeDuration() { return myTempShakeDuration; };
	void SetShakeDuration(float aDuration) { myTempShakeDuration = aDuration; };

private:
	float myShakeTime;
	Camera::CameraSettingsTemplate myCameraOrigin;
	Camera* myTemporaryCamera = nullptr;

	bool myCameraShakeActive;
	float myCameraShakeIntensity;
	float myShakeDuration;
	float myTempShakeIntensity;
	float myTempShakeDuration;
	bool myPlayOnAwake = false;
};

