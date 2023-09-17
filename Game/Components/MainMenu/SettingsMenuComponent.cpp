#include "Game.pch.h"
#include "SettingsMenuComponent.h"
#include "MenuButton.h"
#include "Audio/AudioManager.h"
#include <Shaders/UI/ErodingMenu/UIErodingMenuPS.h>

SettingsMenuComponent::SettingsMenuComponent()
{
}

void SettingsMenuComponent::Awake()
{
	UIManager::GetInstance()->AddSettingsMenuComponent(this);
	for (auto& button : myTransform->GetChildren())
	{
		UIManager::GetInstance()->AddSettingsMenuButtonToManager(button->GetGameObject()->GetComponent<MenuButton>());
		button->GetGameObject()->SetActive(false);
		button->GetGameObject()->GetComponent<UISprite>()->SetAlpha(0);
		if (button->GetChildren().size() > 0)
		{
			for (size_t i = 0; i < button->GetChildren().size(); i++)
			{
				button->GetChildren()[i]->GetGameObject()->GetComponent<UISprite>()->SetAlpha(0);
			}
		}
	}

	UpdateSelectedButtons();
}

void SettingsMenuComponent::Update()
{
	FadeOutSettingsMenuButtons();
	FadeInSettingsMenuButtons();
}

void SettingsMenuComponent::ShowSettingsMenuButtons()
{
	for (auto& button : myTransform->GetChildren())
	{
		button->GetGameObject()->SetActive(true);
	}
}

void SettingsMenuComponent::HideSettingsMenuButtons()
{
	for (auto& button : myTransform->GetChildren())
	{
		button->GetGameObject()->SetActive(false);
	}
}

void SettingsMenuComponent::FadeOutSettingsMenuButtons()
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
	if (myFadeTime > myTotalFadeTime)
	{
		myFadeTime = 0.0f;
		myShouldFadeOut = false;
		HideSettingsMenuButtons();
	}
}

void SettingsMenuComponent::FadeInSettingsMenuButtons()
{
	if (myShouldFadeIn)
	{
		ShowSettingsMenuButtons();
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
	if (myFadeTime > myTotalFadeTime)
	{
		myFadeTime = 0.0f;
		myShouldFadeIn = false;
	}
}

void SettingsMenuComponent::UpdateSelectedButtons()
{
	int currentBtnId = UIManager::GetInstance()->GetSettingsMenuIndex();

	for (size_t i = 0; i < myTransform->GetChildren().size(); i++)
	{
		UIManager::GetInstance()->GetSettingsMenuButtons()[i]->OnMouseExit();
	}
	UIManager::GetInstance()->GetSettingsMenuButtons()[currentBtnId]->OnMouseEnter();

	int musicVolume = Engine::GetInstance()->GetAudioManager()->GetMusicVolume() * 0.5f;
	int SFXVolume = Engine::GetInstance()->GetAudioManager()->GetSFXVolume() * 0.5f;
	std::string imageStringMusic = "Volume_" + to_string(musicVolume);
	std::string imageStringSFX = "Volume_" + to_string(SFXVolume);

	myTransform->GetChildren()[0]->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>(imageStringMusic));
	myTransform->GetChildren()[1]->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>(imageStringSFX));

	if (UIManager::GetInstance()->GetResolution() == 1080)
	{
		myTransform->GetChildren()[2]->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("1080"));
	}
	else if(UIManager::GetInstance()->GetResolution() == 2160)
	{
		myTransform->GetChildren()[2]->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("2160"));
	}
	else if (UIManager::GetInstance()->GetResolution() == 720)
	{
		myTransform->GetChildren()[2]->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("720"));
	}

}

void SettingsMenuComponent::ClickSelectedButton()
{
	int currentBtnId = UIManager::GetInstance()->GetSettingsMenuIndex();
	UIManager::GetInstance()->GetSettingsMenuButtons()[currentBtnId]->OnClick();
}