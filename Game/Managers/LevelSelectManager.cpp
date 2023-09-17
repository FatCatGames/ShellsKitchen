#include "Game.pch.h"
#include "LevelSelectManager.h"
#include "../External/rapidjson/filereadstream.h"
//#include "../External/rapidjson/filewritestream.h"
#include "../External/rapidjson/prettywriter.h"
//#include "../External/rapidjson/stringbuffer.h"
#include "../External/rapidjson/ostreamwrapper.h"

LevelSelectManager* LevelSelectManager::Instance;

LevelSelectManager::LevelSelectManager()
{
	Instance = this;
}

LevelSelectManager::~LevelSelectManager()
{
	Save();
}

void LevelSelectManager::Update()
{

}

void LevelSelectManager::Init()
{
	Load();
}

void LevelSelectManager::SetLevelData(const int& aLevelIndex, const int& aBronzeScore, const int& aSilverScore, const int& aGoldScore, const bool& aHasReachedBronze, const bool& aHasReachedSilver, const bool& aHasReachedGold, const int& aLevelTime)
{
	if (aLevelIndex > myLevelScoreDatas.size()) return;

	myLevelScoreDatas[aLevelIndex].myBronzeScore = aBronzeScore;
	myLevelScoreDatas[aLevelIndex].mySilverScore = aSilverScore;
	myLevelScoreDatas[aLevelIndex].myGoldScore = aGoldScore;

	myLevelScoreDatas[aLevelIndex].myLevelTime = aLevelTime;

	myLevelScoreDatas[aLevelIndex].myHasReachedBronze = aHasReachedBronze;
	myLevelScoreDatas[aLevelIndex].myHasReachedSilver = aHasReachedSilver;
	myLevelScoreDatas[aLevelIndex].myHasReachedGold = aHasReachedGold;
}

void LevelSelectManager::UpdateLevelData(const int& aLevelIndex, const int& aBronzeScore, const int& aSilverScore, const int& aGoldScore, const int& aTime, const std::vector<int>& someAllowedRecipes)
{
	myLevelScoreDatas[aLevelIndex].myBronzeScore = aBronzeScore;
	myLevelScoreDatas[aLevelIndex].mySilverScore = aSilverScore;
	myLevelScoreDatas[aLevelIndex].myGoldScore = aGoldScore;
	myLevelScoreDatas[aLevelIndex].myLevelTime = static_cast<float>(aTime);
	myLevelScoreDatas[aLevelIndex].myAllowedRecipes = someAllowedRecipes;
}

const LevelScoreData& LevelSelectManager::GetLevelData(int aLevel)
{
	if (aLevel - 1 >= 0 && aLevel -1 < myLevelScoreDatas.size())
	{
		return myLevelScoreDatas[aLevel - 1];
	}
	return myLevelScoreDatas[0];
}

void LevelSelectManager::SetLevelAwards(const int& aLevelIndex, const bool& aBronze, const bool& aSilver, const bool& aGold, const int& aScore)
{
	if(aLevelIndex > myLevelScoreDatas.size()) return;

	myLevelScoreDatas[aLevelIndex].myHasReachedBronze = aBronze;
	myLevelScoreDatas[aLevelIndex].myHasReachedSilver = aSilver;
	myLevelScoreDatas[aLevelIndex].myHasReachedGold = aGold;
	if (aScore > myLevelScoreDatas[aLevelIndex].myHighestScore)	myLevelScoreDatas[aLevelIndex].myHighestScore = aScore;
}

void LevelSelectManager::Save()
{
	int version = 3;
	rapidjson::Document output;
	output.SetObject();
	auto& alloc = output.GetAllocator();
	rapidjson::Value value;

	value.SetInt(version);
	output.AddMember("Version", value, alloc);

	rapidjson::Value tempArray(rapidjson::kArrayType);
	for (size_t i = 0; i < myLevelScoreDatas.size(); i++)
	{
		rapidjson::Value levelNumber;
		levelNumber.SetObject();
		levelNumber.AddMember("BronzeRequirement", myLevelScoreDatas[i].myBronzeScore, alloc);
		levelNumber.AddMember("SilverRequirement", myLevelScoreDatas[i].mySilverScore, alloc);
		levelNumber.AddMember("GoldRequirement", myLevelScoreDatas[i].myGoldScore, alloc);
		levelNumber.AddMember("Highscore", myLevelScoreDatas[i].myHighestScore, alloc);
		levelNumber.AddMember("LevelDuration", myLevelScoreDatas[i].myLevelTime, alloc);

		rapidjson::Value recipeArray(rapidjson::kArrayType);
		for (size_t j = 0; j < myLevelScoreDatas[i].myAllowedRecipes.size(); j++)
		{
			recipeArray.PushBack(myLevelScoreDatas[i].myAllowedRecipes[j], alloc);
		}
		levelNumber.AddMember("AllowedRecipes", recipeArray, alloc);
		tempArray.PushBack(levelNumber, alloc);
	}
	output.AddMember("Levels", tempArray, alloc);

	std::ofstream ofs("Assets/Scenes/Levels/LevelData.json");
	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	output.Accept(writer);
}

void LevelSelectManager::Load()
{
	FILE* fp;
	bool error = fopen_s(&fp, "Assets/Scenes/Levels/LevelData.json", "rb");
	if (error) //no project settings file found
	{
		return;
	}

	rapidjson::Document document;
	char readbuffer[65536]{};
	rapidjson::FileReadStream is(fp, readbuffer, sizeof(readbuffer));
	document.ParseStream(is);
	fclose(fp);

	int version = document["Version"].GetInt();

	if (version >= 1)
	{
		for (size_t i = 0; i < document["Levels"].Size(); i++)
		{
			myLevelScoreDatas[i].myBronzeScore = document["Levels"][i]["BronzeRequirement"].GetInt();
			myLevelScoreDatas[i].mySilverScore = document["Levels"][i]["SilverRequirement"].GetInt();
			myLevelScoreDatas[i].myGoldScore = document["Levels"][i]["GoldRequirement"].GetInt();
			myLevelScoreDatas[i].myHighestScore = document["Levels"][i]["Highscore"].GetInt();
			if (myLevelScoreDatas[i].myHighestScore > myLevelScoreDatas[i].myBronzeScore)
			{
				myLevelScoreDatas[i].myHasReachedBronze = true;

				if (myLevelScoreDatas[i].myHighestScore > myLevelScoreDatas[i].mySilverScore)
				{
					myLevelScoreDatas[i].myHasReachedSilver = true;

					if (myLevelScoreDatas[i].myHighestScore > myLevelScoreDatas[i].myGoldScore)
					{
						myLevelScoreDatas[i].myHasReachedGold = true;
					}
				}
			}
		}
	}
	if (version >= 2)
	{
		for (size_t i = 0; i < document["Levels"].Size(); i++)
		{
			myLevelScoreDatas[i].myLevelTime = document["Levels"][i]["LevelDuration"].GetFloat();
		}
	}
	if (version >= 3)
	{
		for (size_t i = 0; i < document["Levels"].Size(); i++)
		{
			std::vector<int> allowedRecipes;
			for (size_t j = 0; j < document["Levels"][i]["AllowedRecipes"].Size(); j++)
			{
				allowedRecipes.push_back(document["Levels"][i]["AllowedRecipes"][j].GetInt());
			}
			myLevelScoreDatas[i].myAllowedRecipes = allowedRecipes;
		}
	}
}