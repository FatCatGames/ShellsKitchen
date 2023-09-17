#include "Game.pch.h"
#include "PauseMenuComponent.h"
#include "Components/MainMenu/MenuButton.h"
#include <Shaders/UI/ErodingMenu/UIErodingMenuPS.h>

PauseMenuComponent::PauseMenuComponent()
{

}

void PauseMenuComponent::Awake()
{
	UIManager::GetInstance()->ClearUIManager();
	UIManager::GetInstance()->AddPauseMenuComponent(this);
	for (auto& button : myTransform->GetChildren())
	{
		UIManager::GetInstance()->AddPauseMenuButtonToManager(button->GetGameObject()->GetComponent<MenuButton>());
		button->GetGameObject()->SetActive(false);
		button->GetGameObject()->GetComponent<UISprite>()->SetAlpha(0);
	}
}

void PauseMenuComponent::Update()
{
	FadeOutPauseMenuButtons();
	FadeInPauseMenuButtons();
}

void PauseMenuComponent::Init()
{
}

void PauseMenuComponent::ShowPauseMenuButtons()
{
	for (auto& button : myTransform->GetChildren())
	{
		button->GetGameObject()->SetActive(true);
	}
}

void PauseMenuComponent::HidePauseMenuButtons()
{
	for (auto& button : myTransform->GetChildren())
	{
		button->GetGameObject()->SetActive(false);
	}
}

void PauseMenuComponent::FadeOutPauseMenuButtons()
{
	if (myShouldFadeOut)
	{
		myFadeTime += realDeltaTime;
		const float Percentage = myFadeTime / myTotalFadeTime;
		float Alpha = Catbox::Lerp(1.0f, 0.0f, Percentage);
		for (auto& button : myTransform->GetChildren())
		{
			button->GetGameObject()->GetComponent<UISprite>()->SetAlpha(Alpha);
			for (size_t i = 0; i < button->GetChildren().size(); i++)
			{
				auto ErodingShader = dynamic_cast<UIErodingMenuPS*>(button->GetChildren()[i]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
				if (ErodingShader)
				{
					ErodingShader->myUIErodingMenuData.Resource = Alpha;
					ErodingShader->SetData();
				}
			}
		}
	}
	if (myFadeTime > myTotalFadeTime)
	{
		myFadeTime = 0.0f;
		myShouldFadeOut = false;
		myIsFading = false;
		HidePauseMenuButtons();
	}
}

void PauseMenuComponent::FadeInPauseMenuButtons()
{
	if (myShouldFadeIn)
	{
		ShowPauseMenuButtons();
		myFadeTime += realDeltaTime;
		const float Percentage = myFadeTime / myTotalFadeTime;
		float Alpha = Catbox::Lerp(0.0f, 1.0f, Percentage);
		for (auto& button : myTransform->GetChildren())
		{
			button->GetGameObject()->GetComponent<UISprite>()->SetAlpha(Alpha);
			for (size_t i = 0; i < button->GetChildren().size(); i++)
			{
				auto ErodingShader = dynamic_cast<UIErodingMenuPS*>(button->GetChildren()[i]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
				if (ErodingShader)
				{
					ErodingShader->myUIErodingMenuData.Resource = Alpha;
					ErodingShader->SetData();
				}
			}
		}
	}
	if (myFadeTime > myTotalFadeTime)
	{
		myFadeTime = 0.0f;
		myShouldFadeIn = false;
		myIsFading = false;
	}
}

void PauseMenuComponent::UpdateSelectedButtons()
{
	if (UIManager::GetInstance()->GetIsInPauseMenu())
	{
		int currentBtnId = UIManager::GetInstance()->GetPauseMenuIndex();
		for (size_t i = 0; i < myTransform->GetChildren().size(); i++)
		{
			UIManager::GetInstance()->GetPauseMenuButtons()[i]->OnMouseExit();
		}
		UIManager::GetInstance()->GetPauseMenuButtons()[currentBtnId]->OnMouseEnter();
	}

}

void PauseMenuComponent::ClickSelectedButton()
{
	int currentBtnId = UIManager::GetInstance()->GetPauseMenuIndex();
	UIManager::GetInstance()->GetPauseMenuButtons()[currentBtnId]->OnClick();
}
