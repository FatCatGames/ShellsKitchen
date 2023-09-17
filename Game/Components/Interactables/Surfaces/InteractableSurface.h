#pragma once

class InteractableSurface : public Interactable
{
public:
	void Awake() override;
	void Interact(Player* aPlayer) override;
	void InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem) override;
	virtual void ClearHeldItem() { myHeldItem = nullptr; };

protected:
	
};