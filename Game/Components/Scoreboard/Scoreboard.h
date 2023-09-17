#pragma once

class Scoreboard : public Component
{
public:
	void Awake() override;
	void SetValues(int score, int tips, int failed, int bronze, int silver, int gold);

private:
	int myOrdersDeliveredScore = 0;
	int myTips = 0;
	int myOrdersFailedScore = 0;
	int myTotalScore = 0;

	int myBronzeScore = 10;
	int mySilverScore = 100;
	int myGoldScore = 200;

	bool myHasStoppedMusic = false;
};