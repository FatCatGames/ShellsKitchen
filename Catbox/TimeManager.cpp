#include "stdafx.h"
#include "TimeManager.h"

Catbox::Timer TimeManager::myTimer;
float TimeManager::myTimeSpeed = 1.0f;
float TimeManager::myTargetTimeSpeed = 0.0f;
float TimeManager::myLerpValue = 0.0f;
bool TimeManager::myShouldLerpTimeSpeed = false;
float TimeManager::myLerpStartTimeSpeed = 0.0f;
float TimeManager::myTotalTime = 0.0f;

void TimeManager::Update()
{
	myTimer.Update();
	PerformanceManager::ClearTimers();
	myTotalTime += GetDeltaTime() * myTimeSpeed;

	if(myShouldLerpTimeSpeed)
	{
		
		if (myLerpStartTimeSpeed < myTargetTimeSpeed) 
		{
			myTimeSpeed += Catbox::Lerp(myLerpStartTimeSpeed, myTargetTimeSpeed, myLerpValue * realDeltaTime);

			if (myTimeSpeed >= myTargetTimeSpeed)
			{
				myTimeSpeed = myTargetTimeSpeed;
				myShouldLerpTimeSpeed = false;
			}
		}
		else
		{
			myTimeSpeed -= Catbox::Lerp(myTargetTimeSpeed, myLerpStartTimeSpeed, myLerpValue * realDeltaTime);
			if (myTimeSpeed <= myTargetTimeSpeed)
			{
				myTimeSpeed = myTargetTimeSpeed;
				myShouldLerpTimeSpeed = false;
			}
		}
	}

	for (auto timer = myPayLoadMap.begin(); timer != myPayLoadMap.end();)
	{
		timer->second.myElpsedTime += GetDeltaTime();
		if (timer->second.myElpsedTime >= timer->second.myTime)
		{
			timer->second.aPayLoadFuction();
			if (timer->second.myShouldUpdate)
			{
				timer->second.myElpsedTime -= timer->second.myTime;
			}
			else
			{
				timer->second.aPayLoadFuction = nullptr;
				timer = myPayLoadMap.erase(timer);
				continue;
			}
		}
		++timer;
	}
}

UINT TimeManager::AddTimer(TimerPayload aTimerPayLoad)
{
	myIdCounter++;
	myPayLoadMap.insert({ myIdCounter, aTimerPayLoad });
	return myIdCounter;
}

UINT TimeManager::AddTimer(std::function<void()> aFuction, float aCurrentTime, float aStartTime, bool aShouldUpdate)
{
	myIdCounter++;
	TimerPayload aPayLoad;
	aPayLoad.aPayLoadFuction = aFuction;
	aPayLoad.myElpsedTime = aCurrentTime;
	aPayLoad.myTime = aStartTime;
	aPayLoad.myShouldUpdate = aShouldUpdate;
	myPayLoadMap.insert({ myIdCounter, aPayLoad });
	return myIdCounter;
}

void TimeManager::RemoveTimer(UINT aId)
{
	if (myPayLoadMap.find(aId) != myPayLoadMap.end()) 
	{
		myPayLoadMap.erase(aId);
	}
}

void TimeManager::ChangeTimeSpeed(float aProcetage) 
{
	myTimeSpeed = aProcetage;
}

void TimeManager::LerpTimeSpeed(float aTimeSpeedTarget, float t)
{
	myTargetTimeSpeed = aTimeSpeedTarget;
	myShouldLerpTimeSpeed = true;
	myLerpStartTimeSpeed = myTimeSpeed;
	myLerpValue = t;

}
