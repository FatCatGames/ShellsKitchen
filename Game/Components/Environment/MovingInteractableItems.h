#pragma once
#include "Components/Interactables/Holdables/Ingredient.h"
class MovingInteractableItems : public Ingredient
{
public:
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	virtual void Awake() override;
private:

};