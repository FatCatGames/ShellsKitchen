#pragma once
#include "Components/UI/UISprite.h"

class CounterComponent : public Component
{
public:
	void Awake() override;
	void SetValue(const int& aValue);
	void AddValue(const int& aValue);

private:
	UISprite* myThousandSprite = nullptr;
	UISprite* myHundredsSprite = nullptr;
	UISprite* myTensSprite = nullptr;
	UISprite* myUnitsSprite = nullptr;

	int myValueToShow = 0;
	int units = 0;
	int tens = 0;
	int hundreds = 0;
	int thousands = 0;

};