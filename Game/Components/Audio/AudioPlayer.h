#pragma once
#include "Audio\AudioMonoComponent.h"

class AudioPlayer : public Component
{
public:
	void Awake() override;
	void Update() override;
	void StartAudio();
	void PitchMusic(const float& aTargetPitch);

private:
	AudioMonoComponent* myAudioMonoComponent = nullptr;

	bool myStartedPlaying = false;
	bool myShouldStartPlaying = false;

	bool myIsMusicPitching = false;
	float myCurrentMusicPitch = 0.0f;
	float myTargetMusicPitch = 0.0f;
	float myPitchTimer = 0.0f;
};