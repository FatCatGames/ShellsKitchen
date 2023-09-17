#pragma once
#include "Components\Player\Player.h"
class ModelInstance;
class HighlightPS;
class Player;
class Interactable : public Component
{

public:
	~Interactable();
	void Awake() override;
	void Update() override;
	virtual void Interact(Player* aPlayer) {}
	virtual void InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem) {}
	virtual void BeginPerformingActionOn(Player* aPlayer) {}
	virtual void PerformActionOn(Player* aPlayer) { aPlayer->FinishAction(); }
	virtual void StopPerformingActionOn(Player* aPlayer) {}
	virtual void StopBeingHeld(Player* aPlayer) {}
	virtual void Highlight();
	virtual void UnHighlight();
	void UnHighlightInstant();
	virtual bool HasItem() { return myHeldItem; }
	void SetMyCanInteract(const bool& aInteractFlag);
	bool GetCanInteract() { return myCanInteract; }
	// Disgusting Elric code
	Player* GetPlayerThrowing() { return myPlayerThrowing; }
	void SetPlayerThrowing(Player* aPlayer) { myPlayerThrowing = aPlayer; }
	bool GetIsBeingHeld() { return myIsBeingHeld; }
	void SetIsBeingHeld(bool anIsBeingHeld) { myIsBeingHeld = anIsBeingHeld; }
	bool GetIsBeingThrown() { return myIsBeingThrown; }
	void SetIsBeingThrown(bool anIsBeingThrown) { myIsBeingThrown = anIsBeingThrown; }
	bool GetIsBeingUsed() { return myIsBeingUsed; }
	int GetInteractPriority() { return myInteractPriority; }
	//
	void SetmyHeldItem(HoldableItem& aHoldableItem);
	HoldableItem* GetMyHeldItem();


protected:
	HoldableItem* myHeldItem;
	void CreateInteractableShader();
	Player* myPlayerThrowing = nullptr;
	bool myIsBeingHeld = false;
	bool myIsBeingThrown = false;
	bool myIsBeingUsed = false;
	std::vector<HighlightPS*> myHighlightPS;
	ModelInstance* myModel;

	const float myHighlightStrength = 2;
	const float myHighlightLerpTime = 0.1f;
	bool myIsLerpingIn = false;
	float myHighlightTimer = 0;
	int myInteractPriority = 0;
	bool myCanInteract = true;
};