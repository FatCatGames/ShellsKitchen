#include "Game.pch.h"
#include "UIManager.h"
#include "../Catbox/Audio/AudioManager.h"
#include "Components/MainMenu/NumberOfPlayersComponent.h"
#include "Components/MainMenu/P1MenuComponent.h"
#include "Components/MainMenu/P2MenuComponent.h"
#include "Components/MainMenu/P1CustomizeMenuComponent.h"
#include "Components/MainMenu/P2CustomizeMenuComponent.h"
#include "Components/MainMenu/InteractableUIObjectComponent.h"
#include "Components/MainMenu/CountdownComponent.h"
#include "Components//MainMenu/PauseMenuComponent.h"
#include "Components//MainMenu/SettingsMenuComponent.h"
#include "ComponentTools\SceneManager.h"
#include "../Catbox/Components/3D/ModelInstance.h"
#include "../Catbox/Assets/Model.h"
#include "../Catbox/Assets/Material.h"
#include <ComponentTools/UIEventHandler.h>
#include "../Catbox/CommonUtilities/InputHandler.h"

UIManager* UIManager::Instance;


UIManager::UIManager()
{
	Instance = this;
}

void UIManager::ClearUIManager()
{
	myP1MenuButtons.clear();
	myP2MenuButtons.clear();
	myPauseMenuButtons.clear();
	mySettingsButtons.clear();
	myP1CustomizeMenuButtons.clear();
	myP2CustomizeMenuButtons.clear();
	//clear other
	//Menu Indexes
	myP1MenuIndex = 0;
	myP2MenuIndex = 0;
	myP1CustomizeMenuIndex = 0;
	myP2CustomizeMenuIndex = 0;
	mySettingsMenuIndex = 0;
	myP1IsInMainMenu = false;
	myP2IsInMainMenu = false;
	myP1IsInSettings = false;
	myP2IsInSettings = false;
	myP1IsInCustomize = false;
	myP2IsInCustomize = false;
	myP1IsInCredits = false;
	myP2IsInCredits = false;
	myP1IsInControls = false;
	myP2IsInControls = false;
	myP1HasConnected = false;
	myP2HasConnected = false;
	myShouldCountdown = false;
	mySplashScreenShows = true;
	myShowingFullscreenControlls = false;

	//Timers
	myReadyTimer = 3.0f;

	myNumberOfPlayersComponent = nullptr;
	myInteractableUIObjectComponent = nullptr;
	myP1MenuComponent = nullptr;
	myP2MenuComponent = nullptr;
	myP1CustomizeMenuComponent = nullptr;
	myP2CustomizeMenuComponent = nullptr;
	myPauseMenuComponent = nullptr;
	mySettingsMenuComponent = nullptr;
	myCountdownComponent = nullptr;
}

void UIManager::LoadLevelSelect()
{
	if (myPlayer1Data->GetIsReady() && myPlayer2Data->GetIsReady())
	{
		if (myShouldCountdown == false)
		{
			//myCountdownComponent->StartCountdown();
			myShouldCountdown = true;
		}
		myReadyTimer -= deltaTime;
	}
	else
	{
		myReadyTimer = 2.0f;
	}
	if (myReadyTimer <= 0.0f)
	{
		ClearUIManager();
		myPlayer1Data->SetIsReady(false);
		myPlayer2Data->SetIsReady(false);
		SetP1IsInMainMenu(false);
		SetP2IsInMainMenu(false);
		GameManager::GetInstance()->SetBoatPos(GameManager::GetInstance()->GetBoatStartPos());
		UIEventHandler::SetNextLevel("LevelSelect");
	}
}

void UIManager::AddInteractableUIObjectComponent(InteractableUIObjectComponent* aComponent)
{
	myInteractableUIObjectComponent = aComponent;
	myPlayer2Object = myInteractableUIObjectComponent->GetTransform()->GetChildren()[1]->GetGameObject();
	myPlayer1Object = myInteractableUIObjectComponent->GetTransform()->GetChildren()[2]->GetGameObject();
}

void UIManager::AddMenuButtonToManager(MenuButton* aButton, int aPlayerID)
{
	if (aPlayerID == 1)
		myP1MenuButtons.push_back(aButton);
	else if (aPlayerID == 2)
		myP2MenuButtons.push_back(aButton);
}

void UIManager::AddCustomizeMenuButtonToManager(MenuButton* aButton, int aPlayerID)
{
	if (aPlayerID == 1)
		myP1CustomizeMenuButtons.push_back(aButton);
	else if (aPlayerID == 2)
		myP2CustomizeMenuButtons.push_back(aButton);
}

void UIManager::AddPauseMenuButtonToManager(MenuButton* aButton)
{
	myPauseMenuButtons.push_back(aButton);
}

void UIManager::AddSettingsMenuButtonToManager(MenuButton* aButton)
{
	mySettingsButtons.push_back(aButton);
}

void UIManager::P1CustomizeInput()
{
	if (Input::GetKeyPress(KeyCode::S))
	{
		ChangeP1CustomizeMenuIndex(1);
	}
	else if (Input::GetKeyPress(KeyCode::W))
	{
		ChangeP1CustomizeMenuIndex(-1);
	}
	else if (Input::GetKeyPress(KeyCode::D))
	{
		if (myP1CustomizeMenuIndex == 0)
		{
			int HatID = myPlayer1Data->GetHatID();
			myPlayer1Data->SetHatID(++HatID);
			HatID = myPlayer1Data->GetHatID();
			const std::string& hatName = GameManager::GetInstance()->GetMyHatsVector()[HatID];
			myInteractableUIObjectComponent->GetTransform()->GetChildren()[2]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>(GameManager::GetInstance()->GetMyHatsVector()[HatID]));

			const string& MatName = hatName.substr(0, hatName.size() - 4) + "mat";
			myInteractableUIObjectComponent->GetTransform()->GetChildren()[2]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>(MatName), 0);
			print("Hat ID Player 1 Increased");
			//Update HatMesh
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
	}
	else if (Input::GetKeyPress(KeyCode::A))
	{
		if (myP1CustomizeMenuIndex == 0)
		{
			int HatID = myPlayer1Data->GetHatID();
			myPlayer1Data->SetHatID(--HatID);
			HatID = myPlayer1Data->GetHatID();
			const std::string& hatName = GameManager::GetInstance()->GetMyHatsVector()[HatID];
			myInteractableUIObjectComponent->GetTransform()->GetChildren()[2]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>(GameManager::GetInstance()->GetMyHatsVector()[HatID]));

			const string& MatName = hatName.substr(0, hatName.size() - 4) + "mat";
			myInteractableUIObjectComponent->GetTransform()->GetChildren()[2]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>(MatName), 0);
			print("Hat ID Player 1 Decreased");
			//Update HatMesh
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
	}
}

