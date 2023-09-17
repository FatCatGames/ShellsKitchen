#pragma once


class SettingsMenuComponent : public Component
{
public:
	SettingsMenuComponent();
	~SettingsMenuComponent() = default;
	void Awake() override;
	void Update() override;

	void ShowSettingsMenuButtons();
	void HideSettingsMenuButtons();
	void FadeOutSettingsMenuButtons();
	void FadeInSettingsMenuButtons();
	void ToggleFadeOut(bool aValue) { myShouldFadeOut = aValue; }
	void ToggleFadeIn(bool aValue) { myShouldFadeIn = aValue; }
	void UpdateSelectedButtons();
	void ClickSelectedButton();
private:

	//Members
	float myFadeTime = 0.0f;
	float myTotalFadeTime = 1.0f;
	bool myShouldFadeOut = false;
	bool myShouldFadeIn = false;
};