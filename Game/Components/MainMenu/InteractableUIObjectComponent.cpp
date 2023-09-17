#include "Game.pch.h"
#include "InteractableUIObjectComponent.h"
#include <Shaders/UI/ErodingMenu/UIErodingMenuPS.h>

InteractableUIObjectComponent::InteractableUIObjectComponent()
{

	if (GameManager::GetInstance()->GetMyHatsVector().size() == 0)
	{
		GameManager::GetInstance()->LoadHatsToVector();

	}
}

InteractableUIObjectComponent::~InteractableUIObjectComponent()
{
	UIManager::GetInstance()->ClearUIManager();
}

void InteractableUIObjectComponent::Awake()
{
	UIManager::GetInstance()->SetP1IsInMainMenu(true);
	UIManager::GetInstance()->SetP2IsInMainMenu(true);
	UIManager::GetInstance()->AddInteractableUIObjectComponent(this);
}

void InteractableUIObjectComponent::Update()
{
	FadeInP1Controls();
	FadeOutP1Controls();
	FadeInP2Controls();
	FadeOutP2Controls();
	FadeInP1Credits();
	FadeOutP1Credits();
	FadeInP2Credits();
	FadeOutP2Credits();

}

void InteractableUIObjectComponent::RenderInProperties(std::vector<Component*>& aComponentList)
{
}

void InteractableUIObjectComponent::RunInEditor()
{
}

