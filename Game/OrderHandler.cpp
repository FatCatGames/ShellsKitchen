#include "Game.pch.h"
#include "OrderHandler.h"
#include "Components\Interactables\Holdables\Plate.h"
#include "Components\Interactables\Holdables\Ingredient.h"
#include "Components\Interactables\Surfaces\PlateHolder.h"
#include "Managers/LevelSelectManager.h"
#include "..\Catbox\Audio\AudioManager.h"
#include "Components\GamePlay\FloatingScore.h"

OrderHandler* OrderHandler::Instance;

OrderHandler::OrderHandler()
{
	Instance = this;

	//Ingredients
	RecipeIngredient rawFish(IngredientType::Fish, true, false);
	RecipeIngredient cookedFish(IngredientType::Fish, false, true);

	RecipeIngredient rawSquid(IngredientType::Squid, true, false);
	RecipeIngredient cookedSquid(IngredientType::Squid, true, true);

	RecipeIngredient rawShrimp(IngredientType::Shrimp, true, false);
	RecipeIngredient cookedShrimp(IngredientType::Shrimp, true, true);

	RecipeIngredient cookedRice(IngredientType::Rice, false, false, true);
	RecipeIngredient friedRice(IngredientType::Rice, false, true, true);

	RecipeIngredient seaweed(IngredientType::Seaweed, false, false);
	RecipeIngredient caviar(IngredientType::Caviar, false, false);


	//Sashimi
	Recipe sashimiRecipe("Sashimi", 0);
	sashimiRecipe.AddIngredient(rawFish);
	myPossibleRecipes.push_back(sashimiRecipe);

	//Squid sashimi
	Recipe squidSashimiRecipe("Squid Sashimi", 1);
	squidSashimiRecipe.AddIngredient(rawSquid);
	myPossibleRecipes.push_back(squidSashimiRecipe);

	//Fried fish
	Recipe friedFishRecipe("Fried fish", 2);
	friedFishRecipe.AddIngredient(cookedFish);
	myPossibleRecipes.push_back(friedFishRecipe);

	//Fried shrimp
	Recipe friedShrimpRecipe("Fried shrimp", 3);
	friedShrimpRecipe.AddIngredient(cookedShrimp);
	myPossibleRecipes.push_back(friedShrimpRecipe);

	//Fried squid
	Recipe friedSquidRecipe("Fried squid", 4);
	friedSquidRecipe.AddIngredient(cookedSquid);
	myPossibleRecipes.push_back(friedSquidRecipe);

	//Shrimp fried rice
	Recipe shrimpFriedRiceRecipe("Shrimp fried rice", 5, "ShrimpFriedRice");
	shrimpFriedRiceRecipe.AddIngredient(cookedShrimp);
	shrimpFriedRiceRecipe.AddIngredient(friedRice);
	myPossibleRecipes.push_back(shrimpFriedRiceRecipe);

	//Squid fried rice
	Recipe squidFriedRiceRecipe("Squid fried rice", 6, "SquidFriedRice");
	squidFriedRiceRecipe.AddIngredient(cookedSquid);
	squidFriedRiceRecipe.AddIngredient(friedRice);
	myPossibleRecipes.push_back(squidFriedRiceRecipe);

	//Shrimp sushi
	Recipe shrimpSushiRecipe("Shrimp sushi", 7, "ShrimpSushi");
	shrimpSushiRecipe.AddIngredient(cookedRice);
	shrimpSushiRecipe.AddIngredient(seaweed);
	shrimpSushiRecipe.AddIngredient(rawShrimp);
	myPossibleRecipes.push_back(shrimpSushiRecipe);

	//Squid sushi
	Recipe squidSushiRecipe("Squid sushi", 8, "SquidSushi");
	squidSushiRecipe.AddIngredient(cookedRice);
	squidSushiRecipe.AddIngredient(seaweed);
	squidSushiRecipe.AddIngredient(rawSquid);
	myPossibleRecipes.push_back(squidSushiRecipe);

	//Fish sushi
	Recipe fishSushiRecipe("Fish sushi", 9, "FishSushi");
	fishSushiRecipe.AddIngredient(cookedRice);
	fishSushiRecipe.AddIngredient(seaweed);
	fishSushiRecipe.AddIngredient(rawFish);
	myPossibleRecipes.push_back(fishSushiRecipe);

	//Shrimp sushi with caviar
	Recipe shrimpCaviarSushiRecipe("Shrimp caviar sushi", 10, "ShrimpCaviarSushi");
	shrimpCaviarSushiRecipe.AddIngredient(cookedRice);
	shrimpCaviarSushiRecipe.AddIngredient(seaweed);
	shrimpCaviarSushiRecipe.AddIngredient(rawShrimp);
	shrimpCaviarSushiRecipe.AddIngredient(caviar);
	myPossibleRecipes.push_back(shrimpCaviarSushiRecipe);

	//Squid sushi
	Recipe squidCaviarSushiRecipe("Squid caviar sushi", 11, "SquidCaviarSushi");
	squidCaviarSushiRecipe.AddIngredient(cookedRice);
	squidCaviarSushiRecipe.AddIngredient(seaweed);
	squidCaviarSushiRecipe.AddIngredient(rawSquid);
	squidCaviarSushiRecipe.AddIngredient(caviar);
	myPossibleRecipes.push_back(squidCaviarSushiRecipe);

	//Fish sushi
	Recipe fishCaviarSushiRecipe("Fish caviar sushi", 12, "FishCaviarSushi");
	fishCaviarSushiRecipe.AddIngredient(cookedRice);
	fishCaviarSushiRecipe.AddIngredient(seaweed);
	fishCaviarSushiRecipe.AddIngredient(rawFish);
	fishCaviarSushiRecipe.AddIngredient(caviar);
	myPossibleRecipes.push_back(fishCaviarSushiRecipe);
}

