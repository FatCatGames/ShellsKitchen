#pragma once
#include "IngredientTypes.h"

#define OrderTime 120

struct RecipeIngredient
{
public:
	IngredientType myIngredientType;
	bool myShouldBeCut = false;
	bool myShouldBeCooked = false;
	bool myShouldBeBoiled = false;
	bool myIsFinished = false;

	RecipeIngredient(IngredientType aType, bool aShouldCut, bool aShouldCook, bool aShouldBoil = false)
	{
		myIngredientType = aType;
		myShouldBeCut = aShouldCut;
		myShouldBeCooked = aShouldCook;
		myShouldBeBoiled = aShouldBoil;
	}
};

struct Recipe
{
	Recipe(const std::string& aName, int anID, const std::string& aMeshName = "") : myName(aName), myMeshName(aMeshName)
	{
		myID = anID;
	}

	Recipe(const Recipe& aRecipe) = default;

	void AddIngredient(RecipeIngredient anIngredient)
	{
		myIngredients.push_back(anIngredient);
		myPoints += 10;
		if (anIngredient.myShouldBeCooked) myPoints += 10;
		if (anIngredient.myShouldBeCut) myPoints += 10;
	}

	int GetPointValue() const { return myPoints; }
	int GetRecipeID() const { return myID; }
	const std::string& GetName() const { return myName; }
	const std::string& GetPrefabName() const { return myMeshName; }
	const std::vector<RecipeIngredient>& GetIngredients() const { return myIngredients; }

private:
	std::string myName = "";
	std::string myMeshName = "";
	int myID = -1;
	int myPoints = 0;
	std::vector<RecipeIngredient> myIngredients;
};

struct Order
{
	Recipe* myRecipe = nullptr;
	float myTimer = OrderTime;
	int myOrderID = -1;

	Order(Recipe* aRecipe)
	{
		myRecipe = aRecipe;
	}
};

struct OrderResult
{
	Order order = nullptr;
	int points = 0;
	int tips = 0;
	bool success = false;

	OrderResult(Order anOrder, int aPoints, int aTips, bool aSucceeded) : order(anOrder)
	{
		points = aPoints;
		tips = aTips;
		success = aSucceeded;
	}
};

class Ingredient;
class Plate;
class OrderHandler
{
public:
	static OrderHandler* GetInstance() { return Instance; }
	OrderHandler();
	void Update();
	void Reset();
	OrderResult DeliverOrder(Plate* aPlate);
	bool CanDeliverOrder(Plate* aPlate);
	void AddPlateToTransit(Plate* aPlate);
	void AddOrderTimedOutListener(ListenerWithParameter& aListener);
	void AddOrderDeliveredListener(ListenerWithParameter& aListener);
	void AddOrderGeneratedListener(ListenerWithParameter& aListener);
	void SetAllowedRecipes();

	const std::vector<Recipe>& GetRecipeList();
	bool RecipeMatch(const Recipe& aRecipe, const std::vector<Ingredient*>& aIngredientList);

private:
	void GenerateOrder();
	void OnOrderDelivered(OrderResult& aResult);
	static OrderHandler* Instance;
	Event myOnOrderTimedOutEvent;
	Event myOnOrderDeliveredEvent;
	Event myOnOrderGeneratedEvent;
	float myTimeSinceOrder = 5;
	std::vector<Recipe> myPossibleRecipes;
	std::vector<Order> myCurrentOrders;
	std::vector<Plate*> myPlatesInTransit;
	int myCurrentOrderID = 0;
	int myAvrageDifficulty = 0;
	int myLatestDifficulty = 0;
	std::vector<int> myAllowedRecipes;

	int myPreviousRecipeID;
	int mySameRecipeInRowCounter = 0;
};