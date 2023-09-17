#pragma once
#include "Components/UI/UISprite.h"

class P2CustomizeMenuComponent : public Component
{
public:
	P2CustomizeMenuComponent();
	~P2CustomizeMenuComponent();
	void Awake() override;
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList);
	void RunInEditor() override;
	void ShowP2CustomizationMenuButtons();
	void HideP2CustomizationMenuButtons();
	void FadeOutP2CustomizeButtons();
	void FadeInP2CustomizeButtons();
	void ToggleFadeOut(bool aValue);
	void ToggleFadeIn(bool aValue);
	bool IsFading() { return myIsFading; }
	void UpdateSelectedButtons();
	void ClickSelectedButton();

private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	//Members
	float myFadeTime = 0.0f;
	float myTotalFadeTime = 1.0f;
	bool myShouldFadeOut = false;
	bool myShouldFadeIn = false;
	bool myIsFading = false;
};

