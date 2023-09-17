#pragma once
#include <functional>

struct TimerPayload
{
public:
	std::function<void()> aPayLoadFuction;
	float myTime = 0.0f;
	bool myShouldUpdate = false;
	float myElpsedTime = 0.0f;
};
class TimeManager
{
public:
	static bool TimerExist(int aId) { return myPayLoadMap.find(aId) != myPayLoadMap.end(); }
	static void Update();
	static UINT AddTimer(TimerPayload aTimerPayLoad);
	static UINT AddTimer(std::function<void()> aFuction, float aCurrentTime = 0.0f, float aStartTime = 0.0f, bool aShouldUpdate = false);
	static void RemoveTimer(UINT aId);
	static void SetPaused(bool aFlag) { myTimer.SetPaused(aFlag); }
	static bool IsPaused() { return myTimer.IsPaused(); }
	static float GetDeltaTime() { if (myTimer.IsPaused()) return 0; return myTimer.GetDeltaTime() * myTimeSpeed; }
	static float GetTotalTime() { return myTotalTime; }
	static float GetRealDeltaTime() { return myTimer.GetDeltaTime(); }
	static double GetRealTotalTime() { return myTimer.GetTotalTime(); }
	static void ChangeTimeSpeed(float aProcetage);
	static float GetTimeSpeed() { return myTimeSpeed; }
	static void LerpTimeSpeed(float aTimeSpeedTarget, float t);
	static void ResetTimeSpeed() { myTimeSpeed = 1.0f; myTargetTimeSpeed = 0.0f; myShouldLerpTimeSpeed = false; myLerpValue = 0; }
private:
	inline static std::unordered_map<int,TimerPayload> myPayLoadMap;
	inline static int myIdCounter = -1;
	static Catbox::Timer myTimer;
	static float myTimeSpeed;
	static float myTargetTimeSpeed;
	static float myLerpValue;
	static bool myShouldLerpTimeSpeed;
	static float myLerpStartTimeSpeed;
	static float myTotalTime;
};

