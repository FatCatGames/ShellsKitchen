#pragma once
#include "IngredientTypes.h"

class Ingredient : public HoldableItem
{
public:
	void Update() override;
	void Awake() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	bool Cook();
	bool Boil();
	bool Cut();

	IngredientType GetIngredientType() { return myType; }
	bool GetIsThrowable() override { return true; }
	bool GetIsCooked() { return myIsCooked; }
	bool GetIsBoiled() { return myIsBoiled; }
	bool GetIsBurnt() { return myIsBurnt; }
	bool GetIsCut() { return myIsCut; }
	bool CanBeBoiled() { return myCanBeBoiled; }
	bool CanBeFried();
	bool CanBeChopped() { return myCanBeChopped; }
	bool GetIsUnprocessed();
	void SetBoilingMesh();
	void SetCookedMesh();
	void SetIconActive(bool aFlag);
	void HideMesh();

	float GetCookPercent();
	float GetBurnPercent() { return myBurnTimer / myTargetCookTime; }
	float GetCutPercent() { return myCutTimer / myTargetCutTime; }

	bool GetIsHeld();
	void SetType(const IngredientType& aType);
	void SetFoodTrailState(bool aShouldPlay);
	void StopBeingHeld(Player* aPlayer) override { SetFoodTrailState(false); }
	void ResetBurn() { myBurnTimer = 0; }
	void ResetSplash();

private:
	ParticleSystem* myFoodTrail;
	bool myTrailIsPlaying = false;

	IngredientType myType;
	bool myCanBeBoiled = false;
	bool myCanBeFried = true;
	bool myCanBeChopped = true;

	bool myIsCooked = false;
	bool myIsBoiled = false;
	bool myIsBurnt = false;
	float myBurnTimer = 0;
	float myCookTimer = 0;
	float myTargetCookTime = 8;

	float myBoilTimer = 0;
	float myTargetBoilTime = 8;
	
	bool myIsCut = false;
	float myCutTimer = 0;
	float myTargetCutTime = 2;

	bool myHasSplashed = false;
};