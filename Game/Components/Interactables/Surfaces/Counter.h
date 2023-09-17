#pragma once
#include "InteractableSurface.h"

class Counter : public InteractableSurface
{
	void InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem) override;
};