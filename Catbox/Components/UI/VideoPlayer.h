#pragma once
#include "stdafx.h"
#include "Components/UI/UISprite.h"
#include "Assets/video_texture.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Material.h"
#include "Audio/AudioManager.h"

class VideoPlayer : public Component
{
public:
	enum VideoState
	{
		Playing = 0,
		Stopped,
		Paused,
		NotStarted,
		Delayed,
		COUNT
	};
	~VideoPlayer();

	void Init() override;
	void Awake() override;

	void Update() override;
	void RunInEditor() override;

	void AddListener(Listener aListener);

	void ResetVideo();

	void RenderInProperties(std::vector<Component*>& aComponentList) override;

	void Play();
	void Pause();
	void Stop();
	float GetVideoLength();

	void SetCanSkip(bool aFlag, float aSkipTimer = 0.0f) { myCanSkip = true; myVideoAbortTimer = aSkipTimer; }

	VideoState GetVideoState() const { return myVideoState; };

	void ShouldLoop(bool aFlag);

	void OpenVideo(std::string aPath);
	void SetAudio(std::string aPath);

	void SetAudioDelay(float aAudioDelay) { myAudioDelay = aAudioDelay; };

	void SetVideoDelay(float aVideoDelay) { myVideoDelay = aVideoDelay; };

	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

private:
	VideoState myVideoState = VideoState::NotStarted;
	VideoTexture myVideoTexture;
	std::shared_ptr<PixelShader> myPixelShader;
	bool myEditorUpdate = false;
	bool myShouldLoop = false;
	bool myCanSkip = true;
	std::string myPlayOrPause;
	std::string myVideoName = "Undefined";
	int myVideoNumber = 0;
	float myVideoAbortTimer;
	float myAudioDelay = 0.0f;
	float myVideoDelay = 0.0f;
	float myVolume = 0.0f;
	bool hasPlayedAudio = false;
	UINT myAudioTimerId = 0;
	UINT myVideoTimerId = 0;
	const AudioManager::SoundAsset* mySound = nullptr;
	Event myEndEvent;
};