#include "Game.pch.h"
#include "LevelSelectAudioPlayer.h"
#include "Managers\GameManager.h"

void LevelSelectAudioPlayer::Awake()
{
	myAudioMonoComponent = myGameObject->GetComponent<AudioMonoComponent>();
}

void LevelSelectAudioPlayer::Update()
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
}