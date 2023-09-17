#include "Game.pch.h"
#include "MovingInteractableItems.h"
#include "Components/Physics/Collisions/BoxCollider.h"
void MovingInteractableItems::Awake()
{
	Ingredient::SetType(IngredientType::Fish);
	Ingredient::Awake();
}

void MovingInteractableItems::RenderInProperties(std::vector<Component*>& aComponentList)
{

}