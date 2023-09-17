#include "Game.pch.h"
#include "PlayerData.h"

PlayerData::PlayerData()
{
}

PlayerData::~PlayerData()
{
}

void PlayerData::SetHatID(int aID)
{
	int aNum = GameManager::GetInstance()->GetMyHatsVector().size();
	if (aID > aNum -1)
	{
		myHatID = 0;
	}
	else if (aID < 0)
	{
		myHatID = GameManager::GetInstance()->GetMyHatsVector().size() - 1;
	}
	else 
	{
		myHatID = aID;
	}
}
