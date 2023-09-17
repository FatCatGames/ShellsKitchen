#pragma once
#include "Components\Physics\RigidBody.h"
#include "..\Catbox\Audio\AudioMonoComponent.h"

class LevelSelectPlayer : public Component
{
public:
	~LevelSelectPlayer();
	void Awake() override;
	void FixedUpdate() override;
	void HandleMovement();

private:
	void HandleWaveLines();
	CommonUtilities::XboxControllerInput* myFirstController = nullptr;
	RigidBody* myRigidBody = nullptr;
	Transform* myWaveLines = nullptr;
	AudioMonoComponent* myAudioComponent = nullptr;

	const float myMoveSpeed = 900.0f;
	const float myRotSpeed = 250.0f;
	float myWaveShaderSpeed = 0.f;
	float mySpawnRippleTimer = 0.f;
	bool myGivingLeftStickInput = false;

};