#include "Game.pch.h"
#include "MenuButton.h"
#include "Graphics/Rendering/GraphicsEngine.h"
#include "../Game/Managers/UIManager.h"
#include "../Catbox/ComponentTools/SceneManager.h"
#include "Components/CameraController.h"
#include <ComponentTools/UIEventHandler.h>
#include "../Catbox/Components/CameraController.h"
#include "Components/MainMenu/P1CustomizeMenuComponent.h"
#include "Components/MainMenu/P2CustomizeMenuComponent.h"
#include "Components/MainMenu/P1MenuComponent.h"
#include "Components/MainMenu/P2MenuComponent.h"
#include "Components//MainMenu/InteractableUIObjectComponent.h"
#include "Components/MainMenu/SettingsMenuComponent.h"
#include "Components/MainMenu/PauseMenuComponent.h"
#include "Components\Animator.h"
#include "Components\MainMenu\CountdownComponent.h"

MenuButton::MenuButton()
{

}

MenuButton::~MenuButton()
{

}

void MenuButton::Awake()
{

}

void MenuButton::Render()
{

}

void MenuButton::OnClick()
{
	switch (myAbilityID)
	{
	case MenuButton::MenuButtonID::Customize:
		print("Menu Button 1 - On Click");
		ButtonAction(MenuButtonID::Customize);
		break;
	case MenuButton::MenuButtonID::Settings:
		print("Menu Button 2 - On Click");
		ButtonAction(MenuButtonID::Settings);
		break;
	case MenuButton::MenuButtonID::Credits:
		print("Menu Button 3 - On Click");
		ButtonAction(MenuButtonID::Credits);
		break;
		break;
	case MenuButton::MenuButtonID::Quit_Game:
		print("Menu Button 5 - On Click");
		ButtonAction(MenuButtonID::Quit_Game);
		break;
	case MenuButton::MenuButtonID::Resume:
		print("Menu Button Resume - On Click");
		ButtonAction(MenuButtonID::Resume);
		break;
	case MenuButton::MenuButtonID::Ready:
		print("Menu Button Ready - On Click");
		ButtonAction(MenuButtonID::Ready);
		break;
	case MenuButton::MenuButtonID::ChangeMusicVol:
		print("Menu Button Music Vol - On Click");
		ButtonAction(MenuButtonID::ChangeMusicVol);
		break;
	case MenuButton::MenuButtonID::ChangeSFXVol:
		print("Menu Button SFX - On Click");
		ButtonAction(MenuButtonID::ChangeSFXVol);
		break;
	case MenuButton::MenuButtonID::ChangeResolution:
		print("Menu Button Resolution - On Click");
		ButtonAction(MenuButtonID::ChangeResolution);
		break;
	case MenuButton::MenuButtonID::CloseSettings:
		print("Menu Button Close Settings - On Click");
		ButtonAction(MenuButtonID::CloseSettings);
		break;
	case MenuButton::MenuButtonID::ReturnToMainMenu:
		print("Menu Button ReturnToMainMenu - On Click");
		ButtonAction(MenuButtonID::ReturnToMainMenu);
		break;
	case MenuButton::MenuButtonID::Hat:
		print("Menu Button Hat - On Click");
		ButtonAction(MenuButtonID::Hat);
		break;
	case MenuButton::MenuButtonID::closeCustomize:
		print("Menu Button closeCustomize - On Click");
		ButtonAction(MenuButtonID::closeCustomize);
		break;
	case MenuButton::MenuButtonID::controls:
		print("Menu Button controls - On Click");
		ButtonAction(MenuButtonID::controls);
		break;
	case MenuButton::MenuButtonID::ResetLevel:
		print("Menu Button Reset Level - On Click");
		ButtonAction(MenuButtonID::ResetLevel);
		break;
	default:
		break;
	}
}

