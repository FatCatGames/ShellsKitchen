#pragma once
#include "Components/UI/UISprite.h"

class CoinCounterComponent : public Component
{
public:
	void Awake() override;
	void SetValue(const int& aValue);
	void AddValue(const int& aValue);
	void Update() override;

private:
	int myValueToShow = 0;
	int myUnits = 0;
	int myTens = 0;
	int myHundreds = 0;
	int myThousands = 0;

};