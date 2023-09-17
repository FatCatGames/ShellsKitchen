#include "Game.pch.h"
#include "P2MenuComponent.h"
#include "MenuButton.h"
#include "..\Catbox\Audio\AudioManager.h"
#include <Shaders/UI/ErodingMenu/UIErodingMenuPS.h>
#include "P2CustomizeMenuComponent.h"

P2MenuComponent::P2MenuComponent()
{
	
}

P2MenuComponent::~P2MenuComponent()
{
}

void P2MenuComponent::Awake()
{
	UIManager::GetInstance()->AddP2MenuComponent(this);
	for (auto& button : myTransform->GetChildren())
	{
		UIManager::GetInstance()->AddMenuButtonToManager(button->GetGameObject()->GetComponent<MenuButton>(), 2);
		button->GetGameObject()->SetActive(false);
		button->GetGameObject()->GetComponent<UISprite>()->SetAlpha(0);
	}

	UpdateSelectedButtons();
}

void P2MenuComponent::Update()
{
	FadeOutP2MenuButtons();
	FadeInP2MenuButtons();
}

void P2MenuComponent::FadeOutP2MenuButtons()
{
	if (myShouldFadeOut)
	{
		if (UIManager::GetInstance()->GetP2CustomizeMenuComponent()->IsFading() == false && myIsFading == true)
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
	}
	if (myFadeTime > myTotalFadeTime)
	{
		myFadeTime = 0.0f;
		myShouldFadeOut = false;
		myIsFading = false;
		HideP2MenuButtons();
	}
}

void P2MenuComponent::FadeInP2MenuButtons()
{
	if (myShouldFadeIn)
	{
		if (UIManager::GetInstance()->GetP2CustomizeMenuComponent()->IsFading() == false && myIsFading == true)
		{
			ShowP2MenuButtons();
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
	}
	if (myFadeTime > myTotalFadeTime)
	{
		myFadeTime = 0.0f;
		myShouldFadeIn = false;
		myIsFading = false;
	}
}

void P2MenuComponent::ToggleFadeOut(bool aValue)
{
	if (myIsFading == false)
	{
		myShouldFadeOut = aValue;
		myIsFading = true;
	}
}

void P2MenuComponent::ToggleFadeIn(bool aValue)
{
	if (myIsFading == false)
	{
		myShouldFadeIn = aValue;
		myIsFading = true;
	}
}

void P2MenuComponent::RenderInProperties(std::vector<Component*>& aComponentList)
{
}

void P2MenuComponent::RunInEditor()
{
}

void P2MenuComponent::ShowP2MenuButtons()
{
	for (auto& button : myTransform->GetChildren())
	{
		button->GetGameObject()->SetActive(true);
	}
}

void P2MenuComponent::HideP2MenuButtons()
{
	for (auto& button : myTransform->GetChildren())
	{
		button->GetGameObject()->SetActive(false);
	}
}

void P2MenuComponent::UpdateSelectedButtons()
{
	int currentBtnId = UIManager::GetInstance()->GetP2MenuIndex();

	for (size_t i = 0; i < myTransform->GetChildren().size(); i++)
	{
		UIManager::GetInstance()->GetP2MenuButtons()[i]->OnMouseExit();
	}
	UIManager::GetInstance()->GetP2MenuButtons()[currentBtnId]->OnMouseEnter();

}

void P2MenuComponent::ClickSelectedButton()
{
	int currentBtnId = UIManager::GetInstance()->GetP2MenuIndex();
	UIManager::GetInstance()->GetP2MenuButtons()[currentBtnId]->OnClick();

	Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_CHOOSE));
}

void P2MenuComponent::Save(rapidjson::Value& aComponentData)
{

}

void P2MenuComponent::Load(rapidjson::Value& aComponentData)
{

}
