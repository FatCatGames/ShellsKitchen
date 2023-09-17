#pragma once
#include "Camera.h"

class CinematicCamera : public Camera
{
public:
	CinematicCamera();
	~CinematicCamera();


	inline int GetIsInstant() { return myIsInstant; }
	inline int GetNextCamera() { return myNextCamera; }
	inline int GetLookAt() { return myLookAtCamera; }
	inline void SetNextCamera(int aCameraId) { myNextCamera = aCameraId; }
	inline void SetLookAtCamera(int aCameraId) { myLookAtCamera = aCameraId; }
	inline float GetDelay() { return myDelay; }
	inline float GetSpeed() { return mySpeed; }
	inline float GetUseCurve() { return myUseCurve; }

	void RunInEditor() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;

private:

	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	int myNextCamera = -1;
	int myLookAtCamera = -1;
	bool myIsInstant = true;
	bool myUseCurve = false;
	float myDelay = 0.0f;
	float mySpeed = 1.0f;
};