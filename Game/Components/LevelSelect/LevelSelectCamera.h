#pragma once

class LevelSelectCamera : public Component
{
public:
	void Awake() override;
	void Update() override;

private:
	const Vector3f myOffset = { 0, 34.0f, -22.5f };
	const Vector3f myRotation = { 56.0f,0,0 };
	const float myMoveSpeed = 2500.0f;
	const float myRotSpeed = 100.0f;

};