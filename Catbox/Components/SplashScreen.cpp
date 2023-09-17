#include "stdafx.h"
#include "SplashScreen.h"
#include "Components/UI/VideoPlayer.h"
#include "ComponentTools/SceneManager.h"
#include "Assets/AssetLoader.h"
#include "ComponentTools/UIEventHandler.h"
#include "Audio\AudioManager.h"
#include "Audio\AudioMonoComponent.h"

void SplashScreen::Init()
{
}

void SplashScreen::Awake()
{
	myHasAwoken = true;
}

void SplashScreen::Update()
{
	if(myHasAwoken == true)
	{
		auto videoPlayer = myGameObject->GetComponent<VideoPlayer>();
		if(videoPlayer)
		{
			VideoPlayer::VideoState state = videoPlayer->GetVideoState();
			if(state == VideoPlayer::VideoState::Stopped)
			{
				if(myHasStartedAudio == false)
				{
					myHasStartedAudio = true;
					if(myGameObject->GetComponent<AudioMonoComponent>())
					{
						myGameObject->GetComponent<AudioMonoComponent>()->PlaySoundEffect(0);
					}
				}
			}
		}
	}

	if (myGameObject->GetComponent<VideoPlayer>()->GetVideoState() == VideoPlayer::VideoState::Stopped && Input::GetKeyPress(KeyCode::SPACE) && !mySplashScreenOver
		|| myGameObject->GetComponent<VideoPlayer>()->GetVideoState() == VideoPlayer::VideoState::Stopped && Input::GetController().IsPressed(CommonUtilities::Xbox::Button::A) && !mySplashScreenOver)
	{
		mySplashScreenOver = true;
		if(myGameObject->GetComponent<AudioMonoComponent>())
		{
			myGameObject->GetComponent<AudioMonoComponent>()->StopSoundEffect(0);
		}
		auto videoPlayer = myGameObject->GetComponent<VideoPlayer>();
		videoPlayer->OpenVideo("Cutscene1.mp4");
		videoPlayer->SetAudio("event:/Music/Cutscene1");
		videoPlayer->SetAudioDelay(2.45f);
		// cutscene 2 videoPlayer->SetAudioDelay(1.5f);
		// cutscene 3 videoPlayer->SetAudioDelay(0.0f);
		// cutscene 3 videoPlayer->SetVideoDelay(0.2f);
		// cutscene 4 videoPlayer->SetAudioDelay(0);
		// cutscene 4 videoPlayer->SetVideoDelay(0.2f);
		videoPlayer->SetCanSkip(true);
		videoPlayer->Awake();
	}
	else if (myGameObject->GetComponent<VideoPlayer>()->GetVideoState() == VideoPlayer::VideoState::Stopped && AssetLoader::GetIsThreadActive() == false && mySplashScreenOver) 
	{
		UIEventHandler::SetNextLevel(mySceneToLoad);
	}
}

void SplashScreen::RenderInProperties(std::vector<Component*>& aComponentList)
{
	ImGui::InputText("Scene To Load", &mySceneToLoad);
}

void SplashScreen::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveString("SceneToLoad", mySceneToLoad.c_str());
}

void SplashScreen::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("SceneToLoad")) 
	{
		mySceneToLoad = aComponentData["SceneToLoad"].GetString();
	}
}
