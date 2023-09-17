#pragma once
#include "InteractableSurface.h"

class Ingredient;
class PlateHolder : public InteractableSurface
{
public:
	void Awake() override;
	void Interact(Player* aPlayer) override;
	void InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem) override;
	void AddPlate(Plate* aPlate);
	bool CanAddPlate(Plate* aPlate);

private:
	bool PlateFood(Ingredient* aFood);
};