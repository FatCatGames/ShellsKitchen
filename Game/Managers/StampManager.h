#pragma once
enum Stamp
{
	//Most procetage of work on each plate
	MVP = 0,
	//Hit 6 plates, players, pots, pans or chooping boards in the same level
	TrickShotter,
	//Wasted more than 7 ingredients
	FoodWaster,
	//Landed in the water more than 4 times
	Clumsy,
	//Pushed the other player more than 4 times in a game
	Annoying
};


class StampManager
{
public:
	static StampManager* GetInstance();
	~StampManager();

	void Reset();
	void AddStampPoints(Stamp aStamp, int aPoint, int aPlayerIndex);

	const std::array<bool, 5>& GetStamps(int aPlayerIndex);

private:
	static StampManager* myInstance;
	std::array<std::array<int, 5>, 2> myStampPoints;
	int myTrickShotReqiured = 6;
	int myFoodWasterReqiured = 8;
	int myClumsyRequired = 4;
	int myAnnoyingRequired = 4;
};

