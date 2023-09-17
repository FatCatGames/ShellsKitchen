#pragma once
#include "Audio\AudioMonoComponent.h"

class LevelSelectAudioPlayer : public Component
{
public:
	void Awake() override;
	void Update() override;

private:
	AudioMonoComponent* myAudioMonoComponent = nullptr;

	bool myStartedPlaying = false;
};