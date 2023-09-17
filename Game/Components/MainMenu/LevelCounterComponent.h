#pragma once
#include "Components/UI/UISprite.h"
#include "ComponentTools/Event.h"

class LevelCounterComponent : public Component
{
public:
	void Awake() override;
	void Update() override;
	void SetValue(const int& aValue);
	void AddValue(const int& aValue);
	void AddThirtySecondsListener(Listener& aListener) { myThirtySecondsEvent.AddListener(aListener); }
	void AddTenSecondsListener(Listener& aListener) { myThirtySecondsEvent.AddListener(aListener); }
	const float& GetRemainingTime() const { return myCurrentTime; }

private:
	int myValueToShow = 0;
	int units = 0;
	int tens = 0;
	int hundreds = 0;
	int thousands = 0;

	const float myStartTime = 11.0f;
	float myCurrentTime = 11.0f;
	float myTickTimer = 1.0f;

	bool myShouldCount = true;
	bool myShouldTickTime = false;

	bool myHasPlayedCountdownAlert = false;
	bool myHasPlayedCountdownAlertSecondTime = false;

	bool myHasDoneFirstMusicPitch = false;
	bool myHasDoneSecondMusicPitch = false;
	Event myThirtySecondsEvent;
	Event myTenSecondsEvent;
};