#include "Game.pch.h"
#include "LevelCounterComponent.h"
#include "Managers\GameManager.h"
#include "Managers\LevelSelectManager.h"
#include "Components\Scoreboard\Scoreboard.h"
#include "Shaders\UI\Timer\UITimerPS.h"
#define MAX_VALUE_TO_SHOW 9999

void LevelCounterComponent::Awake()
{
	int levelNumber = LevelSelectManager::GetInstance()->GetCurrentLevel();
	if (levelNumber == 0) levelNumber = 1;
	const float levelDuration = LevelSelectManager::GetInstance()->GetLevelDuration(levelNumber - 1);
	myCurrentTime = levelDuration;
	myValueToShow = int(levelDuration);
	SetValue(levelDuration);
}

void LevelCounterComponent::Update()
{
	if(true)
	{
		if(true)
		{
			if(myValueToShow <= MAX_VALUE_TO_SHOW)
			{
				auto timeLeftShader = dynamic_cast<UITimerPS*>(myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
				if(timeLeftShader)
				{
					timeLeftShader->myUITimerData.Resource = myCurrentTime + 0.5f;
					timeLeftShader->SetData();
				}

				myShouldTickTime = false;
			}
		}

		myCurrentTime -= deltaTime;
		myTickTimer += deltaTime;

		if(myCurrentTime < 31.5f && myHasPlayedCountdownAlert == false)
		{
			myHasPlayedCountdownAlert = true;
			Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_COUNTDOWN_ALERT));
		}

		if(myCurrentTime < 11.5f && myHasPlayedCountdownAlertSecondTime == false)
		{
			myHasPlayedCountdownAlertSecondTime = true;
			Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_COUNTDOWN_ALERT));
		}

		if(myCurrentTime < 30.5f && myHasDoneFirstMusicPitch == false)
		{
			myHasDoneFirstMusicPitch = true;
			myThirtySecondsEvent.Invoke();
			GameManager::GetInstance()->GetAudioPlayer()->PitchMusic(0.35f);
		}

		if(myCurrentTime < 10.5f && myHasDoneSecondMusicPitch == false)
		{
			myHasDoneSecondMusicPitch = true;
			myTenSecondsEvent.Invoke();
			GameManager::GetInstance()->GetAudioPlayer()->PitchMusic(0.55f);
		}
		
		if(myCurrentTime < 10.5f && myCurrentTime > 0)
		{
			myShouldTickTime = true;
			if(myTickTimer > 1)
			{
				myTickTimer = 0;
				Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_LEVEL_COUNTDOWN));
			}
		}


		if(myCurrentTime < 0 && myShouldCount == true)
		{
			Engine::GetInstance()->GetAudioManager()->StopAllSound();
			Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_LEVEL_COMPLETE));

			myCurrentTime = 0;
			myShouldCount = false;
			int deliveredScore = GameManager::GetInstance()->GetDeliveredScore();
			int tips = GameManager::GetInstance()->GetTips();
			int failedScore = GameManager::GetInstance()->GetFailedScore();
			int totalScore = deliveredScore + tips - failedScore;

			int levelNumber = LevelSelectManager::GetInstance()->GetCurrentLevel()-1;
			int bronze = LevelSelectManager::GetInstance()->GetBronzeScore(levelNumber);
			int silver = LevelSelectManager::GetInstance()->GetSilverScore(levelNumber);
			int gold = LevelSelectManager::GetInstance()->GetGoldScore(levelNumber);

			GameObject* scoreBoard = InstantiatePrefab("Scoreboard");
			scoreBoard->GetComponent<Scoreboard>()->SetValues(deliveredScore, tips, failedScore, bronze, silver, gold);

			GameManager::GetInstance()->GenerateOrders(false);
			GameManager::GetInstance()->SetLevelComplete(true);
			GameManager::GetInstance()->StopLevelMusic();

			bool gotBronze = (totalScore >= bronze) ? true : false;
			bool gotSilver = (totalScore >= silver) ? true : false;
			bool gotGold = (totalScore >= gold) ? true : false;

			LevelSelectManager::GetInstance()->SetLevelAwards(levelNumber, gotBronze, gotSilver, gotGold, totalScore);
		}

		myValueToShow = int(myCurrentTime);
		SetValue(myCurrentTime);
	}
}

void LevelCounterComponent::AddValue(const int& aValue)
{
	SetValue(myValueToShow + aValue);
}

void LevelCounterComponent::SetValue(const int& aValue)
{
	myValueToShow = aValue;
	const int minutes = myValueToShow / 60;
	const int remaining = myValueToShow - minutes * 60;

	units = remaining % 10;
	tens = remaining / 10;

	thousands = minutes;
}