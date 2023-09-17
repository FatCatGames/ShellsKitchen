#pragma once
#include "Components\LevelSelect\LevelSelectDataHandler.h"

struct LevelScoreData
{
	int myHighestScore = 0;

	int myBronzeScore = 0;
	int mySilverScore = 0;
	int myGoldScore = 0;

	bool myHasReachedBronze = false;
	bool myHasReachedSilver = false;
	bool myHasReachedGold = false;

	float myLevelTime = 120.f;

	std::vector<int> myAllowedRecipes = {0,1,2};
};

class LevelSelectManager
{
public:
	static inline LevelSelectManager* GetInstance() { return Instance; }
	LevelSelectManager();
	~LevelSelectManager();
	void Update();
	void Init();

	void SetLevelDataHandler(LevelSelectDataHandler* aDataHandler) { myDataHandler = aDataHandler; }
	LevelSelectDataHandler* GetLevelDataHandler() { return myDataHandler; }

	void SetLevelData(const int& aLevelIndex, const int& aBronzeScore, const int& aSilverScore, const int& aGoldScore, const bool& aHasReachedBronze, const bool& aHasReachedSilver, const bool& aHasReachedGold, const int& aLevelTime);
	void UpdateLevelData(const int& aLevelIndex, const int& aBronzeScore, const int& aSilverScore, const int& aGoldScore, const int& aTime, const std::vector<int>& someAllowedRecipes);
	const LevelScoreData& GetLevelData(int aLevel);
	void SetLevelAwards(const int& aLevelIndex, const bool& aBronze, const bool& aSilver, const bool& aGold, const int& aScore);
	void Save();
	void Load();

	int GetBronzeScore(const int& aLevelIndex) { return myLevelScoreDatas[aLevelIndex].myBronzeScore; }
	int GetSilverScore(const int& aLevelIndex) { return myLevelScoreDatas[aLevelIndex].mySilverScore; }
	int GetGoldScore(const int& aLevelIndex) { return myLevelScoreDatas[aLevelIndex].myGoldScore; }
	bool GetHasBronze(const int& aLevelIndex) { return myLevelScoreDatas[aLevelIndex].myHasReachedBronze; }
	bool GetHasSilver(const int& aLevelIndex) { return myLevelScoreDatas[aLevelIndex].myHasReachedSilver; }
	bool GetHasGold(const int& aLevelIndex) { return myLevelScoreDatas[aLevelIndex].myHasReachedGold; }
	float GetLevelDuration(const int& aLevelIndex) { return myLevelScoreDatas[aLevelIndex].myLevelTime; }

	void SetCurrentLevel(const int& aCurrentLevel) { myCurrentLevel = aCurrentLevel; }
	int GetCurrentLevel() { return myCurrentLevel; }

private:
	static LevelSelectManager* Instance;
	std::array<LevelScoreData, 10> myLevelScoreDatas;

	LevelSelectDataHandler* myDataHandler = nullptr;
	int myCurrentLevel = 0;
};