void OrderHandler::Update()
{
	//Slightly more performant since it reduces function calls
	float timeDelta = deltaTime;

	myTimeSinceOrder += timeDelta;

	if ((myTimeSinceOrder > 10 && myCurrentOrders.size() < 3) || myCurrentOrders.empty())
	{
		GenerateOrder();
	}

	for (size_t i = 0; i < myPlatesInTransit.size();)
	{
		if (myPlatesInTransit[i]->GetTransitTimer() > 5)
		{
			auto plateHolder = GameManager::GetInstance()->GetCurrentLevelManager().GetPlateHolder();
			if (!plateHolder)
			{
				printerror("NO PLATEHOLDER IN SCENE!!");
				break;
			}
			plateHolder->AddPlate(myPlatesInTransit[i]);
			myPlatesInTransit[i]->SetIsInTransit(false);
			myPlatesInTransit.erase(myPlatesInTransit.begin() + i);
		}
		else i++;
	}

	for (size_t i = 0; i < myCurrentOrders.size();)
	{
		myCurrentOrders[i].myTimer -= timeDelta;
		if (myCurrentOrders[i].myTimer <= 0)
		{
			OrderResult result(myCurrentOrders[i], 0, 0, false);
			GameManager::GetInstance()->RemoveOrderSprite(myCurrentOrders[i].myOrderID);
			int failedScore = GameManager::GetInstance()->ChangeCurrentLevelScore(-30, true);
			GameManager::GetInstance()->AddFailedOrderScore(failedScore);

			GameObject* floatingScore = InstantiatePrefab("FloatingScore");
			floatingScore->GetComponent<FloatingScore>()->SetPoints(failedScore);

			myCurrentOrders.erase(myCurrentOrders.begin() + i);
			myOnOrderTimedOutEvent.Invoke(&result);
		}
		else i++;
	}
}

void OrderHandler::Reset()
{
	myTimeSinceOrder = 5;
	myCurrentOrders.clear();
	myPlatesInTransit.clear();
	myOnOrderDeliveredEvent.ClearAllListeners();
	myOnOrderGeneratedEvent.ClearAllListeners();
	myOnOrderTimedOutEvent.ClearAllListeners();
	myCurrentOrderID = 0;
	myAvrageDifficulty = 0;
}

OrderResult OrderHandler::DeliverOrder(Plate* aPlate)
{
	auto& platedIngredients = aPlate->GetIngredients();

	bool successfulDelivery = false;
	int index = 0;
	for (auto& order : myCurrentOrders)
	{
		if (RecipeMatch(*order.myRecipe, platedIngredients))
		{
			successfulDelivery = true;
			break;
		}

		++index;
	}

	if (successfulDelivery)
	{
		int points = myCurrentOrders[index].myRecipe->GetPointValue();
		int maxTips = std::min(points / 2, 20);
		int receivedTips = maxTips * (myCurrentOrders[index].myTimer / OrderTime);
		print("Succesfully delivered " + myCurrentOrders[index].myRecipe->GetName() + ". Tips: " + std::to_string(receivedTips));

		int orderID = myCurrentOrders[index].myOrderID;

		Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_FOOD_AT_COUNTER));

		GameManager::GetInstance()->ChangeCurrentLevelScore(points + receivedTips);
		GameManager::GetInstance()->AddTips(receivedTips);
		GameManager::GetInstance()->AddDeliveredScore(points);

		GameManager::GetInstance()->RemoveOrderSprite(orderID);
		OrderResult result(myCurrentOrders[index], points, receivedTips, true);
		myCurrentOrders.erase(myCurrentOrders.begin() + index);
		OnOrderDelivered(result);

		GameObject* floatingScore = InstantiatePrefab("FloatingScore");
		floatingScore->GetComponent<FloatingScore>()->SetPoints(points + receivedTips);
		return result;
	}
	else
	{
		Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_WRONG_ORDER));
		print("You delivered something nobody ordered");
		OrderResult result(nullptr, 0, 0, false);
		OnOrderDelivered(result);
		return result;
	}
}

