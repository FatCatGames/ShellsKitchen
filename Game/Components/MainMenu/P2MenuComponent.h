#pragma once
#include "Components/UI/UISprite.h"

class P2MenuComponent : public Component
{
public:
	P2MenuComponent();
	~P2MenuComponent();
	void Awake() override;
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList);
	void RunInEditor() override;
	void ShowP2MenuButtons();
	void HideP2MenuButtons();
	void FadeOutP2MenuButtons();
	void FadeInP2MenuButtons();
	void ToggleFadeOut(bool aValue);
	void ToggleFadeIn(bool aValue);
	bool IsFading() { return myIsFading; }
	void UpdateSelectedButtons();
	void ClickSelectedButton();

private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	float myFadeTime = 0.0f;
	float myTotalFadeTime = 1.0f;
	bool myShouldFadeOut = false;
	bool myShouldFadeIn = false;
	bool myIsFading = false;

}; 
