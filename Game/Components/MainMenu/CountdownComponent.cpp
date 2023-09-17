#include "Game.pch.h"
#include "CountdownComponent.h"

CountdownComponent::CountdownComponent()
{

}

CountdownComponent::~CountdownComponent()
{
}

void CountdownComponent::Awake()
{
	UIManager::GetInstance()->AddCountdownComponent(this);
	myGameObject->GetComponent<UISprite>()->SetAlpha(0.0f);
	myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Empty"));
}

void CountdownComponent::Update()
{
	if(myShouldDoIntroCountdown == true)
	{
		myLifeTime += realDeltaTime;


		if(myLifeTime > 0.1f)
		{
			Engine::GetInstance()->SetGamePaused(true);
		}
		if(myLifeTime > myDelayedStart)
		{
			myShouldCountdown = true;
			myShouldDoIntroCountdown = false;
			Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_READY_SET_GO));
		}
	}

	if (myShouldCountdown == true)
	{
		myCurrentTime -= realDeltaTime;
		int imageToShow = myCurrentTime;
		switch (imageToShow)
		{
		case 0:
			myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("1"));
			break;
		case 1:
			myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("2"));
			break;
		case 2:
			myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("3"));
			break;
		default:
			myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Empty"));
			break;
		}
	}
	if (myCurrentTime < 0.0f)
	{	
		ResetCountdown();

		Engine::GetInstance()->SetGamePaused(false);
		GameManager::GetInstance()->StartAudioPlayer();
	}
}

void CountdownComponent::StartCountdown()
{
	GameManager::GetInstance()->SetIsInCountdown(true);
	myShouldDoIntroCountdown = true;
	myTickTime = 0;
	myGameObject->GetComponent<UISprite>()->SetAlpha(1.0f);
	Engine::GetInstance()->SetGamePaused(false);
}

void CountdownComponent::ResetCountdown()
{
	GameManager::GetInstance()->SetIsInCountdown(false);
	myCurrentTime = myTotalTimer;
	myShouldCountdown = false;
	myShouldDoIntroCountdown = false;
	myGameObject->GetComponent<UISprite>()->SetAlpha(0.0f);
	myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Empty"));
}

void CountdownComponent::RenderInProperties(std::vector<Component*>& aComponentList)
{
	Catbox::InputFloat("Start Delay", &myDelayedStart);
}

void CountdownComponent::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Float>("Start Delay", myDelayedStart);
}

void CountdownComponent::Load(rapidjson::Value& aComponentData)
{
	if(aComponentData.HasMember("Start Delay"))
	{
		myDelayedStart = aComponentData["Start Delay"].GetFloat();
	}
}