#include "Game.pch.h"
#include "InteractableSurface.h"
#include "..\Holdables\HoldableItem.h"
#include "Shaders\Interaction\HighlightPS.h"

void InteractableSurface::Awake()
{
	Interactable::Awake();

	for (auto child : myTransform->GetChildren())
	{
		if (myHeldItem = child->GetGameObject()->GetComponent<HoldableItem>())
		{
			//myHeldItem->SetEnabled(false);
			break;
		}
	}
}

void InteractableSurface::Interact(Player* aPlayer)
{
	if (myHeldItem)
	{
		if (!aPlayer->GetHeldItem())
		{
			aPlayer->SetHeldItem(myHeldItem);
			myHeldItem = nullptr;
			ClearHeldItem();
		}
	}
}

void InteractableSurface::InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem)
{
	if (!myHeldItem)
	{
		if (aPlayer) aPlayer->SetHeldItem(nullptr);
		anitem->GetTransform()->SetParent(myTransform);
		anitem->GetTransform()->SetLocalPos(Vector3f::up() * 0.4f);
		anitem->GetTransform()->SetLocalRot(Vector3f::up() * Catbox::GetRandom(-10.f, 10.f));
		myHeldItem = anitem;
		if (aPlayer) 
		{
			myHeldItem->Highlight();
		}
	}
	else
	{
		myHeldItem->InteractWithHeldItem(aPlayer, anitem);
	}
}
