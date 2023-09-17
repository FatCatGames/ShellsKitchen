#include "Game.pch.h"
#include "CuttingBoard.h"
#include "..\Holdables\Ingredient.h"
#include "..\Holdables\Skillet.h"
#include "..\Holdables\Plate.h"
#include "Components\ProgressBar.h"

void CuttingBoard::Awake()
{
	InteractableSurface::Awake();

	for (auto& child : myTransform->GetChildren())
	{
		if (child->GetGameObject()->GetName() == "CookTimer")
		{
			myTimer = child->GetGameObject()->GetComponent<ProgressBar>();
		}
	}
}

void CuttingBoard::BeginPerformingActionOn(Player* aPlayer)
{
	if (myFood && !myFood->GetIsCut())
	{
		myIsBeingUsed = true;
		myTimer->GetGameObject().SetActive(true);
		aPlayer->SetAnimationBool("Chopping", true);
	}
	else aPlayer->FinishAction();
}


void CuttingBoard::PerformActionOn(Player* aPlayer)
{
	if (myFood->Cut())
	{
		myTimer->GetGameObject().SetActive(false);
		StopPerformingActionOn(aPlayer);
	}

	myTimer->SetProgress(myFood->GetCutPercent());
}

void CuttingBoard::StopPerformingActionOn(Player* aPlayer)
{
	myIsBeingUsed = false;
	//myTimer->GetGameObject().SetActive(false);
	aPlayer->SetAnimationBool("Chopping", false);
	aPlayer->FinishAction();
}

void CuttingBoard::Interact(Player* aPlayer)
{
	if (myIsBeingUsed) return;
	if (myFood)
	{
		StopPerformingActionOn(aPlayer);
		myTimer->GetGameObject().SetActive(false);
		myFood = nullptr;
	}

	InteractableSurface::Interact(aPlayer);
}


void CuttingBoard::InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem)
{
	if (myIsBeingUsed) return;
	if (myFood)
	{
		if (Skillet* skillet = dynamic_cast<Skillet*>(anitem))
		{
			if (skillet->AddFood(myFood))
			{
				RemoveFood();
			}
		}
		else if (Plate* plate = dynamic_cast<Plate*>(anitem))
		{
			if (!plate->CanPlateIngredient(myFood)) return;
			plate->PlateIngredient(myFood);
			RemoveFood();
		}
	}
	else
	{
		if (Ingredient* ingredient = dynamic_cast<Ingredient*>(anitem))
		{
			if (!ingredient->CanBeChopped()) return;
			if (aPlayer) aPlayer->SetHeldItem(nullptr);
			myFood = ingredient;
			myHeldItem = anitem;

			if (!myFood->GetIsCut()) 
			{
				myTimer->GetGameObject().SetActive(true);
				myTimer->SetProgress(myFood->GetCutPercent());
			}

			myFood->Highlight();
			myFood->GetTransform()->SetParent(myTransform);
			myFood->GetTransform()->SetLocalRot(0, 0, 0);
			myFood->GetTransform()->SetLocalPos(Vector3f::up() * 0.4f);
		}
	}
}

void CuttingBoard::RemoveFood()
{
	myTimer->GetGameObject().SetActive(false);
	myTimer->SetProgress(myFood->GetCutPercent());
	myFood = nullptr;
	myHeldItem = nullptr;
}
