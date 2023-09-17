#include "stdafx.h"
#include "VideoPlayer.h"
#include "Audio/AudioManager.h"
#include "Audio\AudioMonoComponent.h"
#include "TimeManager.h"

VideoPlayer::~VideoPlayer()
{
	TimeManager::RemoveTimer(myAudioTimerId);
	TimeManager::RemoveTimer(myVideoTimerId);
}

void VideoPlayer::Init()
{
	myVideoAbortTimer = 0.1f;
	if (this->GetGameObject().GetComponent<UISprite>())
	{
		this->GetGameObject().GetComponent<UISprite>()->SetEnabled(false);
	}
}

void VideoPlayer::Awake()
{
	if (myVideoState == VideoState::NotStarted || myVideoState == VideoState::Stopped)
	{
		VideoTexture::createAPI();
		if (myVideoTexture.internal_data)
		{
			myVideoTexture.destroy();
		}
		if (!myVideoTexture.create(&myVideoName[0]))
		{
			myVideoName = "Undefined";
			return;
		}
		if (!this->GetGameObject().GetComponent<UISprite>())
		{
			this->GetGameObject().AddComponent<UISprite>();
			this->GetGameObject().GetComponent<UISprite>()->SetPixelShader(AssetRegistry::GetInstance()->GetAsset<PixelShader>("VideoPS"));

			this->GetTransform()->SetLocalScale(1920, 1080, 1);
		}
		else if (!this->GetGameObject().GetComponent<UISprite>()->IsEnabled())
		{
			this->GetGameObject().GetComponent<UISprite>()->SetEnabled(true);
		}
	}
	if (myVideoName != "Undefined")
	{
		Play();
	}
}

void VideoPlayer::Update()
{
	if (myVideoState != Delayed) 
	{
		myVideoAbortTimer -= realDeltaTime;
		if (Input::GetKeyPress(KeyCode::ESCAPE) || Input::GetController().IsPressed(CommonUtilities::Xbox::Start) || Input::GetController().IsPressed(CommonUtilities::Xbox::B))
		{
			if (myVideoAbortTimer < 0.0f && myCanSkip)
			{
				Engine::GetInstance()->GetAudioManager()->SetSFXVolume(myVolume);
				Engine::GetInstance()->GetAudioManager()->StopAllSound(true);
				Engine::GetInstance()->GetAudioManager()->StopMusic();

				if (myGameObject->GetComponent<AudioMonoComponent>())
				{
					myGameObject->GetComponent<AudioMonoComponent>()->StopSoundEffect(0);
				}

				myVideoState = VideoState::Stopped;
				this->GetGameObject().GetComponent<UISprite>()->SetEnabled(false);
				myEndEvent.Invoke();
			}
		}
		if (myVideoTexture.internal_data != nullptr)
		{
			if (myVideoState == VideoState::Playing)
			{
				if (myVideoTexture.hasFinished())
				{
					if (myShouldLoop == false)
					{
						myVideoState = VideoState::Stopped;
						Engine::GetInstance()->GetAudioManager()->SetSFXVolume(myVolume);
						this->GetGameObject().GetComponent<UISprite>()->SetEnabled(false);
						Engine::GetInstance()->GetAudioManager()->StopAllSound(false);
						myEndEvent.Invoke();
					}
					else
					{
						Play();
					}
				}
				else
				{
					if (this->GetGameObject().GetComponent<UISprite>())
					{
						if (mySound && !myHasStarted)
						{
							myHasStarted = true;
							if (myAudioDelay > 0)
							{
								TimerPayload payload;
								payload.myTime = myAudioDelay;
								payload.myShouldUpdate = false;
								payload.aPayLoadFuction = [this] { Engine::GetInstance()->GetAudioManager()->PlayMusic(mySound->myId); };

								myAudioTimerId = TimeManager::AddTimer(payload);
							}
							else
							{
								Engine::GetInstance()->GetAudioManager()->PlayMusic(mySound->myId);
							}
						}

						myVideoTexture.update(realDeltaTime);
						std::shared_ptr<Texture> frame = std::make_shared<Texture>(*myVideoTexture.getTexture());
						this->GetGameObject().GetComponent<UISprite>()->SetTexture(frame);
					}
				}
			}
		}
		else
		{
			printerror("No video set or video does not exist");
		}
	}
}

void VideoPlayer::RunInEditor()
{
	if (myEditorUpdate)
	{
		Update();
	}
}

void VideoPlayer::AddListener(Listener aListener)
{
	myEndEvent.AddListener(aListener);
}

void VideoPlayer::ResetVideo()
{
	myVideoTexture.ResetVideo();
}

