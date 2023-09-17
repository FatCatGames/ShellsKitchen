#pragma once
#include "..\Interactable.h"
#include "IngredientTypes.h"

class IngredientContainer : public Interactable
{
public:
	void Interact(Player* aPlayer) override;
	void InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem) override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	bool HasItem() override;

private:
	IngredientType myIngredient;
};