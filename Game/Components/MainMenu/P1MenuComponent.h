#pragma once
#include "Components/UI/UISprite.h"

class P1MenuComponent : public Component
{
public:
	P1MenuComponent();
	~P1MenuComponent();
	void Awake() override;
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList);
	void RunInEditor() override;
	void ShowP1MenuButtons();
	void HideP1MenuButtons();
	void FadeOutP1MenuButtons();
	void FadeInP1MenuButtons();
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
