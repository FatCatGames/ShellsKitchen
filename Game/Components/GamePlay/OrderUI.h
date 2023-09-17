#pragma once

class OrderUI : public Component
{
public:
	void Update() override;
	void SetDuration(const float& aDuration) { myOrderDuration = aDuration; }

	void SpawnScaleLerp();
	void MovingToStartLocation();
	void StartMovingToStart();
	void SetTargetPos(const Vector3f& aStartLocation);
	void SetOrderIndex(const int& aIndex);

	int GetOrderIndex() { return myOrderIndex; }

private:
	Vector3f myStartPos = { 0.0f, 0.0f, 0.0f };
	float myOrderDuration = 120.0f; // These two are set again in Update once, so check there. 
	float myOrderTime = 120.0f;

	int myOrderIndex = 0;

	float myLerpingTime = 0.0f;
	bool myIsLerping = false;
	bool myIsLerpingUp = false;
	bool myIsLerpingDown = false;

	bool myIsMovingToStartPos = true;
	bool myHasSetup = false;
	float testtimer = 2.0f;
	bool mySecondUpdate = false;
};