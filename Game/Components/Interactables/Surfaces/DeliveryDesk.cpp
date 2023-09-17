#include "Game.pch.h"
#include "DeliveryDesk.h"
#include "..\Holdables\Plate.h"



void DeliveryDesk::Awake()
{
	Interactable::Awake();
	GameManager::GetInstance()->GetCurrentLevelManager().SetDeliveryDesk(this);
}

void DeliveryDesk::InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem)
{
	if (!aPlayer) return;
	if (HoldableItem* item = aPlayer->GetHeldItem())
	{
		if (Plate* plate = dynamic_cast<Plate*>(item))
		{
			if (OrderHandler::GetInstance()->CanDeliverOrder(plate))
			{
				aPlayer->DeliverItem();
			}
		}
	}
}