void MenuButton::RenderInProperties(std::vector<Component*>& aComponentList)
{
	//Button ID
	int ButtonID = (int)myAbilityID;
	const char* items[] = { "Menu Button Customize", "Menu Button Settings", "Menu Button Credits", "Menu Button Quit",
							"Menu Button Resume","Menu Button Ready",
							"change music Vol", "Change SFX vol", "Change Resolution", "Close Settings","Return to MainMenu",
							"Hat Customization", "Close Customize", "Controls", "Reset Level" };
	if (ImGui::Combo("Button ID", &ButtonID, items, IM_ARRAYSIZE(items)))
	{
		if (ButtonID == 0)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 1)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 2)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 3)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 4)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 5)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 6)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 7)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 8)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 9)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 10)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 11)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 12)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 13)
			myAbilityID = (MenuButtonID)ButtonID;
		if (ButtonID == 14)
			myAbilityID = (MenuButtonID)ButtonID;
	}
	//Button ID
	int PlayerID = (int)myPlayerID;
	const char* items2[] = { "Player 1", "Player 2" };
	if (ImGui::Combo("Player ID", &PlayerID, items2, IM_ARRAYSIZE(items2)))
	{
		if (PlayerID == 0)
			myPlayerID = (MenuButton::PlayerID)PlayerID;
		if (PlayerID == 1)
			myPlayerID = (MenuButton::PlayerID)PlayerID;

	}
}

void MenuButton::OnMouseEnter()
{
	switch (myAbilityID)
	{
	case MenuButton::MenuButtonID::Customize:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("CustomizeSelected"));
		break;
	case MenuButton::MenuButtonID::Settings:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("SettingsSelected"));
		break;
	case MenuButton::MenuButtonID::Credits:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("CreditsSelected"));
		break;
	case MenuButton::MenuButtonID::Quit_Game:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("QuitSelected"));
		break;
	case MenuButton::MenuButtonID::Resume:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("ResumeSelected"));
		break;
	case MenuButton::MenuButtonID::Ready:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("ReadySelected"));
		break;
	case MenuButton::MenuButtonID::ChangeMusicVol:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("MusicVolumeSelected"));
		break;
	case MenuButton::MenuButtonID::ChangeSFXVol:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("SFXVolumeSelected"));
		break;
	case MenuButton::MenuButtonID::ChangeResolution:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("ResolutionSelected"));
		break;
	case MenuButton::MenuButtonID::CloseSettings:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("CloseSettingsSelected"));
		break;
	case MenuButton::MenuButtonID::ReturnToMainMenu:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("MainMenuSelected"));
		break;
	case MenuButton::MenuButtonID::Hat:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("HatSelected"));
		break;
	case MenuButton::MenuButtonID::closeCustomize:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("CloseSettingsSelected"));
		break;
	case MenuButton::MenuButtonID::controls:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("ControlsSelected"));
		break;
	case MenuButton::MenuButtonID::ResetLevel:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("ResetLevelSelected"));
		break;
	default:
		break;
	}
}

void MenuButton::OnMouseExit()
{
	switch (myAbilityID)
	{
	case MenuButton::MenuButtonID::Customize:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Customize"));
		break;
	case MenuButton::MenuButtonID::Settings:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Settings"));
		break;
	case MenuButton::MenuButtonID::Credits:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Credits"));
		break;
	case MenuButton::MenuButtonID::Quit_Game:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Quit"));
		break;
	case MenuButton::MenuButtonID::Resume:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Resume"));
		break;
	case MenuButton::MenuButtonID::Ready:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Ready"));
		break;
	case MenuButton::MenuButtonID::ChangeMusicVol:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("MusicVolume"));
		break;
	case MenuButton::MenuButtonID::ChangeSFXVol:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("SFXVolume"));
		break;
	case MenuButton::MenuButtonID::ChangeResolution:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Resolution"));
		break;
	case MenuButton::MenuButtonID::CloseSettings:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("CloseSettings"));
		break;
	case MenuButton::MenuButtonID::ReturnToMainMenu:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("MainMenu"));
		break;
	case MenuButton::MenuButtonID::Hat:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Hat"));
		break;
	case MenuButton::MenuButtonID::closeCustomize:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("CloseSettings"));
		break;
	case MenuButton::MenuButtonID::controls:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Controls"));
		break;
	case MenuButton::MenuButtonID::ResetLevel:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("ResetLevel"));
		break;
	default:
		break;
	}
}

