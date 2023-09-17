#include "Game.pch.h"
#include "P2CustomizeMenuComponent.h"
#include "MenuButton.h"
#include <Shaders/UI/ErodingMenu/UIErodingMenuPS.h>
#include "P2MenuComponent.h"


P2CustomizeMenuComponent::P2CustomizeMenuComponent()
{

}

P2CustomizeMenuComponent::~P2CustomizeMenuComponent()
{
}

void P2CustomizeMenuComponent::Awake()
{
	UIManager::GetInstance()->AddP2CustomizeMenuComponent(this);
	for (auto& button : myTransform->GetChildren())
	{
		UIManager::GetInstance()->AddCustomizeMenuButtonToManager(button->GetGameObject()->GetComponent<MenuButton>(), 2);
		button->GetGameObject()->SetActive(false);
		button->GetGameObject()->GetComponent<UISprite>()->SetAlpha(0);
	}
}

void P2CustomizeMenuComponent::Update()
{
	FadeOutP2CustomizeButtons();
	FadeInP2CustomizeButtons();
}

void P2CustomizeMenuComponent::RenderInProperties(std::vector<Component*>& aComponentList)
{
}

void P2CustomizeMenuComponent::RunInEditor()
{
}

void P2CustomizeMenuComponent::ShowP2CustomizationMenuButtons()
{
	for (auto& button : myTransform->GetChildren())
	{
		button->GetGameObject()->SetActive(true);
	}
}

void P2CustomizeMenuComponent::HideP2CustomizationMenuButtons()
{
	for (auto& button : myTransform->GetChildren())
	{
		button->GetGameObject()->SetActive(false);
	}
}

void P2CustomizeMenuComponent::FadeOutP2CustomizeButtons()
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
				//button->GetChildren()[i]->GetGameObject()->GetComponent<UISprite>()->SetAlpha(Alpha);
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
		HideP2CustomizationMenuButtons();
	}
}

void P2CustomizeMenuComponent::FadeInP2CustomizeButtons()
{
	if (myShouldFadeIn)
	{
		ShowP2CustomizationMenuButtons();
		myFadeTime += realDeltaTime;
		const float Percentage = myFadeTime / myTotalFadeTime;
		float Alpha = Catbox::Lerp(0.0f, 1.0f, Percentage);
		for (auto& button : myTransform->GetChildren())
		{
			button->GetGameObject()->GetComponent<UISprite>()->SetAlpha(Alpha);
			for (size_t i = 0; i < button->GetChildren().size(); i++)
			{
				//button->GetChildren()[i]->GetGameObject()->GetComponent<UISprite>()->SetAlpha(Alpha);				

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

void P2CustomizeMenuComponent::ToggleFadeOut(bool aValue)
{
	if (UIManager::GetInstance()->GetP2MenuComponent()->IsFading() == false && myIsFading == false)
	{ 
		myIsFading = true;
		myShouldFadeOut = aValue; 
	}
}

void P2CustomizeMenuComponent::ToggleFadeIn(bool aValue)
{
	if (UIManager::GetInstance()->GetP2MenuComponent()->IsFading() == false && myIsFading == false)
	{
		UIManager::GetInstance()->SetP2IsInCustomize(true);
		UIManager::GetInstance()->SetP2IsInMainMenu(false);
		myIsFading = true;
		myShouldFadeIn = aValue; 
	}
}

void P2CustomizeMenuComponent::UpdateSelectedButtons()
{
	int currentBtnId = UIManager::GetInstance()->GetP2CustomizeMenuIndex();

	for (size_t i = 0; i < myTransform->GetChildren().size(); i++)
	{
		UIManager::GetInstance()->GetP2CustomizeMenuButtons()[i]->OnMouseExit();
	}
	UIManager::GetInstance()->GetP2CustomizeMenuButtons()[currentBtnId]->OnMouseEnter();

}

void P2CustomizeMenuComponent::ClickSelectedButton()
{
	int currentBtnId = UIManager::GetInstance()->GetP2CustomizeMenuIndex();
	UIManager::GetInstance()->GetP2CustomizeMenuButtons()[currentBtnId]->OnClick();
}

void P2CustomizeMenuComponent::Save(rapidjson::Value& aComponentData)
{

}

void P2CustomizeMenuComponent::Load(rapidjson::Value& aComponentData)
{

}
