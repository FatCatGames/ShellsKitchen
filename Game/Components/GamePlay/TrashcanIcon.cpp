#include "Game.pch.h"
#include "TrashcanIcon.h"
#include "Components/Physics/Collisions/Collider.h"
#include "../Game/Managers/GameManager.h"
#include "Components\GamePlay\IngredientIcon.h"

void TrashcanIcon::OnTriggerEnter(Collider* aCollider)
{
	if(aCollider->GetCollisionLayer() == 1)
	{
		if(myTransform->GetChildren().size() > 0)
		{
			myTransform->GetChildren()[0]->GetGameObject()->SetActive(true);
			myGameObject->GetComponent<IngredientIcon>()->SetOffset({ 0.0f, 0.15f, 0.0f });
		}
	}
}

void TrashcanIcon::OnTriggerExit(Collider* aCollider)
{
	if(aCollider->GetCollisionLayer() == 1)
	{
		if(myTransform->GetChildren().size() > 0)
		{
			myTransform->GetChildren()[0]->GetGameObject()->SetActive(false);
		}
	}
}