bool OrderHandler::CanDeliverOrder(Plate* aPlate)
{
	auto& platedIngredients = aPlate->GetIngredients();
	if (platedIngredients.empty()) return false;
	return true;
}

void OrderHandler::AddPlateToTransit(Plate* aPlate)
{
	if (!aPlate) return;
	myPlatesInTransit.push_back(aPlate);
	aPlate->SetIsInTransit(true);
	aPlate->Clear();
}

void OrderHandler::AddOrderTimedOutListener(ListenerWithParameter& aListener)
{
	myOnOrderTimedOutEvent.AddListener(aListener);
}

void OrderHandler::AddOrderDeliveredListener(ListenerWithParameter& aListener)
{
	myOnOrderDeliveredEvent.AddListener(aListener);
}

void OrderHandler::AddOrderGeneratedListener(ListenerWithParameter& aListener)
{
	myOnOrderGeneratedEvent.AddListener(aListener);
}

void OrderHandler::GenerateOrder()
{
	bool leeway = false;
	if (myLatestDifficulty > myAvrageDifficulty)
	{
		leeway = true;
	}

	int rand = Catbox::GetRandom(0, static_cast<int>(myAllowedRecipes.size()));;

	if (leeway)
	{
		while (myPossibleRecipes[myAllowedRecipes[rand]].GetPointValue() == myLatestDifficulty)
		{
			rand = Catbox::GetRandom(0, static_cast<int>(myAllowedRecipes.size()));
		}
	}

	if (myPossibleRecipes[myAllowedRecipes[rand]].GetRecipeID() == myPreviousRecipeID)
	{
		if (mySameRecipeInRowCounter > Catbox::GetRandom(0, 3))
		{
			mySameRecipeInRowCounter = 0;
			while (myPossibleRecipes[myAllowedRecipes[rand]].GetRecipeID() == myPreviousRecipeID)
			{
				rand = Catbox::GetRandom(0, static_cast<int>(myAllowedRecipes.size()));
			}
		}
		else
		{
			++mySameRecipeInRowCounter;
		}
	}

	Order newOrder(&myPossibleRecipes[myAllowedRecipes[rand]]);
	myPreviousRecipeID = myPossibleRecipes[myAllowedRecipes[rand]].GetRecipeID();
	newOrder.myOrderID = myCurrentOrderID;

	myCurrentOrders.push_back(newOrder);
	myTimeSinceOrder = 0;
	GameManager::GetInstance()->AddOrderSprite(newOrder, newOrder.myRecipe->GetRecipeID());
	myCurrentOrderID++;

	myOnOrderGeneratedEvent.Invoke(&myCurrentOrders.back());

	myLatestDifficulty = myPossibleRecipes[myAllowedRecipes[rand]].GetPointValue();
}

void OrderHandler::OnOrderDelivered(OrderResult& aResult)
{
	myOnOrderDeliveredEvent.Invoke(&aResult);
}

void OrderHandler::SetAllowedRecipes()
{
	auto levelData = LevelSelectManager::GetInstance()->GetLevelData(LevelSelectManager::GetInstance()->GetCurrentLevel());
	myAllowedRecipes = levelData.myAllowedRecipes;

	for (int i = 0; i < myAllowedRecipes.size(); i++)
	{
		myAvrageDifficulty += myPossibleRecipes[myAllowedRecipes[i]].GetPointValue();
	}
	myAvrageDifficulty = myAvrageDifficulty / myAllowedRecipes.size();
}

const std::vector<Recipe>& OrderHandler::GetRecipeList()
{
	return myPossibleRecipes;
}

bool OrderHandler::RecipeMatch(const Recipe& aRecipe, const std::vector<Ingredient*>& aIngredientList)
{
	if (aIngredientList.size() != aRecipe.GetIngredients().size()) return false;

	//Go over all the ingredients on the plate
	for (Ingredient* platedIngredient : aIngredientList)
	{
		bool ingredientMatch = false;

		//Check if it matches one of the ingredients of the recipe
		for (auto& recipeIngredient : aRecipe.GetIngredients())
		{
			if (platedIngredient->GetIngredientType() != recipeIngredient.myIngredientType) continue;
			if (platedIngredient->GetIsCut() != recipeIngredient.myShouldBeCut) continue;
			if (platedIngredient->GetIsCooked() != recipeIngredient.myShouldBeCooked) continue;

			ingredientMatch = true;
		}

		if (!ingredientMatch) return false;
	}

	return true;
}