void VideoPlayer::RenderInProperties(std::vector<Component*>& aComponentList)
{
	if (myVideoState != VideoState::Paused)
	{
		myPlayOrPause = "Play";
	}
	else
	{
		myPlayOrPause = "Resume";
	}

	if (ImGui::Button(&myPlayOrPause[0]))
	{
		myEditorUpdate = true;
		Play();
	}
	if (ImGui::Button("Pause") && myVideoState == VideoState::Playing)
	{
		Pause();
	}
	if (ImGui::Button("Stop") && (myVideoState == VideoState::Playing || myVideoState == VideoState::Paused))
	{
		Stop();
	}
	if (ImGui::Checkbox("Should Loop", &myShouldLoop))
	{
		if (myVideoState == VideoState::Playing || myVideoState == VideoState::Paused)
		{
			ShouldLoop(myShouldLoop);
		}
	}
	ImGui::Checkbox("Able to skip", &myCanSkip);
	if (myCanSkip == true)
	{
		ImGui::InputFloat("Unskippable Seconds", &myVideoAbortTimer);
	}
	ImGui::InputText("VideoName", &myVideoName[0], 20);

	auto& list = Engine::GetInstance()->GetAudioManager()->GetMonoList();
	std::vector<const char*> soundStrings;
	//std::vector<AudioManager::eSoundEffects> soundEnums;
	soundStrings.push_back("Nothing");
	//soundEnums.push_back(AudioManager::eSoundEffects::Nothing);

	int selected = 0;
	for (int i = 0; i < list.size(); i++)
	{
		//soundEnums.push_back(soundAsset.first);
		soundStrings.push_back(list[i].myShortName.c_str());
		if (mySound && mySound->myShortName == list[i].myShortName)
		{
			selected = i + 1;
		}
	}

	if (ImGui::Combo(("Sound effect " + std::to_string(0)).c_str(), &selected, soundStrings.data(), static_cast<int>(soundStrings.size())))
	{
		if (selected == 0)
		{
			mySound = nullptr;
		}
		else
		{
			mySound = &list[selected - 1];
		}
	}

	ImGui::InputFloat("Audio Delay", &myAudioDelay);
	ImGui::InputFloat("Video Delay", &myVideoDelay);
}

void VideoPlayer::Play()
{
	if (myVideoTexture.internal_data != nullptr)
	{
		myVolume = Engine::GetInstance()->GetAudioManager()->GetSFXVolume();
		Engine::GetInstance()->GetAudioManager()->SetSFXVolume(0);
		Engine::GetInstance()->GetAudioManager()->StopAllSound(true);
		Engine::GetInstance()->GetAudioManager()->StopMusic();

		ShouldLoop(myShouldLoop);
		myHasStarted = false;

		if (myVideoState != VideoState::Paused)
		{
			ResetVideo();
		}

		if (myVideoDelay > 0) 
		{
			myVideoState = Delayed;
			TimerPayload payload;
			payload.myTime = myVideoDelay;
			payload.myShouldUpdate = false;
			payload.aPayLoadFuction = [this] { 	myVideoTexture.resume(); myVideoState = VideoState::Playing; };

			myVideoTimerId = TimeManager::AddTimer(payload);
		}
		else 
		{
			myVideoTexture.resume();
			myVideoState = VideoState::Playing;
		}
	}
	else
	{
		Awake();
	}
}

void VideoPlayer::Pause()
{
	if (myVideoTexture.internal_data != nullptr)
	{
		myVideoState = VideoState::Paused;
		myVideoTexture.pause();
	}
}

void VideoPlayer::Stop()
{
	if (myVideoTexture.internal_data != nullptr)
	{
		myEndEvent.Invoke();
		myVideoState = VideoState::Stopped;
		myVideoTexture.destroy();
		Engine::GetInstance()->GetAudioManager()->SetSFXVolume(myVolume);
		Engine::GetInstance()->GetAudioManager()->StopMusic();
		this->GetGameObject().GetComponent<UISprite>()->SetEnabled(false);
	}
}

float VideoPlayer::GetVideoLength()
{
	return myVideoTexture.GetVideoLength();
}

void VideoPlayer::ShouldLoop(bool aFlag)
{
	myVideoTexture.ShouldLoop(aFlag);
}

void VideoPlayer::OpenVideo(std::string aPath)
{
	if (myVideoTexture.create(aPath.c_str()))
	{
		myVideoName = aPath;
	}
}

void VideoPlayer::SetAudio(std::string aPath)
{
	mySound = Engine::GetInstance()->GetAudioManager()->GetMonoSoundAssetByPath(aPath);
}

void VideoPlayer::Save(rapidjson::Value& /*aComponentData*/)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Bool>("Loop", myShouldLoop);
	wrapper.SaveValue<DataType::Bool>("Skip", myCanSkip);
	wrapper.SaveString("VideoName", myVideoName.c_str());
	wrapper.SaveValue<DataType::Float>("Abort", myVideoAbortTimer);
	wrapper.SaveValue<DataType::Float>("AudioDelay", myAudioDelay);
	wrapper.SaveValue<DataType::Float>("AudioDelay", myVideoDelay);
	if (mySound) 
	{
		wrapper.SaveString("AudioName", mySound->myEventPath.c_str());
	}
}

void VideoPlayer::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Loop"))
	{
		myShouldLoop = aComponentData["Loop"].GetBool();
	}
	if (aComponentData.HasMember("Skip"))
	{
		myCanSkip = aComponentData["Skip"].GetBool();
	}
	if (aComponentData.HasMember("VideoName"))
	{
		myVideoName = aComponentData["VideoName"].GetString();
	}
	if (aComponentData.HasMember("Abort"))
	{
		myVideoAbortTimer = aComponentData["Abort"].GetFloat();
	}
	if (aComponentData.HasMember("AudioDelay"))
	{
		myAudioDelay = aComponentData["AudioDelay"].GetFloat();
	}
	if (aComponentData.HasMember("VideoDelay"))
	{
		myVideoDelay = aComponentData["VideoDelay"].GetFloat();
	}
	if (aComponentData.HasMember("AudioName"))
	{
		mySound = Engine::GetInstance()->GetAudioManager()->GetMonoSoundAssetByPath(aComponentData["AudioName"].GetString());
	}
}