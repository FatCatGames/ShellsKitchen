#include "Game.pch.h"
#include "P1CustomizeMenuComponent.h"
#include "MenuButton.h"
#include <Shaders/UI/ErodingMenu/UIErodingMenuPS.h>
#include "P1MenuComponent.h"

P1CustomizeMenuComponent::P1CustomizeMenuComponent()
{

}

P1CustomizeMenuComponent::~P1CustomizeMenuComponent()
{
}

void P1CustomizeMenuComponent::Awake()
{
	UIManager::GetInstance()->AddP1CustomizeMenuComponent(this);
	for (auto& button : myTransform->GetChildren())
	{
		UIManager::GetInstance()->AddCustomizeMenuButtonToManager(button->GetGameObject()->GetComponent<MenuButton>(), 1);
		button->GetGameObject()->SetActive(false);
		button->GetGameObject()->GetComponent<UISprite>()->SetAlpha(0);
	}
}

void P1CustomizeMenuComponent::Update()
{
	FadeOutP1CustomizeButtons();
	FadeInP1CustomizeButtons();
}

void P1CustomizeMenuComponent::RenderInProperties(std::vector<Component*>& aComponentList)
{
}

void P1CustomizeMenuComponent::RunInEditor()
{
}

void P1CustomizeMenuComponent::ShowP1CustomizationMenuButtons()
{
	for (auto& button : myTransform->GetChildren())
	{
		button->GetGameObject()->SetActive(true);

	}
}

void P1CustomizeMenuComponent::HideP1CustomizationMenuButtons()
{
	for (auto& button : myTransform->GetChildren())
	{
		button->GetGameObject()->SetActive(false);
	}
}

void P1CustomizeMenuComponent::FadeOutP1CustomizeButtons()
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
		HideP1CustomizationMenuButtons();
	}
}

void P1CustomizeMenuComponent::FadeInP1CustomizeButtons()
{
	if (myShouldFadeIn)
	{
		ShowP1CustomizationMenuButtons();
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

void P1CustomizeMenuComponent::ToggleFadeOut(bool aValue)
{
	if (UIManager::GetInstance()->GetP1MenuComponent()->IsFading() == false && myIsFading == false)
	{
		myIsFading = true;
		myShouldFadeOut = aValue; 
	}
}

void P1CustomizeMenuComponent::ToggleFadeIn(bool aValue)
{
	if (UIManager::GetInstance()->GetP1MenuComponent()->IsFading() == false && myIsFading == false)
	{
		UIManager::GetInstance()->SetP1IsInCustomize(true);
		UIManager::GetInstance()->SetP1IsInMainMenu(false);
		myIsFading = true;
		myShouldFadeIn = aValue;
	}
}

void P1CustomizeMenuComponent::UpdateSelectedButtons()
{
	int currentBtnId = UIManager::GetInstance()->GetP1CustomizeMenuIndex();

	for (size_t i = 0; i < myTransform->GetChildren().size(); i++)
	{
		UIManager::GetInstance()->GetP1CustomizeMenuButtons()[i]->OnMouseExit();
	}
	UIManager::GetInstance()->GetP1CustomizeMenuButtons()[currentBtnId]->OnMouseEnter();

}

void P1CustomizeMenuComponent::ClickSelectedButton()
{
	int currentBtnId = UIManager::GetInstance()->GetP1CustomizeMenuIndex();
	UIManager::GetInstance()->GetP1CustomizeMenuButtons()[currentBtnId]->OnClick();
}

void P1CustomizeMenuComponent::Save(rapidjson::Value& aComponentData)
{

}

void P1CustomizeMenuComponent::Load(rapidjson::Value& aComponentData)
{

}
