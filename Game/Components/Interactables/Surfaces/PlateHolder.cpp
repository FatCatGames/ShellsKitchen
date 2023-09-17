#include "Game.pch.h"
#include "PlateHolder.h"
#include "../Holdables/Plate.h"
#include "../Holdables/Ingredient.h"
#include "../Holdables/Skillet.h"

void PlateHolder::Awake()
{
	InteractableSurface::Awake();

	for (size_t i = 0; i < 4; i++)
	{
		GameObject* plate = InstantiatePrefab("Plate");
		if (plate)
		{
			AddPlate(plate->GetComponent<Plate>());
		}
	}

	CurrentLevel::GetInstance()->SetPlateHolder(this);
}

void PlateHolder::Interact(Player* aPlayer)
{
	auto& plates = myTransform->GetChildren();
	if (plates.empty()) return;
	Plate* plate = plates.back()->GetGameObject()->GetComponent<Plate>();
	aPlayer->SetHeldItem(plate);
}

void PlateHolder::InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem)
{
	if (Plate* plate = dynamic_cast<Plate*>(anitem))
	{
		if (!CanAddPlate(plate)) return;

		if (aPlayer)
		{
			aPlayer->SetHeldItem(nullptr);
		}
		AddPlate(plate);
	}
	else if (Ingredient* food = dynamic_cast<Ingredient*>(anitem))
	{
		if (PlateFood(food))
		{
			if (aPlayer) aPlayer->SetHeldItem(nullptr);
		}
	}
	else if (Skillet* skillet = dynamic_cast<Skillet*>(anitem))
	{
		if (Ingredient* food = skillet->GetFood())
		{
			PlateFood(food);
			skillet->TakeFood();
		}
	}

}

void PlateHolder::AddPlate(Plate* aPlate)
{
	aPlate->SetMyCanInteract(false);
	aPlate->GetTransform()->SetParent(myTransform);
	const float plateSpacing = 0.1f;

	size_t plateCount = myTransform->GetChildren().size();
	if (plateCount > 1 && !myTransform->GetChildren()[plateCount-2]->GetGameObject()->GetComponent<Plate>()->GetIngredients().empty())
	{
		myTransform->SwapChildren(plateCount - 1, plateCount - 2);
	}

	aPlate->GetTransform()->SetLocalPos(Vector3f::up() * static_cast<float>(plateCount - 1) * plateSpacing + Vector3f::up() * 0.3f);
	myTransform->GetChildren().back()->SetLocalPos(Vector3f::up() * static_cast<float>(plateCount) * plateSpacing + Vector3f::up() * 0.3f);
	aPlate->GetTransform()->SetLocalRot(0, 0, 0);
}

bool PlateHolder::CanAddPlate(Plate* aPlate)
{
	size_t plateCount = myTransform->GetChildren().size();
	if (plateCount > 1)
	{
		if (!myTransform->GetChildren().back()->GetGameObject()->GetComponent<Plate>()->GetIngredients().empty()) return false;
	}

	return true;
}

bool PlateHolder::PlateFood(Ingredient* aFood)
{
	if (!aFood) return false;
	auto& plates = myTransform->GetChildren();
	if (!plates.empty())
	{
		if (auto plate = plates.back()->GetGameObject()->GetComponent<Plate>())
		{
			if (!plate->CanPlateIngredient(aFood)) return false;
			plate->PlateIngredient(aFood);
			aFood->UnHighlight();
			return true;
		}
	}
	return false;
}
