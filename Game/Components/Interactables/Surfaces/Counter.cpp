#include "Game.pch.h"
#include "Counter.h"
#include "../Holdables/Skillet.h"
#include "../Holdables/Plate.h"
#include "../Holdables/Ingredient.h"

void Counter::InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem)
{
	if (myHeldItem) 
	{
		if (Ingredient* food = dynamic_cast<Ingredient*>(myHeldItem))
		{
			if (Plate* plate = dynamic_cast<Plate*>(anitem))
			{
				if (!plate->CanPlateIngredient(food)) return;
				plate->PlateIngredient(food);
				myHeldItem = nullptr;
			}
			else if (Skillet* skillet = dynamic_cast<Skillet*>(anitem))
			{
				if (skillet->AddFood(food))
				{
					myHeldItem = nullptr;
				}
			}
		}
		else
		{
			InteractableSurface::InteractWithHeldItem(aPlayer, anitem);
		}
	}
	else 
	{
		InteractableSurface::InteractWithHeldItem(aPlayer, anitem);
	}
}
