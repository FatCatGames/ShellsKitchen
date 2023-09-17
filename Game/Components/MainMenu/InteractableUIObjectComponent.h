#pragma once
#include "Components/UI/UISprite.h"

class InteractableUIObjectComponent : public Component
{
public:
	InteractableUIObjectComponent();
	~InteractableUIObjectComponent();
	void Awake() override;
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList);
	void RunInEditor() override;
	//Fade Controls
	void FadeInP1Controls();
	void FadeOutP1Controls();
	void FadeInP2Controls();
	void FadeOutP2Controls();
	void ToggleP1ControlsFadeIn(bool aValue);
	void ToggleP1ControlsFadeOut(bool aValue);
	void ToggleP2ControlsFadeIn(bool aValue);
	void ToggleP2ControlsFadeOut(bool aValue);
	//Fade Credits
	void FadeInP1Credits();
	void FadeOutP1Credits();
	void FadeInP2Credits();
	void FadeOutP2Credits();
	void ToggleP1CreditsFadeIn(bool aValue);
	void ToggleP1CreditsFadeOut(bool aValue);
	void ToggleP2CreditsFadeIn(bool aValue);
	void ToggleP2CreditsFadeOut(bool aValue);


private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	//Fading Controls P1
	bool myFadeInP1Controls = false;
	bool myFadeOutP1Controls = false;
	bool myP1ControlsIsFading = false;
	//Fading Controls P2
	bool myFadeInP2Controls = false;
	bool myFadeOutP2Controls = false;
	bool myP2ControlsIsFading = false;

	//Fade timers Controls
	float myP1FadeTimeControls = 0.0f;
	float myP2FadeTimeControls = 0.0f;
	float myP1TotalFadeTimeControls = 1.0f;
	float myP2TotalFadeTimeControls = 1.0f;

	//Fading Credits P1
	bool myFadeInP1Credits = false;
	bool myFadeOutP1Credits = false;
	bool myP1CreditsIsFading = false;
	//Fading Credits P2
	bool myFadeInP2Credits = false;
	bool myFadeOutP2Credits = false;
	bool myP2CreditsIsFading = false;

	//Fade timers Credits
	float myP1FadeTimeCredits = 0.0f;
	float myP2FadeTimeCredits = 0.0f;
	float myP1TotalFadeTimeCredits = 1.0f;
	float myP2TotalFadeTimeCredits = 1.0f;
};