void UIManager::P2CustomizeInput()
{
	if (Input::GetKeyPress(KeyCode::DOWN))
	{
		ChangeP2CustomizeMenuIndex(1);
	}
	else if (Input::GetKeyPress(KeyCode::UP))
	{
		ChangeP2CustomizeMenuIndex(-1);
	}
	if (Input::GetKeyPress(KeyCode::RIGHT))
	{
		if (myP2CustomizeMenuIndex == 0)
		{
			int HatID = myPlayer2Data->GetHatID();
			myPlayer2Data->SetHatID(++HatID);
			HatID = myPlayer2Data->GetHatID();
			const std::string& hatName = GameManager::GetInstance()->GetMyHatsVector()[HatID];
			myInteractableUIObjectComponent->GetTransform()->GetChildren()[1]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>(GameManager::GetInstance()->GetMyHatsVector()[HatID]));

			const string& MatName = hatName.substr(0, hatName.size() - 4) + "mat";
			myInteractableUIObjectComponent->GetTransform()->GetChildren()[1]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>(MatName), 0);

			print("Hat ID Player 2 Increased");
			//Update HatMesh
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
	}
	else if (Input::GetKeyPress(KeyCode::LEFT))
	{
		if (myP2CustomizeMenuIndex == 0)
		{
			int HatID = myPlayer2Data->GetHatID();
			myPlayer2Data->SetHatID(--HatID);
			HatID = myPlayer2Data->GetHatID();
			const std::string& hatName = GameManager::GetInstance()->GetMyHatsVector()[HatID];
			myInteractableUIObjectComponent->GetTransform()->GetChildren()[1]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>(GameManager::GetInstance()->GetMyHatsVector()[HatID]));

			const string& MatName = hatName.substr(0, hatName.size() - 4) + "mat";
			myInteractableUIObjectComponent->GetTransform()->GetChildren()[1]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>(MatName), 0);

			print("Hat ID Player 2 Decreased");
			//Update HatMesh
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
	}
}

