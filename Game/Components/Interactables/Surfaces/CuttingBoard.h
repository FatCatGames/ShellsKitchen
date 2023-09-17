#pragma once
#include "InteractableSurface.h"

class ProgressBar;
class Ingredient;
class CuttingBoard : public InteractableSurface
{
public:
	void Awake() override;
	void BeginPerformingActionOn(Player* aPlayer) override;
	void PerformActionOn(Player* aPlayer) override;
	void StopPerformingActionOn(Player* aPlayer) override;
	void Interact(Player* aPlayer) override;
	void InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem) override;
	void ClearHeldItem() override { myFood = nullptr; }

private:
	void RemoveFood();
	Ingredient* myFood;
	ProgressBar* myTimer;
};

