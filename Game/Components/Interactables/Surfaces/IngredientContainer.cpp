#include "Game.pch.h"
#include "IngredientContainer.h"
#include "..\Holdables\Ingredient.h"
#include "..\Holdables\Plate.h"
#include "..\Holdables\Pot.h"

void IngredientContainer::Interact(Player* aPlayer)
{
	print("Picked up " + IngredientTypeToName(myIngredient));
	GameObject* newIngredient = InstantiatePrefab(IngredientTypeToName(myIngredient));
	Ingredient* ingredient = newIngredient->GetComponent<Ingredient>();
	aPlayer->SetHeldItem(ingredient);
}

void IngredientContainer::InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem)
{
	if (myIngredient == IngredientType::Seaweed || myIngredient == IngredientType::Caviar)
	{
		if (Plate* plate = dynamic_cast<Plate*>(anitem))
		{
			if (plate->HasIngredientType(myIngredient)) return;
			GameObject* newIngredient = InstantiatePrefab(IngredientTypeToName(myIngredient));
			Ingredient* ingredient = newIngredient->GetComponent<Ingredient>();
			plate->PlateIngredient(ingredient);
		}
	}
	else if (myIngredient == IngredientType::Rice)
	{
		if (Pot* pot = dynamic_cast<Pot*>(anitem))
		{
			if (pot->GetFood()) return;
			GameObject* newIngredient = InstantiatePrefab(IngredientTypeToName(myIngredient));
			Ingredient* ingredient = newIngredient->GetComponent<Ingredient>();
			pot->AddFood(ingredient);
		}
	}
}

void IngredientContainer::RenderInProperties(std::vector<Component*>& aComponentList)
{
	vector<string> ingredientNamesStr(IngredientType::COUNT);
	vector<const char*> ingredientNames(IngredientType::COUNT);
	for (size_t i = 0; i < IngredientType::COUNT; i++)
	{
		std::string ingredientName = IngredientTypeToName(static_cast<IngredientType>(i));
		ingredientNamesStr[i] = ingredientName;
		ingredientNames[i] = ingredientNamesStr[i].c_str();
	}

	int ingredientIndex = static_cast<int>(myIngredient);
	if (ImGui::Combo("Ingredient", &ingredientIndex, ingredientNames.data(), static_cast<int>(IngredientType::COUNT)))
	{
		myIngredient = static_cast<IngredientType>(ingredientIndex);
	}
}


void IngredientContainer::Save(rapidjson::Value& /*aComponentData*/)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();
	wrapper.SaveValue<DataType::Int>("Type", myIngredient);
}

void IngredientContainer::Load(rapidjson::Value& aComponentData)
{
	myIngredient = static_cast<IngredientType>(aComponentData["Type"].GetInt());
}

bool IngredientContainer::HasItem()
{
	return true;
}