void MenuButton::ButtonAction(MenuButtonID aID)
{
	switch (aID)
	{
	case MenuButton::MenuButtonID::Customize:
		print("Menu Button Customize - In ButtonAction()");
		if (myPlayerID == MenuButton::PlayerID::Player1)
		{
			UIManager::GetInstance()->GetP1CustomizeMenuComponent()->ToggleFadeIn(true);
			//UIManager::GetInstance()->SetP1IsInCustomize(true);
			//UIManager::GetInstance()->SetP1IsInMainMenu(false);
		}
		else if (myPlayerID == MenuButton::PlayerID::Player2)
		{
			UIManager::GetInstance()->GetP2CustomizeMenuComponent()->ToggleFadeIn(true);
			//UIManager::GetInstance()->SetP2IsInCustomize(true);
			//UIManager::GetInstance()->SetP2IsInMainMenu(false);
		}
		break;
	case MenuButton::MenuButtonID::Settings:
	{
		print("Menu Button Settings - In ButtonAction()");
		if (myPlayerID == MenuButton::PlayerID::Player1 && !UIManager::GetInstance()->GetP2IsInSettings())
		{
			UIManager::GetInstance()->SetP1IsInMainMenu(false);
			UIManager::GetInstance()->SetP1IsInSettings(true);
			if (UIManager::GetInstance()->GetIsInPauseMenu())
			{
				UIManager::GetInstance()->GetSettingsMenuComponent()->GetTransform()->SetWorldPos({ -0.37f,-0.04f,0.0f });
			}
			else 
			{
				UIManager::GetInstance()->GetSettingsMenuComponent()->GetTransform()->SetWorldPos({ -0.30f,0.22f,0.0f });
				UIManager::GetInstance()->GetP2MenuComponent()->GetTransform()->GetChildren()[1]->GetGameObject()->GetComponent<UISprite>()->SetAlpha(0.2f);
			}
			UIManager::GetInstance()->GetSettingsMenuComponent()->ToggleFadeIn(true);
		}
		else if (myPlayerID == MenuButton::PlayerID::Player2 && !UIManager::GetInstance()->GetP1IsInSettings())
		{
			UIManager::GetInstance()->SetP2IsInMainMenu(false);
			UIManager::GetInstance()->SetP2IsInSettings(true);
			UIManager::GetInstance()->GetSettingsMenuComponent()->GetTransform()->SetWorldPos({ 0.30f,0.22f,0.0f });
			UIManager::GetInstance()->GetP1MenuComponent()->GetTransform()->GetChildren()[1]->GetGameObject()->GetComponent<UISprite>()->SetAlpha(0.2f);
			UIManager::GetInstance()->GetSettingsMenuComponent()->ToggleFadeIn(true);
		}
		break;
	}
	case MenuButton::MenuButtonID::Credits:
		print("Menu Button Credits - In ButtonAction()");
		if (myPlayerID == MenuButton::PlayerID::Player1)
		{
			if (UIManager::GetInstance()->GetInteractableUIObjectComponent()->GetTransform()->GetChildren()[6]->GetGameObject()->IsActive())
			{
				//UIManager::GetInstance()->GetInteractableUIObjectComponent()->GetTransform()->GetChildren()[6]->GetGameObject()->SetActive(false);
				UIManager::GetInstance()->GetInteractableUIObjectComponent()->ToggleP1CreditsFadeOut(true);
				UIManager::GetInstance()->SetP1IsInCredits(false);
			}
			else
			{
				UIManager::GetInstance()->GetInteractableUIObjectComponent()->GetTransform()->GetChildren()[6]->GetGameObject()->SetActive(true);
				UIManager::GetInstance()->GetInteractableUIObjectComponent()->ToggleP1CreditsFadeIn(true);
				UIManager::GetInstance()->SetP1IsInCredits(true);
			}
		}
		else if (myPlayerID == MenuButton::PlayerID::Player2)
		{
			if (UIManager::GetInstance()->GetInteractableUIObjectComponent()->GetTransform()->GetChildren()[5]->GetGameObject()->IsActive())
			{
				//UIManager::GetInstance()->GetInteractableUIObjectComponent()->GetTransform()->GetChildren()[5]->GetGameObject()->SetActive(false);
				UIManager::GetInstance()->GetInteractableUIObjectComponent()->ToggleP2CreditsFadeOut(true);
				UIManager::GetInstance()->SetP2IsInCredits(false);
			}
			else
			{
				UIManager::GetInstance()->GetInteractableUIObjectComponent()->GetTransform()->GetChildren()[5]->GetGameObject()->SetActive(true);
				UIManager::GetInstance()->GetInteractableUIObjectComponent()->ToggleP2CreditsFadeIn(true);
				UIManager::GetInstance()->SetP2IsInCredits(true);
			}
		}
		break;
	case MenuButton::MenuButtonID::Quit_Game:
		print("Menu Button Quit - In ButtonAction()");
		PostMessage(GetForegroundWindow(), WM_CLOSE, 0, 0);
		break;
	case MenuButton::MenuButtonID::Resume:
		UIManager::GetInstance()->SetIsInPauseMenu(false);
		Engine::GetInstance()->SetGamePaused(false);
		UIManager::GetInstance()->GetPauseMenuComponent()->ToggleFadeOut(true);
		print("Menu Button Resume - In ButtonAction()");
		break;
	case MenuButton::MenuButtonID::Ready:
		print("Menu Button Ready - In ButtonAction()");
		
		if (myPlayerID == MenuButton::PlayerID::Player1)
		{
			if (GameManager::GetInstance()->GetPlayer1Data()->GetIsReady() == false)
			{
				Color ReadyColor = { 0.0f,1.0f,0.0f,1.0f };
				myGameObject->GetComponent<UISprite>()->SetColor(ReadyColor);
				GameManager::GetInstance()->GetPlayer1Data()->SetIsReady(true);
				UIManager::GetInstance()->GetPlayer1Object()->GetComponent<Animator>()->SetBool("Selected", true);
				Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_PLAYER_READY));
			}
			else
			{
				Color NotReadyColor = { 1.0f,1.0f,1.0f,1.0f };
				myGameObject->GetComponent<UISprite>()->SetColor(NotReadyColor);
				GameManager::GetInstance()->GetPlayer1Data()->SetIsReady(false);
				UIManager::GetInstance()->GetPlayer1Object()->GetComponent<Animator>()->SetBool("Selected", false);
			}
		}
		else if (myPlayerID == MenuButton::PlayerID::Player2)
		{
			if (GameManager::GetInstance()->GetPlayer2Data()->GetIsReady() == false)
			{
				Color ReadyColor = { 0.0f,1.0f,0.0f,1.0f };
				myGameObject->GetComponent<UISprite>()->SetColor(ReadyColor);
				GameManager::GetInstance()->GetPlayer2Data()->SetIsReady(true);
				UIManager::GetInstance()->GetPlayer2Object()->GetComponent<Animator>()->SetBool("Selected", true);
				Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_PLAYER_READY));
			}
			else
			{
				Color NotReadyColor = { 1.0f,1.0f,1.0f,1.0f };
				myGameObject->GetComponent<UISprite>()->SetColor(NotReadyColor);
				GameManager::GetInstance()->GetPlayer2Data()->SetIsReady(false);
				UIManager::GetInstance()->GetPlayer2Object()->GetComponent<Animator>()->SetBool("Selected", false);
			}

		}
		break;
	case MenuButton::MenuButtonID::ChangeMusicVol:
		print("Menu Button Change music vol - In ButtonAction()");
		break;
	case MenuButton::MenuButtonID::ChangeSFXVol:
		print("Menu Button Change SFX vol - In ButtonAction()");
		break;
	case MenuButton::MenuButtonID::ChangeResolution:
		print("Menu Button Change Resolution - In ButtonAction()");
		break;
	case MenuButton::MenuButtonID::CloseSettings:
	{
		print("Menu Button Close settings - In ButtonAction()");

		UIManager::GetInstance()->SetP1IsInSettings(false);
		if (UIManager::GetInstance()->GetIsInPauseMenu() == false)
		{
			UIManager::GetInstance()->SetP2IsInSettings(false);
			UIManager::GetInstance()->SetP1IsInMainMenu(true);
			UIManager::GetInstance()->SetP2IsInMainMenu(true);
			UIManager::GetInstance()->GetP1MenuComponent()->GetTransform()->GetChildren()[1]->GetGameObject()->GetComponent<UISprite>()->SetAlpha(1.0f);
			UIManager::GetInstance()->GetP2MenuComponent()->GetTransform()->GetChildren()[1]->GetGameObject()->GetComponent<UISprite>()->SetAlpha(1.0f);
		}
		UIManager::GetInstance()->GetSettingsMenuComponent()->ToggleFadeOut(true);
		UIManager::GetInstance()->SetSettingMenuIndex(0);
		break;
	}
	case MenuButton::MenuButtonID::ReturnToMainMenu:
	{
		GameManager::GetInstance()->SetBoatPos(GameManager::GetInstance()->GetBoatStartPos());
		UIManager::GetInstance()->SetIsInPauseMenu(false);
		Engine::GetInstance()->SetGamePaused(false);
		UIManager::GetInstance()->GetPauseMenuComponent()->ToggleFadeOut(true);
		GameManager::GetInstance()->ResetLevel();
		GameManager::GetInstance()->GenerateOrders(false);
		UIManager::GetInstance()->ClearUIManager();
		if(UIManager::GetInstance()->GetCountdownComponent())
		{
			UIManager::GetInstance()->GetCountdownComponent()->ResetCountdown();
			UIManager::GetInstance()->GetCountdownComponent()->StartCountdown();
		}
		UIEventHandler::SetNextLevel("Main_Menu_lvl");
		break;
	}
	case MenuButton::MenuButtonID::Hat:
	{
		print("Menu Button Hat - In ButtonAction()");
		break;
	}
	case MenuButton::MenuButtonID::closeCustomize:
	{
		print("Menu Button Close Customze - In ButtonAction()");
		if (myPlayerID == MenuButton::PlayerID::Player1)
		{
			UIManager::GetInstance()->GetP1CustomizeMenuComponent()->ToggleFadeOut(true);
			//UIManager::GetInstance()->GetP1MenuComponent()->ToggleFadeIn(true);
			UIManager::GetInstance()->SetP1IsInCustomize(false);
			UIManager::GetInstance()->SetP1IsInMainMenu(true);
			UIManager::GetInstance()->SetP1CustomizeIndex(0);
		}
		else if (myPlayerID == MenuButton::PlayerID::Player2)
		{
			UIManager::GetInstance()->GetP2CustomizeMenuComponent()->ToggleFadeOut(true);
			//UIManager::GetInstance()->GetP2MenuComponent()->ToggleFadeIn(true);
			UIManager::GetInstance()->SetP2IsInCustomize(false);
			UIManager::GetInstance()->SetP2IsInMainMenu(true);
			UIManager::GetInstance()->SetP2CustomizeIndex(0);
		}
		break;
	}
	case MenuButton::MenuButtonID::controls:
	{
		print("Menu Button Controls - In ButtonAction()");
		if (myPlayerID == MenuButton::PlayerID::Player1)
		{
			if (!UIManager::GetInstance()->GetIsInPauseMenu())
			{
				if (UIManager::GetInstance()->GetInteractableUIObjectComponent()->GetTransform()->GetChildren()[7]->GetGameObject()->IsActive())
				{					
					UIManager::GetInstance()->GetInteractableUIObjectComponent()->ToggleP1ControlsFadeOut(true);
					UIManager::GetInstance()->SetP1IsInControls(false);
				}
				else
				{
					UIManager::GetInstance()->GetInteractableUIObjectComponent()->GetTransform()->GetChildren()[7]->GetGameObject()->SetActive(true);
					UIManager::GetInstance()->GetInteractableUIObjectComponent()->ToggleP1ControlsFadeIn(true);
					UIManager::GetInstance()->SetP1IsInControls(true);
				}
			}
			else
			{
				if (UIManager::GetInstance()->GetPauseMenuComponent()->GetTransform()->GetChildren()[3]->GetChildren()[0]->GetGameObject()->IsActive())
				{
					UIManager::GetInstance()->GetPauseMenuComponent()->GetTransform()->GetChildren()[3]->GetChildren()[0]->GetGameObject()->SetActive(false);
					UIManager::GetInstance()->GetPauseMenuComponent()->GetTransform()->GetChildren()[3]->GetChildren()[1]->GetGameObject()->SetActive(false);
				}
				else
				{
					UIManager::GetInstance()->GetPauseMenuComponent()->GetTransform()->GetChildren()[3]->GetChildren()[0]->GetGameObject()->SetActive(true);
					UIManager::GetInstance()->GetPauseMenuComponent()->GetTransform()->GetChildren()[3]->GetChildren()[1]->GetGameObject()->SetActive(true);
				}
			}
		}
		else if (myPlayerID == MenuButton::PlayerID::Player2)
		{
			if (UIManager::GetInstance()->GetP2IsInControls())
			{				
				UIManager::GetInstance()->GetInteractableUIObjectComponent()->ToggleP2ControlsFadeOut(true);
				//UIManager::GetInstance()->SetP2IsInControls(false);
			}
			else
			{
				UIManager::GetInstance()->GetInteractableUIObjectComponent()->GetTransform()->GetChildren()[8]->GetGameObject()->SetActive(true);
				UIManager::GetInstance()->GetInteractableUIObjectComponent()->ToggleP2ControlsFadeIn(true);
				UIManager::GetInstance()->SetP2IsInControls(true);
			}
		}
		break;
	}
	case MenuButton::MenuButtonID::ResetLevel:
	{
		Scene& activeScene = *Engine::GetInstance()->GetSceneManager()->GetCurrentScene();	

		if (activeScene.GetName() != "LevelSelect")
		{
			UIManager::GetInstance()->SetIsInPauseMenu(false);
			GameManager::GetInstance()->ResetLevel();
			UIManager::GetInstance()->GetCountdownComponent()->ResetCountdown();
			GameManager::GetInstance()->SetIsShowingTutorial(true);
			UIEventHandler::SetNextLevel(activeScene.GetName());

			print("Menu Button Reset Level - In ButtonAction()");
		}
		else
		{
			Engine::GetInstance()->SetGamePaused(false);
			UIManager::GetInstance()->SetIsInPauseMenu(false);
			UIEventHandler::SetNextLevel(activeScene.GetName());
		}
		break;
	}
	default:
		break;
	}
}

