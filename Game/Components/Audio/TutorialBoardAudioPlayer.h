#pragma once
#include "Audio\AudioMonoComponent.h"

class TutorialBoardAudioPlayer : public Component
{
public:
	void Awake() override;
	void Update() override;

private:
	AudioMonoComponent* myAudioMonoComponent = nullptr;

	bool myStartedPlaying = false;
};