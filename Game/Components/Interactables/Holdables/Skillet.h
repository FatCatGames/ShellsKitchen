#pragma once

class ProgressBar;
class Ingredient;
class ParticleSystem;
class BurnIndicator;
class Skillet : public HoldableItem
{
public:
	void Awake() override;
	void Update() override;
	void Cook();
	void StartCooking();
	void StopCooking();
	void InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem) override;
	bool AddFood(Ingredient* aFood);
	void OnPickup() override;
	Ingredient* TakeFood();
	Ingredient* GetFood() { return myFood; }
	virtual bool GetIsPot() { return false; }

protected:
	float myBurnTimer = 0.f;
	bool myIsCooking = false;
	bool myIsPlayingWarningSound = false;
	Ingredient* myFood;
	ParticleSystem* mySteam;
	ParticleSystem* mySteamTrail;
	ParticleSystem* mySmoke;
	ProgressBar* myTimer;
	BurnIndicator* myBurnIndicator;
	AudioMonoComponent* myAudioComponent = nullptr;
};