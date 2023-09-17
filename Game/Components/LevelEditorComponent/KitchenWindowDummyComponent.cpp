#include "Game.pch.h"
#include "KitchenWindowDummyComponent.h"
#include "Editor/Windows/KitchenMakerWindow.h"
#include "Managers/LevelSelectManager.h"
#include "ComponentTools/SceneManager.h"
#include "OrderHandler.h"

KitchenWindowDummyComponent::KitchenWindowDummyComponent()
{
	if (!EDITORMODE)
	{
		//myGameObject->Destroy();
	}
	else
	{
		KitchenMakerWindow::KitchenData currentLevel;
		currentLevel.currentLevel = GetLevel();

		if (currentLevel.currentLevel > -1)
		{
			const LevelScoreData& levelData = LevelSelectManager::GetInstance()->GetLevelData(currentLevel.currentLevel + 1);
			std::vector<Recipe> recipeList = OrderHandler::GetInstance()->GetRecipeList();
			currentLevel.recipes.resize(recipeList.size());

			for (bool rec : currentLevel.recipes)
			{
				rec = false;
			}

			for (size_t i = 0; i < levelData.myAllowedRecipes.size(); i++)
			{
				currentLevel.recipes[levelData.myAllowedRecipes[i]] = true;
			}

			currentLevel.bronzeScore = levelData.myBronzeScore;
			currentLevel.silverScore = levelData.mySilverScore;
			currentLevel.goldScore = levelData.myGoldScore;
			currentLevel.levelDuration = static_cast<int>(levelData.myLevelTime);
		}

		if (Editor::GetInstance()->GetWindowHandler().GetWindowOfType<KitchenMakerWindow>(WindowType::KitchenEditor))
			Editor::GetInstance()->GetWindowHandler().GetWindowOfType<KitchenMakerWindow>(WindowType::KitchenEditor)->SetLevelData(currentLevel);
	}
}

void KitchenWindowDummyComponent::Awake()
{
	GameManager::GetInstance()->GenerateOrders(true);
	OrderHandler::GetInstance()->SetAllowedRecipes();
	myGameObject->Destroy();
}

void KitchenWindowDummyComponent::RunInEditor()
{
	if (!PLAYMODE)
	{
		auto kitchenEditor = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<KitchenMakerWindow>(WindowType::KitchenEditor);
		if (kitchenEditor)
		{
			if (kitchenEditor->GetShouldSave())
			{
				kitchenEditor->Saved();
				auto& levelData = kitchenEditor->GetLevelData();
				std::vector<int> allowedRecipes;
				for (size_t i = 0; i < levelData.recipes.size(); i++)
				{
					if (levelData.recipes[i]) allowedRecipes.push_back(i);
				}
				if (levelData.currentLevel > -1)
				{
					LevelSelectManager::GetInstance()->UpdateLevelData(levelData.currentLevel, levelData.bronzeScore, levelData.silverScore, levelData.goldScore, levelData.levelDuration, allowedRecipes);
				}
				LevelSelectManager::GetInstance()->Save();
			}
			if (kitchenEditor->GetRequestRecipes())
			{
				std::vector<Recipe> recipeList = OrderHandler::GetInstance()->GetRecipeList();
				std::vector<std::string> recipeNames;

				for (size_t i = 0; i < recipeList.size(); i++)
				{
					recipeNames.push_back(recipeList[i].GetName());
				}

				kitchenEditor->PushRecipeNames(recipeNames);
			}
		}
	}
}

int KitchenWindowDummyComponent::GetLevel()
{
	std::string levelName = Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->GetName();
	if (levelName == "First_Level_lvl")
	{
		return 0;
	}
	else if (levelName == "Second_Level_lvl")
	{
		return 1;
	}
	else if (levelName == "Third_Level_lvl")
	{
		return 2;
	}
	else if (levelName == "Fourth_Level_lvl")
	{
		return 3;
	}
	else if (levelName == "Fifth_Level_lvl")
	{
		return 4;
	}
	else if (levelName == "Sixth_Level_lvl")
	{
		return 5;
	}
	else if (levelName == "Seventh_Level_lvl")
	{
		return 6;
	}
	else if (levelName == "Eighth_Level_lvl")
	{
		return 7;
	}
	else if (levelName == "Ninth_Level_lvl")
	{
		return 8;
	}	
	else if (levelName == "Tenth_Level_lvl")
	{
		return 9;
	}
	else
	{
		return -1;
	}
}
