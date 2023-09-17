#pragma once
#include "InteractableSurface.h"

class Skillet;
class Stove : public InteractableSurface
{
public:
	void Awake() override;
	void Interact(Player* aPlayer) override;
	void InteractWithHeldItem(Player* aPlayer, HoldableItem* anItem) override;
	void Update() override;
	void ClearHeldItem() override { mySkillet = nullptr; }
	void AddSkillet(Skillet* aSkillet);
private:
	Skillet* mySkillet;

};