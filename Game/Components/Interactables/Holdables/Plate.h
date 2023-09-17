#pragma once

class Ingredient;
class Plate : public HoldableItem
{
public:
	void InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem) override;
	const std::vector<Ingredient*>& GetIngredients() { return myIngredients; }
	void Update() override;
	void PlateIngredient(Ingredient* anIngredient);
	bool CanPlateIngredient(Ingredient* anIngredient);
	void Clear();
	void SetIsInTransit(bool aIsInTransit);
	float GetTransitTimer() { return myTimeInTransit; }
	void OffsetIngredientIcons();
	bool HasIngredientType(IngredientType aType);

private:
	std::vector<Ingredient*> myIngredients;
	float myTimeInTransit = 0;
	bool myIsInTransit = false;
	bool myShouldChangeIngredientIcon = false;
};

