#include "Game.pch.h"
#include "LevelSelectDataHandler.h"
#include "Components\LevelSelect\LevelSelectPreview.h"

void LevelSelectDataHandler::Awake()
{
	LevelSelectManager::GetInstance()->SetLevelDataHandler(this);

	const int numberOfLevels = 10;

	for(int index = 0; index < numberOfLevels; index++)
	{
		const bool reachedBronze = LevelSelectManager::GetInstance()->GetHasBronze(index);
		const bool reachedSilver = LevelSelectManager::GetInstance()->GetHasSilver(index);
		const bool reachedGold = LevelSelectManager::GetInstance()->GetHasGold(index);

		const int bronzeScore = LevelSelectManager::GetInstance()->GetBronzeScore(index);
		const int silverScore = LevelSelectManager::GetInstance()->GetSilverScore(index);
		const int goldScore = LevelSelectManager::GetInstance()->GetGoldScore(index);

		if(index != 9)
		{
			myTransform->GetChildren()[index]->GetGameObject()->GetTransform()->GetChildren()[0]->GetGameObject()
				->GetComponent<LevelSelectPreview>()->SetValues(reachedBronze, reachedSilver, reachedGold, bronzeScore, silverScore, goldScore);
		}
	}
}