void UIManager::P1SettingsInput()
{
	if (Input::GetKeyPress(KeyCode::S))
	{
		ChangeSettingsIndex(1);
	}
	else if (Input::GetKeyPress(KeyCode::W))
	{
		ChangeSettingsIndex(-1);
	}
	if (Input::GetKeyPress(KeyCode::D))
	{
		if (mySettingsMenuIndex == 0)
		{
			myAudioManager->IncreaseMusicVolume();
			print("Change Music Volume - UP");
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		if (mySettingsMenuIndex == 1)
		{
			myAudioManager->IncreaseSFXVolume();
			print("Change SFX Volume - UP");
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		if (mySettingsMenuIndex == 2)
		{
			if (GetResolution() == 720)
			{
				DX11::SetResolution(1920, 1080);
				SetResolution(1080);
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			else if (GetResolution() == 1080)
			{
				DX11::SetResolution(3840, 2160);
				SetResolution(2160);
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			else if (GetResolution() == 2160)
			{
				DX11::SetResolution(1280, 720);
				SetResolution(720);
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			print("Change Resolution - UP");
		}

		if (mySettingsMenuComponent)
			mySettingsMenuComponent->UpdateSelectedButtons();
	}
	else if (Input::GetKeyPress(KeyCode::A))
	{
		if (mySettingsMenuIndex == 0)
		{
			myAudioManager->DecreaseMusicVolume();
			print("Change Music Volume - DOWN");
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		if (mySettingsMenuIndex == 1)
		{
			myAudioManager->DecreaseSFXVolume();
			print("Change SFX Volume - DOWN");
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		if (mySettingsMenuIndex == 2)
		{
			if (GetResolution() == 720)
			{
				DX11::SetResolution(3840, 2160);
				SetResolution(2160);
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			else if (GetResolution() == 1080)
			{
				DX11::SetResolution(1280, 720);
				SetResolution(720);
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			else if (GetResolution() == 2160)
			{
				DX11::SetResolution(1920, 1080);
				SetResolution(1080);
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			print("Change Resolution - DOWN");
		}

		if (mySettingsMenuComponent)
			mySettingsMenuComponent->UpdateSelectedButtons();
	}
}

void UIManager::P2SettingsInput()
{
	if (Input::GetKeyPress(KeyCode::DOWN))
	{
		ChangeSettingsIndex(1);
	}
	else if (Input::GetKeyPress(KeyCode::UP))
	{
		ChangeSettingsIndex(-1);
	}
	if (Input::GetKeyPress(KeyCode::RIGHT))
	{
		if (mySettingsMenuIndex == 0)
		{
			myAudioManager->IncreaseMusicVolume();
			print("Change Music Volume - UP");
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		if (mySettingsMenuIndex == 1)
		{
			myAudioManager->IncreaseSFXVolume();
			print("Change SFX Volume - UP");
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		if (mySettingsMenuIndex == 2)
		{
			if (GetResolution() == 720)
			{
				DX11::SetResolution(1920, 1080);
				SetResolution(1080);
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			else if (GetResolution() == 1080)
			{
				DX11::SetResolution(3840, 2160);
				SetResolution(2160);
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			else if (GetResolution() == 2160)
			{
				DX11::SetResolution(1280, 720);
				SetResolution(720);
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			print("Change Resolution - UP");
		}

		if (mySettingsMenuComponent)
			mySettingsMenuComponent->UpdateSelectedButtons();
	}
	else if (Input::GetKeyPress(KeyCode::LEFT))
	{
		if (mySettingsMenuIndex == 0)
		{
			myAudioManager->DecreaseMusicVolume();
			print("Change Music Volume - DOWN");
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		if (mySettingsMenuIndex == 1)
		{
			myAudioManager->DecreaseSFXVolume();
			print("Change SFX Volume - DOWN");
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		if (mySettingsMenuIndex == 2)
		{
			if (GetResolution() == 720)
			{
				DX11::SetResolution(3840, 2160);
				SetResolution(2160);
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			else if (GetResolution() == 1080)
			{
				DX11::SetResolution(1280, 720);
				SetResolution(720);
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			else if (GetResolution() == 2160)
			{
				DX11::SetResolution(1920, 1080);
				SetResolution(1080);
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			print("Change Resolution - DOWN");
		}

		if (mySettingsMenuComponent)
			mySettingsMenuComponent->UpdateSelectedButtons();
	}
}

void UIManager::LevelSelectShortcut()
{
	
	if (Input::GetKeyHeld(KeyCode::SHIFT))
	{
		if (Input::GetKeyPress(KeyCode::N1))
			myLevelToLoad = 1;
		else if (Input::GetKeyPress(KeyCode::N2))
			myLevelToLoad = 2;
		else if (Input::GetKeyPress(KeyCode::N3))
			myLevelToLoad = 3;
		else if (Input::GetKeyPress(KeyCode::N4))
			myLevelToLoad = 4;
		else if (Input::GetKeyPress(KeyCode::N5))
			myLevelToLoad = 5;
		else if (Input::GetKeyPress(KeyCode::N6))
			myLevelToLoad = 6;
		else if (Input::GetKeyPress(KeyCode::N7))
			myLevelToLoad = 7;
		else if (Input::GetKeyPress(KeyCode::N8))
			myLevelToLoad = 8;
		else if (Input::GetKeyPress(KeyCode::N9))
			myLevelToLoad = 9;
		string LevelToLoadString = "";
		switch (myLevelToLoad)
		{
		case 1:
			LevelToLoadString = "First_Level_lvl";
			break;
		case 2:
			LevelToLoadString = "Second_Level_lvl";
			break;
		case 3:
			LevelToLoadString = "Third_Level_lvl";
			break;
		case 4:
			LevelToLoadString = "Fourth_Level_lvl";
			break;
		case 5:
			LevelToLoadString = "Fifth_Level_lvl";
			break;
		case 6:
			LevelToLoadString = "Sixth_Level_lvl";
			break;
		case 7:
			LevelToLoadString = "Seventh_Level_lvl";
			break;
		case 8:
			LevelToLoadString = "Eighth_Level_lvl";
			break;
		case 9:
			LevelToLoadString = "Ninth_Level_lvl";
			break;
		default:
			break;
		}
		if (myLevelToLoad != -1)
		{
			ClearUIManager();
			myPlayer1Data->SetIsReady(false);
			myPlayer2Data->SetIsReady(false);
			SetP1IsInMainMenu(false);
			SetP2IsInMainMenu(false);
			GameManager::GetInstance()->SetIsShowingTutorial(true);
			GameManager::GetInstance()->SetBoatPos(GameManager::GetInstance()->GetBoatStartPos());
			LevelSelectManager::GetInstance()->SetCurrentLevel(myLevelToLoad);
			UIEventHandler::SetNextLevel(LevelToLoadString);
			myLevelToLoad = -1;
		}
	}
}

void UIManager::ChangeP1MenuIndex(int aChange)
{
	myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));

	myP1MenuIndex += aChange;
	if (myP1MenuIndex > 5)
	{
		myP1MenuIndex = 0;
	}
	else if(myP1MenuIndex < 0)
	{
		myP1MenuIndex = 5;
	}

	if (myP1MenuComponent)
		myP1MenuComponent->UpdateSelectedButtons();
}

void UIManager::ChangeP2MenuIndex(int aChange)
{
	myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));

	myP2MenuIndex += aChange;
	if (myP2MenuIndex > 5)
	{
		myP2MenuIndex = 0;
	}
	else if (myP2MenuIndex < 0)
	{
		myP2MenuIndex = 5;

	}
	if (myP2MenuComponent)
		myP2MenuComponent->UpdateSelectedButtons();
}

void UIManager::ChangeP1CustomizeMenuIndex(int aChange)
{
	myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));

	myP1CustomizeMenuIndex += aChange;
	if (myP1CustomizeMenuIndex > static_cast<int>(myP1CustomizeMenuButtons.size()) - 1)
	{
		myP1CustomizeMenuIndex = 0;
	}
	else if (myP1CustomizeMenuIndex < 0)
	{
		myP1CustomizeMenuIndex = myP1CustomizeMenuButtons.size() - 1;
	}

	if (myP1CustomizeMenuComponent)
		myP1CustomizeMenuComponent->UpdateSelectedButtons();
}

void UIManager::ChangeP2CustomizeMenuIndex(int aChange)
{
	myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));

	myP2CustomizeMenuIndex += aChange;
	if (myP2CustomizeMenuIndex > static_cast<int>(myP2CustomizeMenuButtons.size()) - 1)
	{
		myP2CustomizeMenuIndex = 0;
	}
	else if (myP2CustomizeMenuIndex < 0)
	{
		myP2CustomizeMenuIndex = myP2CustomizeMenuButtons.size() - 1;
	}

	if (myP2CustomizeMenuComponent)
		myP2CustomizeMenuComponent->UpdateSelectedButtons();
}

void UIManager::ChangeSettingsIndex(int aChange)
{
	myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));

	mySettingsMenuIndex += aChange;
	int maxSize = mySettingsButtons.size() - 1;
	if (mySettingsMenuIndex > maxSize)
	{
		mySettingsMenuIndex = 0;
	}
	else if (mySettingsMenuIndex < 0)
	{
		mySettingsMenuIndex = maxSize;
	}

	if (mySettingsMenuComponent)
		mySettingsMenuComponent->UpdateSelectedButtons();
}

void UIManager::CalculateXboxInputP1()
{
	myP1InputCD -= realDeltaTime;
	myP1XboxUp = ControllerP1->WasPressedThisFrame(CommonUtilities::Xbox::Up);
	myP1XboxDown = ControllerP1->WasPressedThisFrame(CommonUtilities::Xbox::Down);
	myP1XboxLeft = ControllerP1->WasPressedThisFrame(CommonUtilities::Xbox::Left);
	myP1XboxRight = ControllerP1->WasPressedThisFrame(CommonUtilities::Xbox::Right);
	if (myP1InputCD < 0.0)
	{
		myP1InputCD = myP1InputDelay;
		Vector2f input = ControllerP1->GetLeftStickAxis();
		Vector2f input2 = ControllerP1->GetRightStickAxis();
		if ((input.y > treshold && (input.x < treshold && input.x > -treshold)) || (input2.y > treshold && (input2.x < treshold && input2.x > -treshold)))
		{
			print("Up - P1"); 
			myP1XboxUp = true;
		}
		if ((input.y < -treshold && (input.x < treshold && input.x > -treshold)) || (input2.y < -treshold && (input2.x < treshold && input2.x > -treshold)))
		{
			print("Down - P1");
			myP1XboxDown = true;
		}
		if ((input.x < -treshold && (input.y < treshold && input.y > -treshold)) || (input2.x < -treshold && (input2.y < treshold && input2.y > -treshold)))
		{
			print("Left - P1");
			myP1XboxLeft = true;
		}
		if ((input.x > treshold && (input.y < treshold && input.y > -treshold)) || (input2.x > treshold && (input2.y < treshold && input2.y > -treshold)))
		{
			print("Right - P1");
			myP1XboxRight = true;
		}

	}
}

void UIManager::CalculateXboxInputP2()
{
	myP2InputCD -= realDeltaTime;
	myP2XboxUp = ControllerP2->WasPressedThisFrame(CommonUtilities::Xbox::Up);
	myP2XboxDown = ControllerP2->WasPressedThisFrame(CommonUtilities::Xbox::Down);
	myP2XboxLeft = ControllerP2->WasPressedThisFrame(CommonUtilities::Xbox::Left);
	myP2XboxRight = ControllerP2->WasPressedThisFrame(CommonUtilities::Xbox::Right);
	if (myP2InputCD < 0.0)
	{
		myP2InputCD = myP2InputDelay;
		Vector2f input = ControllerP2->GetLeftStickAxis();
		Vector2f input2 = ControllerP2->GetRightStickAxis();
		if ((input.y > treshold && (input.x < treshold && input.x > -treshold)) || (input2.y > treshold && (input2.x < treshold && input2.x > -treshold)))
		{
			print("Up - P2");
			myP2XboxUp = true;
		}
		if ((input.y < -treshold && (input.x < treshold && input.x > -treshold)) || (input2.y < -treshold && (input2.x < treshold && input2.x > -treshold)))
		{
			print("Down - P2");
			myP2XboxDown = true;
		}
		if ((input.x < -treshold && (input.y < treshold && input.y > -treshold)) || (input2.x < -treshold && (input2.y < treshold && input2.y > -treshold)))
		{
			print("Left - P2");
			myP2XboxLeft = true;
		}
		if ((input.x > treshold && (input.y < treshold && input.y > -treshold)) || (input2.x > treshold && (input2.y < treshold && input2.y > -treshold)))
		{
			print("Right - P2");
			myP2XboxRight = true;
		}

	}
}

void UIManager::resetP1XboxInput()
{
	myP1XboxUp = false;
	myP1XboxDown = false;
	myP1XboxLeft = false;
	myP1XboxRight = false;
}

void UIManager::resetP2XboxInput()
{
	myP2XboxUp = false;
	myP2XboxDown = false;
	myP2XboxLeft = false;
	myP2XboxRight = false;
}

void UIManager::Update()
{
	LevelSelectShortcut();

	if (ControllerP1->IsConnected())
	{
		CalculateXboxInputP1();
	}
	if (ControllerP2->IsConnected())
	{
		CalculateXboxInputP2();
	}
	//Updates
	if (myP1IsInMainMenu || myP1IsInCustomize || myP2IsInCustomize || myP1IsInSettings || myP2IsInSettings)
	{
		LoadLevelSelect();
		//KeyBoard Input
		
		UpdateP1Keyboard();
		UpdateP2Keyboard();
		UpdateP1XboxController();
		UpdateP2XboxController();
		//Update Buttons in Menus
	}
	//Updates while in Game
	if (myPauseMenuComponent)
	{
		myPauseMenuComponent->UpdateSelectedButtons();
		HandlePauseMenu();
	}
}

void UIManager::Init()
{
	ControllerP1 = GameManager::GetInstance()->GetPlayerController(1);
	ControllerP2 = GameManager::GetInstance()->GetPlayerController(2);
	myPlayer1Data = GameManager::GetInstance()->GetPlayer1Data();
	myPlayer2Data = GameManager::GetInstance()->GetPlayer2Data();
	myAudioManager = Engine::GetInstance()->GetAudioManager();
}

void UIManager::HandlePauseMenu()
{
	if (myPauseMenuComponent && GameManager::GetInstance()->IsInCountdown() == false)
	{
		const bool showingTutorialBoard = GameManager::GetInstance()->GetIsShowingTutorial();
		if (Input::GetKeyPress(KeyCode::ESCAPE) || GameManager::GetInstance()->GetPlayerController(1)->WasPressedThisFrame(CommonUtilities::Xbox::Start))
		{
			if (myIsInPauseMenu)
			{
				if (!myPauseMenuComponent->IsFading())
				{
					Engine::GetInstance()->SetGamePaused(false);
					myIsInPauseMenu = false;
					GetPauseMenuComponent()->ToggleFadeOut(true);
					mySettingsMenuComponent->ToggleFadeOut(true);
					SetP1IsInSettings(false);
					SetP2IsInSettings(false);

					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_OPEN_MENU));
				}
			}
			else
			{
				if (showingTutorialBoard == false)
				{
					if (!myPauseMenuComponent->IsFading())
					{
						Engine::GetInstance()->SetGamePaused(true);
						myIsInPauseMenu = true;
						GetPauseMenuComponent()->ToggleFadeIn(true);

						myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_CLOSE_MENU));
					}
				}
			}
		}
		if (myIsInPauseMenu)
		{
			if ((Input::GetKeyPress(KeyCode::W) || myP1XboxUp == true) && !myP1IsInSettings)
			{
				resetP1XboxInput();
				myPauseMenuComponent->GetTransform()->GetChildren()[3]->GetChildren()[0]->GetGameObject()->SetActive(false);
				myPauseMenuComponent->GetTransform()->GetChildren()[3]->GetChildren()[1]->GetGameObject()->SetActive(false);
				myPauseMenuIndex--;
				if (myPauseMenuIndex < 0)
					myPauseMenuIndex = myPauseMenuButtons.size() - 1;

				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			else if ((Input::GetKeyPress(KeyCode::S) || myP1XboxDown == true) && !myP1IsInSettings)
			{
				resetP1XboxInput();
				myPauseMenuComponent->GetTransform()->GetChildren()[3]->GetChildren()[0]->GetGameObject()->SetActive(false);
				myPauseMenuComponent->GetTransform()->GetChildren()[3]->GetChildren()[1]->GetGameObject()->SetActive(false);
				myPauseMenuIndex++;
				if (myPauseMenuIndex > myPauseMenuButtons.size() - 1)
					myPauseMenuIndex = 0;

				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			if ((Input::GetKeyPress(KeyCode::S) || myP1XboxDown == true) && myP1IsInSettings)
			{
				resetP1XboxInput();
				ChangeSettingsIndex(1);
			}
			else if ((Input::GetKeyPress(KeyCode::W) || myP1XboxUp == true) && myP1IsInSettings)
			{
				resetP1XboxInput();
				ChangeSettingsIndex(-1);
			}
			if ((Input::GetKeyPress(KeyCode::D) || myP1XboxRight == true) && myP1IsInSettings)
			{
				resetP1XboxInput();
				if (mySettingsMenuIndex == 0)
				{
					myAudioManager->IncreaseMusicVolume();
					print("Change Music Volume - UP");
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				if (mySettingsMenuIndex == 1)
				{
					myAudioManager->IncreaseSFXVolume();
					print("Change SFX Volume - UP");
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				if (mySettingsMenuIndex == 2)
				{
					if (GetResolution() == 720)
					{
						DX11::SetResolution(1920, 1080);
						SetResolution(1080);
						myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
					}
					else if (GetResolution() == 1080)
					{
						DX11::SetResolution(1280, 720);
						SetResolution(720);
						myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
					}
					print("Change Resolution - UP");
				}

				if (mySettingsMenuComponent)
					mySettingsMenuComponent->UpdateSelectedButtons();
			}
			else if ((Input::GetKeyPress(KeyCode::A) || myP1XboxLeft == true) && myP1IsInSettings)
			{
				resetP1XboxInput();
				if (mySettingsMenuIndex == 0)
				{
					myAudioManager->DecreaseMusicVolume();
					print("Change Music Volume - DOWN");
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				if (mySettingsMenuIndex == 1)
				{
					myAudioManager->DecreaseSFXVolume();
					print("Change SFX Volume - DOWN");
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				if (mySettingsMenuIndex == 2)
				{
					if (GetResolution() == 720)
					{
						DX11::SetResolution(1920, 1080);
						SetResolution(1080);
						myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
					}
					else if (GetResolution() == 1080)
					{
						DX11::SetResolution(1280, 720);
						SetResolution(720);
						myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
					}
					print("Change Resolution - DOWN");
				}

				if (mySettingsMenuComponent)
					mySettingsMenuComponent->UpdateSelectedButtons();
			}
			//Select
			if (Input::GetKeyPress(KeyCode::SPACE) || GameManager::GetInstance()->GetPlayerController(1)->WasPressedThisFrame(CommonUtilities::Xbox::A))
			{
				if (myPauseMenuComponent && !myP1IsInSettings)
					myPauseMenuComponent->ClickSelectedButton();
				if (mySettingsMenuComponent && myP1IsInSettings)
					mySettingsMenuComponent->ClickSelectedButton();

				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_CHOOSE));
			}
		}
	}
}

void UIManager::UpdateP1Keyboard()
{
	if (ControllerP1->IsConnected())
	{
		return;
	}
	//Connect
	if (Input::GetKeyPress(KeyCode::SPACE) && myP1IsInMainMenu)
	{
		if (myShowingFullscreenControlls == false)
		{
			if (myNumberOfPlayersComponent && myP1HasConnected == false)
			{
				if (mySplashScreenShows == true)
				{
					myInteractableUIObjectComponent->GetTransform()->GetChildren()[0]->GetGameObject()->SetActive(false);
					mySplashScreenShows = false;
				}
				else
				{
					myP1HasConnected = true;
					myNumberOfPlayersComponent->AddPlayerToGame();
					myP1MenuComponent->ToggleFadeIn(true);
					myPlayer1Object->SetActive(true);
					myInteractableUIObjectComponent->GetTransform()->GetChildren()[3]->GetGameObject()->SetActive(false);
					//myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_CHOOSE));
					return;
				}
			}

			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_CHOOSE));
			myShowingFullscreenControlls = true;
		}
	}

	//Navigate Main Menu
	if (myP1HasConnected)
	{
		if (Input::GetKeyPress(KeyCode::S) && myP1IsInMainMenu && !myPlayer1Data->GetIsReady())
		{
			if (myP1IsInCredits)
			{
				GetInteractableUIObjectComponent()->ToggleP1CreditsFadeOut(true);
			}
			if (myP1IsInControls)
			{
				GetInteractableUIObjectComponent()->ToggleP1ControlsFadeOut(true);
			}
			ChangeP1MenuIndex(1);
		}
		else if (Input::GetKeyPress(KeyCode::W) && myP1IsInMainMenu && !myPlayer1Data->GetIsReady())
		{
			if (myP1IsInCredits)
			{
				GetInteractableUIObjectComponent()->ToggleP1CreditsFadeOut(true);
			}			
			if (myP1IsInControls)
			{
				GetInteractableUIObjectComponent()->ToggleP1ControlsFadeOut(true);
			}
			ChangeP1MenuIndex(-1);
		}
		if (myP1IsInCustomize)
		{
			P1CustomizeInput();
		}
		//Navigate Settings Menu
		if (myP1IsInSettings)
		{
			P1SettingsInput();
		}
		
		//Select
		if (Input::GetKeyPress(KeyCode::SPACE))
		{
			if (myP1MenuComponent && myP1IsInMainMenu)
				myP1MenuComponent->ClickSelectedButton();
			if (myP1CustomizeMenuComponent && myP1IsInCustomize)
				myP1CustomizeMenuComponent->ClickSelectedButton();
			if (mySettingsMenuComponent && myP1IsInSettings)
				mySettingsMenuComponent->ClickSelectedButton();

			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_CHOOSE));
		}
	}
}

void UIManager::UpdateP2Keyboard()
{
	if (ControllerP2->IsConnected())
	{
		return;
	}
	//Connect
	if (Input::GetKeyPress(KeyCode::RETURN) && myP2IsInMainMenu)
	{
		if (myNumberOfPlayersComponent && myP2HasConnected == false && mySplashScreenShows == false && myNumberOfPlayersComponent->GetNumberOfPlayers() > 0)
		{
			myP2HasConnected = true;
			myNumberOfPlayersComponent->AddPlayerToGame();
			myP2MenuComponent->ToggleFadeIn(true);
			myPlayer2Object->SetActive(true);
			myInteractableUIObjectComponent->GetTransform()->GetChildren()[4]->GetGameObject()->SetActive(false);
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_CHOOSE));
			return;
		}
	}
	if (myP2HasConnected)
	{

		//Navigate Main Menu
		if (Input::GetKeyPress(KeyCode::DOWN) && myP2IsInMainMenu && !myPlayer2Data->GetIsReady())
		{
			if (myP2IsInCredits)
			{
				GetInteractableUIObjectComponent()->ToggleP2CreditsFadeOut(true);
			}
			if (myP2IsInControls)
			{
				GetInteractableUIObjectComponent()->ToggleP2ControlsFadeOut(true);
			}
			ChangeP2MenuIndex(1);

			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		else if (Input::GetKeyPress(KeyCode::UP) && myP2IsInMainMenu && !myPlayer2Data->GetIsReady())
		{
			if (myP2IsInCredits)
			{
				GetInteractableUIObjectComponent()->ToggleP2CreditsFadeOut(true);
			}
			if (myP2IsInControls)
			{
				GetInteractableUIObjectComponent()->ToggleP2ControlsFadeOut(true);
			}
			ChangeP2MenuIndex(-1);

			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}

		//Navigate Customize
		if (myP2IsInCustomize)
		{
			P2CustomizeInput();
		}
		
		//Navigate Settings Menu
		if (myP2IsInSettings)
		{
			P2SettingsInput();
		}
		
		//Select
		if (Input::GetKeyPress(KeyCode::RETURN))
		{
			if (myP2MenuComponent && myP2IsInMainMenu)
				myP2MenuComponent->ClickSelectedButton();
			if (myP2CustomizeMenuComponent && myP2IsInCustomize)
				myP2CustomizeMenuComponent->ClickSelectedButton();
			if (mySettingsMenuComponent && myP2IsInSettings)
				mySettingsMenuComponent->ClickSelectedButton();

			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_CHOOSE));
		}
	}
}

void UIManager::UpdateP1XboxController()
{
	if (!ControllerP1->IsConnected())
	{
		return;
	}
	//Connect
	if (ControllerP1->WasPressedThisFrame(CommonUtilities::Xbox::A) && myP1IsInMainMenu && mySplashScreenShows == false)
	{
		//Update UI Sprite in main menu
		if (myNumberOfPlayersComponent && myP1HasConnected == false)
		{
			myP1HasConnected = true;
			myNumberOfPlayersComponent->AddPlayerToGame();
			myP1MenuComponent->ToggleFadeIn(true);
			myPlayer1Object->SetActive(true);
			myInteractableUIObjectComponent->GetTransform()->GetChildren()[3]->GetGameObject()->SetActive(false);
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_CHOOSE));
			//Update controller ID
			return;
		}
	}
	if (ControllerP1->WasPressedThisFrame(CommonUtilities::Xbox::A) && myP1IsInMainMenu)
	{
		if (myNumberOfPlayersComponent && myP1HasConnected == false && mySplashScreenShows == true)
		{
			myInteractableUIObjectComponent->GetTransform()->GetChildren()[0]->GetGameObject()->SetActive(false);
			mySplashScreenShows = false;
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_CHOOSE));
		}
	}
	//Navigate Main Menu
	if (myP1HasConnected)
	{
		if (myP1XboxDown == true && myP1IsInMainMenu && !myPlayer1Data->GetIsReady())
		{
			resetP1XboxInput();
			if (myP1IsInCredits)
			{
				GetInteractableUIObjectComponent()->ToggleP1CreditsFadeOut(true);
			}
			if (myP1IsInControls)
			{
				GetInteractableUIObjectComponent()->ToggleP1ControlsFadeOut(true);
			}
			ChangeP1MenuIndex(1);
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		else if (myP1XboxUp == true && myP1IsInMainMenu && !myPlayer1Data->GetIsReady())
		{
			resetP1XboxInput();
			if (myP1IsInCredits)
			{
				GetInteractableUIObjectComponent()->ToggleP1CreditsFadeOut(true);
			}
			if (myP1IsInControls)
			{
				GetInteractableUIObjectComponent()->ToggleP1ControlsFadeOut(true);
			}
			ChangeP1MenuIndex(-1);
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		//Navigate Customize
		if (myP1XboxDown == true && myP1IsInCustomize)
		{
			resetP1XboxInput();
			ChangeP1CustomizeMenuIndex(1);
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		else if (myP1XboxUp == true && myP1IsInCustomize)
		{
			resetP1XboxInput();
			ChangeP1CustomizeMenuIndex(-1);
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		if (myP1XboxRight == true && myP1IsInCustomize)
		{
			resetP1XboxInput();
			if (myP1CustomizeMenuIndex == 0)
			{
				int HatID = myPlayer1Data->GetHatID();
				myPlayer1Data->SetHatID(++HatID);
				HatID = myPlayer1Data->GetHatID();
				const std::string& hatName = GameManager::GetInstance()->GetMyHatsVector()[HatID];
				myInteractableUIObjectComponent->GetTransform()->GetChildren()[2]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>(GameManager::GetInstance()->GetMyHatsVector()[HatID]));

				const string& MatName = hatName.substr(0, hatName.size() - 4) + "mat";
				myInteractableUIObjectComponent->GetTransform()->GetChildren()[2]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>(MatName), 0);
				print("Hat ID Player 1 Increased");
				//Update HatMesh
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
		}
		else if (myP1XboxLeft == true && myP1IsInCustomize)
		{
			resetP1XboxInput();
			if (myP1CustomizeMenuIndex == 0)
			{
				int HatID = myPlayer1Data->GetHatID();
				myPlayer1Data->SetHatID(--HatID);
				HatID = myPlayer1Data->GetHatID();
				const std::string& hatName = GameManager::GetInstance()->GetMyHatsVector()[HatID];
				myInteractableUIObjectComponent->GetTransform()->GetChildren()[2]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>(GameManager::GetInstance()->GetMyHatsVector()[HatID]));

				const string& MatName = hatName.substr(0, hatName.size() - 4) + "mat";
				myInteractableUIObjectComponent->GetTransform()->GetChildren()[2]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>(MatName), 0);
				print("Hat ID Player 1 Decreased");
				//Update HatMesh
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
		}
		//Navigate Settings Menu
		if (myP1XboxDown == true && myP1IsInSettings)
		{
			resetP1XboxInput();
			ChangeSettingsIndex(1);
		}
		else if (myP1XboxUp == true && myP1IsInSettings)
		{
			resetP1XboxInput();
			ChangeSettingsIndex(-1);
		}
		if (myP1XboxRight == true && myP1IsInSettings)
		{
			resetP1XboxInput();
			if (mySettingsMenuIndex == 0)
			{
				myAudioManager->IncreaseMusicVolume();
				//print("Change Music Volume - UP");
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			if (mySettingsMenuIndex == 1)
			{
				myAudioManager->IncreaseSFXVolume();
				//print("Change SFX Volume - UP");
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			if (mySettingsMenuIndex == 2)
			{
				if (GetResolution() == 720)
				{
					DX11::SetResolution(1920, 1080);
					SetResolution(1080);
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				else if (GetResolution() == 1080)
				{
					DX11::SetResolution(3840, 2160);
					SetResolution(2160);
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				else if (GetResolution() == 2160)
				{
					DX11::SetResolution(1280, 720);
					SetResolution(720);
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				print("Change Resolution - UP");
			}
		}
		else if (myP1XboxLeft == true && myP1IsInSettings)
		{
			resetP1XboxInput();
			if (mySettingsMenuIndex == 0)
			{
				myAudioManager->DecreaseMusicVolume();
				print("Change Music Volume - DOWN");
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			if (mySettingsMenuIndex == 1)
			{
				myAudioManager->DecreaseSFXVolume();
				print("Change SFX Volume - DOWN");
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			if (mySettingsMenuIndex == 2)
			{
				if (GetResolution() == 720)
				{
					DX11::SetResolution(3840, 2160);
					SetResolution(2160);
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				else if (GetResolution() == 1080)
				{
					DX11::SetResolution(1280, 720);
					SetResolution(720);
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				else if (GetResolution() == 2160)
				{
					DX11::SetResolution(1920, 1080);
					SetResolution(1080);
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				print("Change Resolution - DOWN");
			}
		}
		//Select
		if (ControllerP1->WasPressedThisFrame(CommonUtilities::Xbox::A))
		{
			if (myP1MenuComponent && myP1IsInMainMenu)
				myP1MenuComponent->ClickSelectedButton();
			if (myP1CustomizeMenuComponent && myP1IsInCustomize)
				myP1CustomizeMenuComponent->ClickSelectedButton();
			if (mySettingsMenuComponent && myP1IsInSettings)
				mySettingsMenuComponent->ClickSelectedButton();
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_CHOOSE));
		}
	}
}

void UIManager::UpdateP2XboxController()
{
	if (!ControllerP2->IsConnected())
	{
		return;
	}
	//Connect
	if (ControllerP2->WasPressedThisFrame(CommonUtilities::Xbox::A) && myP2IsInMainMenu && mySplashScreenShows == false)
	{
		if (myNumberOfPlayersComponent && myP2HasConnected == false)
		{
			myP2HasConnected = true;
			myNumberOfPlayersComponent->AddPlayerToGame();
			//myP2MenuComponent->ShowP2MenuButtons();
			myP2MenuComponent->ToggleFadeIn(true);
			myPlayer2Object->SetActive(true);
			myInteractableUIObjectComponent->GetTransform()->GetChildren()[4]->GetGameObject()->SetActive(false);
			return;
		}
	}
	if (myP2HasConnected)
	{

		//Navigate Main Menu
		if (myP2XboxDown == true && myP2IsInMainMenu && !myPlayer2Data->GetIsReady())
		{
			resetP2XboxInput();
			if (myP2IsInCredits)
			{
				GetInteractableUIObjectComponent()->ToggleP2CreditsFadeOut(true);
			}
			if (myP2IsInControls)
			{
				GetInteractableUIObjectComponent()->ToggleP2ControlsFadeOut(true);
			}
			myP2MenuIndex++;
			if (myP2MenuIndex > 5)
			{
				myP2MenuIndex = 0;
			}
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		else if (myP2XboxUp == true && myP2IsInMainMenu && !myPlayer2Data->GetIsReady())
		{
			resetP2XboxInput();
			if (myP2IsInCredits)
			{
				GetInteractableUIObjectComponent()->ToggleP2CreditsFadeOut(true);
			}
			if (myP1IsInControls)
			{
				GetInteractableUIObjectComponent()->ToggleP2ControlsFadeOut(true);
			}
			ChangeP2MenuIndex(-1);
		}

		//Navigate Customize
		if (myP2XboxDown == true && myP2IsInCustomize)
		{
			resetP2XboxInput();
			myP2CustomizeMenuIndex++;
			if (myP2CustomizeMenuIndex > myP2CustomizeMenuButtons.size() - 1)
			{
				myP2CustomizeMenuIndex = 0;
			}
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		else if (myP2XboxUp == true && myP2IsInCustomize)
		{
			resetP2XboxInput();
			ChangeP2CustomizeMenuIndex(-1);
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		if (myP2XboxRight == true && myP2IsInCustomize)
		{
			resetP2XboxInput();
			if (myP2CustomizeMenuIndex == 0)
			{
				int HatID = myPlayer2Data->GetHatID();
				myPlayer2Data->SetHatID(++HatID);
				HatID = myPlayer2Data->GetHatID();
				const std::string& hatName = GameManager::GetInstance()->GetMyHatsVector()[HatID];
				myInteractableUIObjectComponent->GetTransform()->GetChildren()[1]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>(GameManager::GetInstance()->GetMyHatsVector()[HatID]));

				const string& MatName = hatName.substr(0, hatName.size() - 4) + "mat";
				myInteractableUIObjectComponent->GetTransform()->GetChildren()[1]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>(MatName), 0);

				print("Hat ID Player 2 Increased");
				//Update HatMesh
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
		}
		else if (myP2XboxLeft == true && myP2IsInCustomize)
		{
			resetP2XboxInput();
			if (myP2CustomizeMenuIndex == 0)
			{
				int HatID = myPlayer2Data->GetHatID();
				myPlayer2Data->SetHatID(--HatID);
				HatID = myPlayer2Data->GetHatID();
				const std::string& hatName = GameManager::GetInstance()->GetMyHatsVector()[HatID];
				myInteractableUIObjectComponent->GetTransform()->GetChildren()[1]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>(GameManager::GetInstance()->GetMyHatsVector()[HatID]));

				const string& MatName = hatName.substr(0, hatName.size() - 4) + "mat";
				myInteractableUIObjectComponent->GetTransform()->GetChildren()[1]->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>(MatName), 0);

				print("Hat ID Player 2 Decreased");
				//Update HatMesh
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
		}
		//Navigate Settings Menu
		if (myP2XboxDown == true && myP2IsInSettings)
		{
			resetP2XboxInput();
			mySettingsMenuIndex++;
			if (mySettingsMenuIndex > mySettingsButtons.size() - 1)
			{
				mySettingsMenuIndex = 0;
			}
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		else if (myP2XboxUp == true && myP2IsInSettings)
		{
			resetP2XboxInput();
			ChangeSettingsIndex(-1);
			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
		}
		if (myP2XboxRight == true && myP2IsInSettings)
		{
			resetP2XboxInput();
			if (mySettingsMenuIndex == 0)
			{
				myAudioManager->IncreaseMusicVolume();
				print("Change Music Volume - UP");
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			if (mySettingsMenuIndex == 1)
			{
				myAudioManager->IncreaseSFXVolume();
				print("Change SFX Volume - UP");
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			if (mySettingsMenuIndex == 2)
			{
				if (GetResolution() == 720)
				{
					DX11::SetResolution(1920, 1080);
					SetResolution(1080);
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				else if (GetResolution() == 1080)
				{
					DX11::SetResolution(3840, 2160);
					SetResolution(2160);
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				else if (GetResolution() == 2160)
				{
					DX11::SetResolution(1280, 720);
					SetResolution(720);
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				print("Change Resolution - UP");
			}
		}
		else if (myP2XboxLeft == true && myP2IsInSettings)
		{
			resetP2XboxInput();
			if (mySettingsMenuIndex == 0)
			{
				myAudioManager->DecreaseMusicVolume();
				print("Change Music Volume - DOWN");
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			if (mySettingsMenuIndex == 1)
			{
				myAudioManager->DecreaseSFXVolume();
				print("Change SFX Volume - DOWN");
				myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
			}
			if (mySettingsMenuIndex == 2)
			{
				if (GetResolution() == 720)
				{
					DX11::SetResolution(3840, 2160);
					SetResolution(2160);
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				else if (GetResolution() == 1080)
				{
					DX11::SetResolution(1280, 720);
					SetResolution(720);
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				else if (GetResolution() == 2160)
				{
					DX11::SetResolution(1920, 1080);
					SetResolution(1080);
					myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_NAVIGATE));
				}
				print("Change Resolution - DOWN");
			}
		}
		//Select
		if (ControllerP2->WasPressedThisFrame(CommonUtilities::Xbox::A))
		{
			if (myP2MenuComponent && myP2IsInMainMenu)
				myP2MenuComponent->ClickSelectedButton();
			if (myP2CustomizeMenuComponent && myP2IsInCustomize)
				myP2CustomizeMenuComponent->ClickSelectedButton();
			if (mySettingsMenuComponent && myP2IsInSettings)
				mySettingsMenuComponent->ClickSelectedButton();

			myAudioManager->PlaySoundEffectByFullPath(std::string(AUDIO_BUTTON_CHOOSE));
		}
	}
}
