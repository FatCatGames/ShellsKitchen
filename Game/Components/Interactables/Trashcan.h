#pragma once

class Trashcan : public Interactable
{
public:
	void InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem) override;

private:
};