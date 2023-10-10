#include "Game.pch.h"
#include "P1MenuComponent.h"
#include "MenuButton.h"
#include "..\Catbox\Audio\AudioManager.h"
#include <Shaders/UI/ErodingMenu/UIErodingMenuPS.h>
#include "P1CustomizeMenuComponent.h"

P1MenuComponent::P1MenuComponent()
{

}

P1MenuComponent::~P1MenuComponent()
{
}

void P1MenuComponent::Awake()
{
	UIManager::GetInstance()->AddP1MenuComponent(this);
	for (auto& button : myTransform->GetChildren())
	{
		UIManager::GetInstance()->AddMenuButtonToManager(button->GetGameObject()->GetComponent<MenuButton>(), 1);
		button->GetGameObject()->SetActive(false);
		button->GetGameObject()->GetComponent<UISprite>()->SetAlpha(0);
	}
}

void P1MenuComponent::Update()
{
	FadeOutP1MenuButtons();
	FadeInP1MenuButtons();
}

void P1MenuComponent::RenderInProperties(std::vector<Component*>& aComponentList)
{
}

void P1MenuComponent::RunInEditor()
{
}

void P1MenuComponent::ShowP1MenuButtons()
{
	for (auto& button : myTransform->GetChildren())
	{
		button->GetGameObject()->SetActive(true);
	}
}

void P1MenuComponent::HideP1MenuButtons()
{
	for (auto& button : myTransform->GetChildren())
	{
		button->GetGameObject()->SetActive(false);
	}
}

void P1MenuComponent::FadeOutP1MenuButtons()
{
	if (myShouldFadeOut)
	{
		if (UIManager::GetInstance()->GetP1CustomizeMenuComponent()->IsFading() == false && myIsFading == true)
		{
			myFadeTime += realDeltaTime;
			const float Percentage = myFadeTime / myTotalFadeTime;
			float Alpha = Catbox::Lerp(1.0f, 0.0f, Percentage);
			for (auto& button : myTransform->GetChildren())
			{
				button->GetGameObject()->GetComponent<UISprite>()->SetAlpha(Alpha);
				for (size_t i = 0; i < button->GetChildren().size(); i++)
				{
					button->GetChildren()[i]->GetGameObject()->GetComponent<UISprite>()->SetAlpha(Alpha);
					auto ErodingShader = dynamic_cast<UIErodingMenuPS*>(button->GetChildren()[i]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
					if (ErodingShader)
					{
						ErodingShader->myUIErodingMenuData.Resource = Alpha;
						ErodingShader->SetData();
					}
				}
			}
		}
	}
	if (myFadeTime > myTotalFadeTime)
	{
		myFadeTime = 0.0f;
		myShouldFadeOut = false;
		myIsFading = false;
		HideP1MenuButtons();
	}
}

void P1MenuComponent::FadeInP1MenuButtons()
{
	if (myShouldFadeIn)
	{
		if (UIManager::GetInstance()->GetP1CustomizeMenuComponent()->IsFading() == false && myIsFading == true)
		{
			ShowP1MenuButtons();
			myFadeTime += realDeltaTime;
			const float Percentage = myFadeTime / myTotalFadeTime;
			float Alpha = Catbox::Lerp(0.0f, 1.0f, Percentage);
			for (auto& button : myTransform->GetChildren())
			{
				button->GetGameObject()->GetComponent<UISprite>()->SetAlpha(Alpha);
				for (size_t i = 0; i < button->GetChildren().size(); i++)
				{
					button->GetChildren()[i]->GetGameObject()->GetComponent<UISprite>()->SetAlpha(Alpha);
					auto ErodingShader = dynamic_cast<UIErodingMenuPS*>(button->GetChildren()[i]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
					if (ErodingShader)
					{
						ErodingShader->myUIErodingMenuData.Resource = Alpha;
						ErodingShader->SetData();
					}
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

void P1MenuComponent::ToggleFadeOut(bool aValue)
{
	if( myIsFading == false)
	{
		myShouldFadeOut = aValue; 
		myIsFading = true; 
	}
}

void P1MenuComponent::ToggleFadeIn(bool aValue)
{
	if (myIsFading == false)
	{
		myShouldFadeIn = aValue;
		myIsFading = true;
	}
}

void P1MenuComponent::UpdateSelectedButtons()
{
	int currentBtnId = UIManager::GetInstance()->GetP1MenuIndex();

	for (size_t i = 0; i < myTransform->GetChildren().size(); i++)
	{
		UIManager::GetInstance()->GetP1MenuButtons()[i]->OnMouseExit();
	}
	UIManager::GetInstance()->GetP1MenuButtons()[currentBtnId]->OnMouseEnter();

}

void P1MenuComponent::ClickSelectedButton()
{
	int currentBtnId = UIManager::GetInstance()->GetP1MenuIndex();
	UIManager::GetInstance()->GetP1MenuButtons()[currentBtnId]->OnClick();

	Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_CHOOSE));
}

void P1MenuComponent::Save(rapidjson::Value& aComponentData)
{

}

void P1MenuComponent::Load(rapidjson::Value& aComponentData)
{

}
