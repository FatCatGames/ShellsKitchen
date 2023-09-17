#include "Game.pch.h"
#include "Plate.h"
#include "Ingredient.h"
#include "Skillet.h"
#include "Components\3D\ModelInstance.h"
#include "Components\Interactables\Surfaces\PlateHolder.h"
#include "Components\GamePlay\IngredientIcon.h"

void Plate::InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem)
{
	if (myIsInTransit) return;

	if (Skillet* skillet = dynamic_cast<Skillet*>(anitem))
	{
		if (Ingredient* ingredient = skillet->GetFood())
		{
			if (!CanPlateIngredient(ingredient)) return;
			PlateIngredient(ingredient);
			skillet->TakeFood();
		}
	}
	else if (Ingredient* ingredient = dynamic_cast<Ingredient*>(anitem))
	{
		if (!CanPlateIngredient(ingredient)) return;
		if (aPlayer) aPlayer->SetHeldItem(nullptr);
		PlateIngredient(ingredient);
	}
}

void Plate::Update()
{
	if (myIsInTransit)
	{
		myTimeInTransit += deltaTime;
		return;
	}

	HoldableItem::Update();

	//Respawn
	if (myTransform->worldPos().y < -5.5f)
	{
		auto plateHolder = GameManager::GetInstance()->GetCurrentLevelManager().GetPlateHolder();
		if (plateHolder)
		{
			auto rb = myGameObject->GetComponent<RigidBody>();
			if (rb)
			{
				myGameObject->RemoveComponent(rb);
				myIsInTransit = false;
				Clear();
				plateHolder->AddPlate(this);
				auto smoke = InstantiatePrefab("RespawnSmokePoof");
				smoke->GetTransform()->SetWorldPos(myTransform->worldPos());
			}
		}
	}

	//if(myShouldChangeIngredientIcon == true)
	//{
	//	myShouldChangeIngredientIcon = false;
	//	OffsetIngredientIcons();
	//}
}

void Plate::PlateIngredient(Ingredient* anIngredient)
{
	myIngredients.push_back(anIngredient);

	anIngredient->SetIconActive(true);

	auto& recipes = OrderHandler::GetInstance()->GetRecipeList();
	for (auto& recipe : recipes)
	{
		if (OrderHandler::GetInstance()->RecipeMatch(recipe, myIngredients))
		{
			auto dish = InstantiatePrefab(recipe.GetPrefabName());
			if (dish)
			{
				for (auto& ingredient : myIngredients)
				{
					ingredient->HideMesh();
				}

				dish->GetTransform()->SetParent(myTransform);
				dish->GetTransform()->Reset();
			}
			break;
		}
	}

	myHeldItem = anIngredient;

	if (!myIsBeingHeld)
	{
		anIngredient->Highlight();
	}
	else anIngredient->UnHighlight();

	anIngredient->GetTransform()->SetParent(myTransform);
	anIngredient->GetTransform()->SetLocalPos(Vector3f::zero());

	myShouldChangeIngredientIcon = true;
	OffsetIngredientIcons();

	//this code is so fucking hard rn...
	if (anIngredient->CanBeBoiled())
	{
		anIngredient->SetCookedMesh();
	}
}

bool Plate::HasIngredientType(IngredientType aType)
{
	for (auto& ingredient : myIngredients)
	{
		if (ingredient->GetIngredientType() == aType) return true;
	}

	return false;
}

bool Plate::CanPlateIngredient(Ingredient* anIngredient)
{
	if (anIngredient->GetIsUnprocessed()) return false;
	if (HasIngredientType(anIngredient->GetIngredientType())) return false;
	return true;
}

void Plate::Clear()
{
	for (auto& child : myTransform->GetChildren())
	{
		child->GetGameObject()->Destroy();
	}

	myHeldItem = nullptr;
	myIngredients.clear();
}

void Plate::SetIsInTransit(bool aIsInTransit)
{
	myIsInTransit = aIsInTransit;
	myTimeInTransit = 0;
	myGameObject->GetComponent<ModelInstance>()->SetEnabled(!aIsInTransit);
}

void Plate::OffsetIngredientIcons()
{
	if (myIngredients.size() == 1)
	{
		myIngredients[0]->GetGameObject().GetComponent<IngredientIcon>()->SetOffset(Vector3f{ 0.0f, 0.1f, 0.0f });
		return;
	}

	const std::vector<Vector3f> offsets = {
	{-0.025f, 0.08f, 0.0f},
	{0.025f, 0.08f, 0.0f},
	{-0.025f, 0.16f, 0.0f},
	{0.025f, 0.16f, 0.0f}
	};

	for (int i = 0; i < myIngredients.size(); i++)
	{
		IngredientIcon* icon = myIngredients[i]->GetGameObject().GetComponent<IngredientIcon>();

		if (icon != nullptr)
		{
			icon->SetOffset(offsets[i]);
		}
	}
}
