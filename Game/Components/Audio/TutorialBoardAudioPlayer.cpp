#include "Game.pch.h"
#include "TutorialBoardAudioPlayer.h"
#include "Managers\GameManager.h"

void TutorialBoardAudioPlayer::Awake()
{
	myAudioMonoComponent = myGameObject->GetComponent<AudioMonoComponent>();
	GameManager::GetInstance()->SetTutorialAudioPlayer(this);
}

void TutorialBoardAudioPlayer::Update()
{
	if(myStartedPlaying == false)
	{
		myStartedPlaying = true;
		if(myAudioMonoComponent)
		{
			for(int index = 0; index < myAudioMonoComponent->GetNumberOfSounds(); index++)
			{
				myAudioMonoComponent->PlaySoundEffect(index);
			}
		}
	}

	if(Input::GetKeyPress(KeyCode::SPACE) || Input::GetKeyPress(KeyCode::ESCAPE) || Input::GetController().WasPressedThisFrame(CommonUtilities::Xbox::A))
	{
		myAudioMonoComponent->StopAllSoundEffect();
	}
}