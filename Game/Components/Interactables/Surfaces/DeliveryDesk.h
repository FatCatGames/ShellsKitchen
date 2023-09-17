#pragma once
#include "..\Interactable.h"

class DeliveryDesk : public Interactable
{
public:
	void Awake() override;
	void InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem) override;
};