void MenuButton::ActivateChild(int aIndex)
{
	if (myTransform->GetChildren()[aIndex]->GetGameObject()->IsActive())
	{
		myTransform->GetChildren()[aIndex]->GetGameObject()->SetActive(false);
	}
	else
	{
		myTransform->GetChildren()[aIndex]->GetGameObject()->SetActive(true);
	}
}

void MenuButton::CloseAllChildren()
{
	if (myTransform->GetChildren().size() > 0)
	{
		if (myTransform->GetChildren()[0]->GetGameObject()->IsActive())
			myTransform->GetChildren()[0]->GetGameObject()->SetActive(false);
	}
}

void MenuButton::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();

	int AbilityID = (int)myAbilityID;
	wrapper.SaveValue<DataType::Int>("MenuButtonID", AbilityID);
	int PlayerID = (int)myPlayerID;
	wrapper.SaveValue<DataType::Int>("PlayerID", PlayerID);
}

void MenuButton::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("MenuButtonID"))
	{
		myAbilityID = (MenuButtonID)aComponentData["MenuButtonID"].GetInt();
	}
	else
	{
		myAbilityID = MenuButtonID::Customize;
	}
	if (aComponentData.HasMember("PlayerID"))
	{
		myPlayerID = (MenuButton::PlayerID)aComponentData["PlayerID"].GetInt();
	}
	else
	{
		myPlayerID = MenuButton::PlayerID::Player1;
	}
}




