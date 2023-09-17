#include "Game.pch.h"
#include "AudioPlayer.h"
#include "Managers\GameManager.h"

void AudioPlayer::Awake()
{
	myAudioMonoComponent = myGameObject->GetComponent<AudioMonoComponent>();
	GameManager::GetInstance()->SetAudioPlayer(this);
}

void AudioPlayer::Update()
{
	if(myShouldStartPlaying)
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
	myShouldStartPlaying = false;

	if(myIsMusicPitching == true)
	{

		Engine::GetInstance()->GetAudioManager()->SetParameterByIndex(myAudioMonoComponent->GetSoundEventByIndex(1), 0, myTargetMusicPitch);
		myCurrentMusicPitch = myTargetMusicPitch;
		myIsMusicPitching = false;

		// Want to lerp it? Try the code below. 
		//myPitchTimer += deltaTime;

		//const float pitchDuration = 0.5f;
		//const float pitchToSet = Catbox::Lerp(myCurrentMusicPitch, myTargetMusicPitch, myPitchTimer / pitchDuration);

		//Engine::GetInstance()->GetAudioManager()->SetParameterByIndex(myAudioMonoComponent->GetSoundEventByIndex(1), 0, pitchToSet);

		//if(pitchDuration < myPitchTimer)
		//{
		//	myIsMusicPitching = false;
		//	myCurrentMusicPitch = myTargetMusicPitch;
		//	myPitchTimer = 0.0f;
		//}
	}
}

void AudioPlayer::StartAudio()
{
	myShouldStartPlaying = true;
}

void AudioPlayer::PitchMusic(const float& aTargetPitch)
{
	myIsMusicPitching = true;
	myTargetMusicPitch = aTargetPitch;
}