void InteractableUIObjectComponent::FadeInP1Controls()
{
	if (myFadeInP1Controls)
	{
		myP1FadeTimeControls += realDeltaTime;
		const float Percentage = myP1FadeTimeControls / myP1TotalFadeTimeControls;
		float Alpha = Catbox::Lerp(0.0f, 1.0f, Percentage);

		auto ErodingShader = dynamic_cast<UIErodingMenuPS*>(myTransform->GetChildren()[7]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
		if (ErodingShader)
		{
			ErodingShader->myUIErodingMenuData.Resource = Alpha;
			ErodingShader->SetData();
		}
	}
	if (myP1FadeTimeControls > myP1TotalFadeTimeControls)
	{
		myP1FadeTimeControls = 0.0f;
		myFadeInP1Controls = false;
		myP1ControlsIsFading = false;		
	}
}

void InteractableUIObjectComponent::FadeOutP1Controls()
{
	if (myFadeOutP1Controls)
	{
		myP1FadeTimeControls += realDeltaTime;
		const float Percentage = myP1FadeTimeControls / myP1TotalFadeTimeControls;
		float Alpha = Catbox::Lerp(1.0f, 0.0f, Percentage);

		auto ErodingShader = dynamic_cast<UIErodingMenuPS*>(myTransform->GetChildren()[7]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
		if (ErodingShader)
		{
			ErodingShader->myUIErodingMenuData.Resource = Alpha;
			ErodingShader->SetData();
		}


	}
	if (myP1FadeTimeControls > myP1TotalFadeTimeControls)
	{
		UIManager::GetInstance()->GetInteractableUIObjectComponent()->GetTransform()->GetChildren()[7]->GetGameObject()->SetActive(false);
		myP1FadeTimeControls = 0.0f;
		myFadeOutP1Controls = false;
		myP1ControlsIsFading = false;
		UIManager::GetInstance()->SetP1IsInControls(false);
	}
}

void InteractableUIObjectComponent::FadeInP2Controls()
{
	if (myFadeInP2Controls)
	{
		myP2FadeTimeControls += realDeltaTime;
		const float Percentage = myP2FadeTimeControls / myP2TotalFadeTimeControls;
		float Alpha = Catbox::Lerp(0.0f, 1.0f, Percentage);

		auto ErodingShader = dynamic_cast<UIErodingMenuPS*>(myTransform->GetChildren()[8]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
		if (ErodingShader)
		{
			ErodingShader->myUIErodingMenuData.Resource = Alpha;
			ErodingShader->SetData();
		}
	}
	if (myP2FadeTimeControls > myP2TotalFadeTimeControls)
	{
		myP2FadeTimeControls = 0.0f;
		myFadeInP2Controls = false;
		myP2ControlsIsFading = false;
	}
}

void InteractableUIObjectComponent::FadeOutP2Controls()
{

	if (myFadeOutP2Controls)
	{
		myP2FadeTimeControls += realDeltaTime;
		const float Percentage = myP2FadeTimeControls / myP2TotalFadeTimeControls;
		float Alpha = Catbox::Lerp(1.0f, 0.0f, Percentage);

		auto ErodingShader = dynamic_cast<UIErodingMenuPS*>(myTransform->GetChildren()[8]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
		if (ErodingShader)
		{
			ErodingShader->myUIErodingMenuData.Resource = Alpha;
			ErodingShader->SetData();
		}
	}
	if (myP2FadeTimeControls > myP2TotalFadeTimeControls)
	{
		UIManager::GetInstance()->GetInteractableUIObjectComponent()->GetTransform()->GetChildren()[8]->GetGameObject()->SetActive(false);
		myP2FadeTimeControls = 0.0f;
		myFadeOutP2Controls = false;
		myP2ControlsIsFading = false;
		UIManager::GetInstance()->SetP2IsInControls(false);
	}
}

void InteractableUIObjectComponent::ToggleP1ControlsFadeIn(bool aValue)
{
	if (!myP1ControlsIsFading)
	{
		myFadeInP1Controls = aValue;
		myP1FadeTimeControls = 0.0f;
		//myP1ControlsIsFading = true;
	}
}

void InteractableUIObjectComponent::ToggleP1ControlsFadeOut(bool aValue)
{
	if (!myP1ControlsIsFading)
	{
		myFadeOutP1Controls = aValue;
		myP1FadeTimeControls = 0.0f;
		myP1ControlsIsFading = true;
	}
}

void InteractableUIObjectComponent::ToggleP2ControlsFadeIn(bool aValue)
{
	if (!myP2ControlsIsFading)
	{
		myFadeInP2Controls = aValue;
		myP2FadeTimeControls = 0.0f;
		//myP2ControlsIsFading = true;
	}
}

void InteractableUIObjectComponent::ToggleP2ControlsFadeOut(bool aValue)
{
	if (!myP2ControlsIsFading)
	{
		myFadeOutP2Controls = aValue;
		myP2FadeTimeControls = 0.0f;
		myP2ControlsIsFading = true;
	}
}

void InteractableUIObjectComponent::FadeInP1Credits()
{
	if (myFadeInP1Credits)
	{
		myP1FadeTimeCredits += realDeltaTime;
		const float Percentage = myP1FadeTimeCredits / myP1TotalFadeTimeCredits;
		float Alpha = Catbox::Lerp(0.0f, 1.0f, Percentage);

		auto ErodingShader = dynamic_cast<UIErodingMenuPS*>(myTransform->GetChildren()[6]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
		if (ErodingShader)
		{
			ErodingShader->myUIErodingMenuData.Resource = Alpha;
			ErodingShader->SetData();
		}
	}
	if (myP1FadeTimeCredits > myP1TotalFadeTimeCredits)
	{
		myP1FadeTimeCredits = 0.0f;
		myFadeInP1Credits = false;
		myP1CreditsIsFading = false;
	}
}

void InteractableUIObjectComponent::FadeOutP1Credits()
{
	if (myFadeOutP1Credits)
	{
		myP1FadeTimeCredits += realDeltaTime;
		const float Percentage = myP1FadeTimeCredits / myP1TotalFadeTimeCredits;
		float Alpha = Catbox::Lerp(1.0f, 0.0f, Percentage);

		auto ErodingShader = dynamic_cast<UIErodingMenuPS*>(myTransform->GetChildren()[6]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
		if (ErodingShader)
		{
			ErodingShader->myUIErodingMenuData.Resource = Alpha;
			ErodingShader->SetData();
		}


	}
	if (myP1FadeTimeCredits > myP1TotalFadeTimeCredits)
	{
		UIManager::GetInstance()->GetInteractableUIObjectComponent()->GetTransform()->GetChildren()[6]->GetGameObject()->SetActive(false);
		myP1FadeTimeCredits = 0.0f;
		myFadeOutP1Credits = false;
		myP1CreditsIsFading = false;
		UIManager::GetInstance()->SetP1IsInCredits(false);
	}
}

void InteractableUIObjectComponent::FadeInP2Credits()
{
	if (myFadeInP2Credits)
	{
		myP2FadeTimeCredits += realDeltaTime;
		const float Percentage = myP2FadeTimeCredits / myP2TotalFadeTimeCredits;
		float Alpha = Catbox::Lerp(0.0f, 1.0f, Percentage);

		auto ErodingShader = dynamic_cast<UIErodingMenuPS*>(myTransform->GetChildren()[5]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
		if (ErodingShader)
		{
			ErodingShader->myUIErodingMenuData.Resource = Alpha;
			ErodingShader->SetData();
		}
	}
	if (myP2FadeTimeCredits > myP2TotalFadeTimeCredits)
	{
		myP2FadeTimeCredits = 0.0f;
		myFadeInP2Credits = false;
		myP2CreditsIsFading = false;
	}
}

void InteractableUIObjectComponent::FadeOutP2Credits()
{
	if (myFadeOutP2Credits)
	{
		myP2FadeTimeCredits += realDeltaTime;
		const float Percentage = myP2FadeTimeCredits / myP2TotalFadeTimeCredits;
		float Alpha = Catbox::Lerp(1.0f, 0.0f, Percentage);

		auto ErodingShader = dynamic_cast<UIErodingMenuPS*>(myTransform->GetChildren()[5]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
		if (ErodingShader)
		{
			ErodingShader->myUIErodingMenuData.Resource = Alpha;
			ErodingShader->SetData();
		}
	}
	if (myP2FadeTimeCredits > myP2TotalFadeTimeCredits)
	{
		UIManager::GetInstance()->GetInteractableUIObjectComponent()->GetTransform()->GetChildren()[5]->GetGameObject()->SetActive(false);
		myP2FadeTimeCredits = 0.0f;
		myFadeOutP2Credits = false;
		myP2CreditsIsFading = false;
		UIManager::GetInstance()->SetP2IsInCredits(false);
	}
}

void InteractableUIObjectComponent::ToggleP1CreditsFadeIn(bool aValue)
{
	if (!myP1CreditsIsFading)
	{
		myFadeInP1Credits = aValue;
		myP1FadeTimeCredits = 0.0f;
		//myP1CreditsIsFading = true;
	}
}

void InteractableUIObjectComponent::ToggleP1CreditsFadeOut(bool aValue)
{
	if (!myP1CreditsIsFading)
	{
		myFadeOutP1Credits = aValue;
		myP1FadeTimeCredits = 0.0f;
		myP1CreditsIsFading = true;
	}
}

void InteractableUIObjectComponent::ToggleP2CreditsFadeIn(bool aValue)
{
	if (!myP2CreditsIsFading)
	{
		myFadeInP2Credits = aValue;
		myP2FadeTimeCredits = 0.0f;
		//myP2CreditsIsFading = true;
	}
}

void InteractableUIObjectComponent::ToggleP2CreditsFadeOut(bool aValue)
{
	if (!myP2CreditsIsFading)
	{
		myFadeOutP2Credits = aValue;
		myP2FadeTimeCredits = 0.0f;
		myP2CreditsIsFading = true;
	}
}

void InteractableUIObjectComponent::Save(rapidjson::Value& aComponentData)
{

}

void InteractableUIObjectComponent::Load(rapidjson::Value& aComponentData)
{

}
