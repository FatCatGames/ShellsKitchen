#pragma once


class PauseMenuComponent : public Component
{
public:
	PauseMenuComponent();
	~PauseMenuComponent() = default;
	void Awake() override;
	void Update() override;
	void Init();
	void ShowPauseMenuButtons();
	void HidePauseMenuButtons();
	void FadeOutPauseMenuButtons();
	void FadeInPauseMenuButtons();
	void ToggleFadeOut(bool aValue) {myShouldFadeOut = aValue; myIsFading= true;}
	void ToggleFadeIn(bool aValue) { myShouldFadeIn = aValue; myIsFading = true; }
	void UpdateSelectedButtons();
	void ClickSelectedButton();
	bool IsFading() { return myIsFading; }

private:


	//Members
	float myFadeTime = 0.0f;
	float myTotalFadeTime = 0.5f;
	bool myShouldFadeOut = false;
	bool myShouldFadeIn = false;
	bool myIsFading = false;
};