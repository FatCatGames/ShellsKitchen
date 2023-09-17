#include "Game.pch.h"
#include "GameManager.h"
#include "Assets/Model.h"
#include "Components\UI\UISprite.h"
#include "Components\GamePlay\OrderUI.h"
#include "Components\Scoreboard\Scoreboard.h"
#include <ComponentTools/UIEventHandler.h>
#include "ComponentTools\SceneManager.h"
#include "Components\MainMenu\CountdownComponent.h"
#include "TimeManager.h"
#include "..\Game\Managers\LevelSelectManager.h"
#include "..\Game\Shaders\UI\Counter\UICounterPS.h"
#include "Components/UI/VideoPlayer.h"
#include "Managers/StampManager.h"

GameManager* GameManager::Instance;

GameManager::GameManager()
{
	Instance = this;

	for (size_t i = 0; i < MAX_NUMBERS_OF_PLAYERS; i++)
	{
		myConnectedPlayers.push_back(false);
	}
}

GameManager::~GameManager()
{
	delete myPlayer1Data;
	delete myPlayer2Data;
	delete myP1Controller;
	delete myP2Controller;
	delete myP3Controller;
	delete myP4Controller;
}

int GameManager::ConnectControllerToID()
{
	for (int i = 0; i < myConnectedPlayers.size(); i++)
	{
		if (myConnectedPlayers[i] == false)
		{
			myConnectedPlayers[i] = true;
			return i + 1;
		}
	}
	return -1;
}

void GameManager::Init()
{
	myP1Controller = new CommonUtilities::XboxControllerInput(CommonUtilities::XboxPlayer::One);
	myP2Controller = new CommonUtilities::XboxControllerInput(CommonUtilities::XboxPlayer::Two);
	myP3Controller = new CommonUtilities::XboxControllerInput(CommonUtilities::XboxPlayer::Three);
	myP4Controller = new CommonUtilities::XboxControllerInput(CommonUtilities::XboxPlayer::Four);
	SetControls();
}

void GameManager::SetControls()
{
	myP1Controls.up = KeyCode::W;
	myP1Controls.left = KeyCode::A;
	myP1Controls.down = KeyCode::S;
	myP1Controls.right = KeyCode::D;

	myP1Controls.pickup = KeyCode::T;
	myP1Controls.interact = KeyCode::Y;
	myP1Controls.dash = KeyCode::U;
	myP1Controls.emote = KeyCode::G;

	myP2Controls.up = KeyCode::UP;
	myP2Controls.left = KeyCode::LEFT;
	myP2Controls.down = KeyCode::DOWN;
	myP2Controls.right = KeyCode::RIGHT;

	myP2Controls.pickup = KeyCode::OEM_COMMA;
	myP2Controls.interact = KeyCode::OEM_PERIOD;
	myP2Controls.dash = KeyCode::OEM_MINUS;
	myP2Controls.emote = KeyCode::M;


	myP1Controls.xboxPickup = CommonUtilities::Xbox::A;
	myP1Controls.xboxInteract = CommonUtilities::Xbox::X;
	myP1Controls.xboxDash = CommonUtilities::Xbox::B;
	myP1Controls.xboxEmote = CommonUtilities::Xbox::Y;

	myP2Controls.xboxPickup = CommonUtilities::Xbox::A;
	myP2Controls.xboxInteract = CommonUtilities::Xbox::X;
	myP2Controls.xboxDash = CommonUtilities::Xbox::B;
	myP2Controls.xboxEmote = CommonUtilities::Xbox::Y;

	myP3Controls.xboxPickup = CommonUtilities::Xbox::A;
	myP3Controls.xboxInteract = CommonUtilities::Xbox::X;
	myP3Controls.xboxDash = CommonUtilities::Xbox::B;
	myP3Controls.xboxEmote = CommonUtilities::Xbox::Y;

	myP4Controls.xboxPickup = CommonUtilities::Xbox::A;
	myP4Controls.xboxInteract = CommonUtilities::Xbox::X;
	myP4Controls.xboxDash = CommonUtilities::Xbox::B;
	myP4Controls.xboxEmote = CommonUtilities::Xbox::Y;

}

void GameManager::LoadHatsToVector()
{
	if (myHasLoadedHatsVector == false)
	{
		auto aStart = std::chrono::system_clock::now();
		std::vector<std::shared_ptr<Asset>> myModels = AssetRegistry::GetInstance()->GetAssets<Model>();
		for (int i = 0; i < myModels.size(); i++)
		{
			if (myModels[i]->GetName().find("_hat") != std::string::npos)
			{				
				myHats.push_back(myModels[i]->GetName());
			}
		}
		auto aEnd = std::chrono::system_clock::now();
		double time = std::chrono::duration<double>(aEnd - aStart).count();
		print(to_string(time) + " Time to load Hats");
		myHasLoadedHatsVector = true;
	}
}

std::vector<string>& GameManager::GetMyHatsVector()
{
	if (myHasLoadedHatsVector == true)
	{
		return myHats;
	}
	else
	{
		LoadHatsToVector();
		return myHats;
	}
}

void GameManager::Update()
{
	if (myP1Controller)
	{
		myP1Controller->Refresh();
	}
	if (myP2Controller)
	{
		myP2Controller->Refresh();
	}
	if (myP3Controller)
	{
		myP3Controller->Refresh();
	}
	if (myP4Controller)
	{
		myP4Controller->Refresh();
	}

	if (myLevelCompleted)
	{

		myShouldShowChange = false;
		myScoreBoardTimer += deltaTime;

		if (myScoreBoardTimer > 1.5f)
		{

			Engine::GetInstance()->SetGamePaused(true);
			if (Input::GetKeyPress(KeyCode::SPACE) || Input::GetController().WasPressedThisFrame(CommonUtilities::Xbox::A))
			{
				Engine::GetInstance()->SetGamePaused(false);
				myScoreBoardTimer = 0.0f;

				bool completed = false;
				if (LevelSelectManager::GetInstance()->GetBronzeScore(LevelSelectManager::GetInstance()->GetCurrentLevel() - 1) < myCurrentLevelTotalScore)
				{
					completed = true;
				}
				SetLevelComplete(false);
				GenerateOrders(false);
				ResetLevel();

				bool shouldWait = false;

				if (completed)
				{
					for (int i = 0; i < myCutScenesLevels.size(); i++)
					{
						if (myCutScenesLevels[i] == Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->GetName())
						{
							auto object = InstantiatePrefab(std::string(myCutScenesLevels[i].begin(), myCutScenesLevels[i].end() - 3) + "cutScene");
							object->GetTransform()->SetWorldPos(0, 0, 0);
							auto videoPlayer = object->GetComponent<VideoPlayer>();
							shouldWait = true;
							Listener aListener;
							aListener.action = [&] { UIEventHandler::SetNextLevel("LevelSelect");  };
							videoPlayer->AddListener(aListener);
						}
					}
				}

				if (!shouldWait)
				{
					UIEventHandler::SetNextLevel("LevelSelect");
				}
			}
		}
	}

	if (myShouldRemoveTutorialAudioPlayer == true)
	{
		if (myTutorialBoardAudioPlayer)
		{
			myShouldRemoveTutorialAudioPlayer = false;
			myTutorialBoardAudioPlayer->Destroy();
			myTutorialBoard = nullptr;
		}
	}

	if (myShowingTutorialBoard == true)
	{
		if (myTutorialBoard == nullptr)
		{
			CreateTutorialBoard();
		}

		if (myCanCloseTutorial == false)
		{
			myTutorialFreezeTime += realDeltaTime;
		}
		if (myTutorialFreezeTime > 0.1f)
		{
			myCanCloseTutorial = true;
		}
		if (myCanCloseTutorial)
		{
			if (Input::GetKeyPress(KeyCode::SPACE) || Input::GetKeyPress(KeyCode::ESCAPE) || Input::GetController().WasPressedThisFrame(CommonUtilities::Xbox::A))
			{
				if (myTutorialBoard != nullptr)
				{
					myTutorialBoard->Destroy();
				}
				myTutorialBoard = nullptr;
				myShowingTutorialBoard = false;
				myCanCloseTutorial = false;
				myTutorialFreezeTime = 0.0f;

				UIManager::GetInstance()->GetCountdownComponent()->StartCountdown();
			}
		}
	}
	if (myShouldGenerateOrders)
	{
		myOrderHandler.Update();
	}

	if (myShouldShowChange == true)
	{
		myChangeTimer += deltaTime;

		const float changeDuration = 2.0f;
		if (myChangeTimer < changeDuration)
		{
			if(myLevelCoinCounter)
			{
				auto transform = myLevelCoinCounter->GetTransform();
				if (transform) 
				{
					auto object = transform->GetChildren();
					if (object.size() >= 1)
					{
						auto component = object[0]->GetGameObject()->GetComponent<UISprite>();
						if (component)
						{
							auto counterShader = dynamic_cast<UICounterPS*>(component->GetPixelShader().get());
							if (counterShader)
							{
								counterShader->myUICounterData.Driver = myChangeTimer / 2.0f;
								counterShader->myUICounterData.Points = myLastChange;
								counterShader->myUICounterData.TotalPoints = myCurrentLevelTotalScore;
								counterShader->SetData();
							}

						}
					}
				}
			}
		}

		if (myChangeTimer > changeDuration) 
		{
			myShouldShowChange = false;
			myChangeTimer = 0;
		}
	}
}

void GameManager::SetLevelSelectCamera(LevelSelectCamera* aCamera)
{
	myLevelSelectCamera = aCamera;
}

void GameManager::SetLevelSelectPlayer(LevelSelectPlayer* aPlayer)
{
	myLevelSelectPlayer = aPlayer;
}

int GameManager::AddPlayer(Player* aPlayer)
{
	myPlayers.push_back(aPlayer);
	return myPlayers.size();
}

void GameManager::RemovePlayer(Player* aPlayer)
{
	for (size_t i = 0; i < myPlayers.size(); i++)
	{
		if (aPlayer == myPlayers[i])
		{
			myPlayers.erase(myPlayers.begin() + i);
		}
	}
}

void GameManager::AddOrderSprite(Order anOrder, const int& anOrderIndex)
{
	if (true)
	{
		GameObject* newOrder = InstantiatePrefab("Order");

		UISprite* sprite = nullptr;
		if (newOrder)
		{
			sprite = newOrder->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>();
		}

		if (sprite)
		{
			switch (anOrderIndex)
			{
			case 0:
				sprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Recipe00"));
				break;
			case 1:
				sprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Recipe01"));
				break;
			case 2:
				sprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Recipe02"));
				break;
			case 3:
				sprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Recipe03"));
				break;
			case 4:
				sprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Recipe04"));
				break;
			case 5:
				sprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Recipe05"));
				break;
			case 6:
				sprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Recipe06"));
				break;
			case 7:
				sprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Recipe07"));
				break;
			case 8:
				sprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Recipe08"));
				break;
			case 9:
				sprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Recipe09"));
				break;
			case 10:
				sprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Recipe10"));
				break;
			case 11:
				sprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Recipe11"));
				break;
			case 12:
				sprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Recipe12"));
				break;
			}
		}

		if (newOrder)
		{
			const float targetY = 0.85f;
			newOrder->GetTransform()->SetWorldPos({ 3.5f , targetY, 0 });
			newOrder->GetComponent<OrderUI>()->SetTargetPos({ -0.9f + (myOrderUISpacing * (myOrderSprites.size())), targetY, 0.0f });
			newOrder->GetComponent<OrderUI>()->SetOrderIndex(anOrder.myOrderID);

			myOrderSprites.push_back(newOrder);
		}
	}
}

void GameManager::RemoveOrderSprite(const int& anIndex)
{
	int positionToRemove = 99;

	for (int index = 0; index < myOrderSprites.size(); index++)
	{
		int orderIndex = myOrderSprites[index]->GetComponent<OrderUI>()->GetOrderIndex();

		if (orderIndex == anIndex)
		{
			positionToRemove = index;
			myOrderSprites[index]->Destroy();
			myOrderSprites.erase(myOrderSprites.begin() + index);

			break;
		}
	}

	for (int index = positionToRemove; index < myOrderSprites.size(); index++)
	{
		GameObject* sprite = myOrderSprites[index];
		Vector3f spritePos = { 0,0,0 };
		if (sprite)
		{
			spritePos = sprite->GetTransform()->worldPos();
			float Xpos = (-0.9f + (myOrderUISpacing * index));
			sprite->GetComponent<OrderUI>()->SetTargetPos({ Xpos, spritePos.y, spritePos.z });
			sprite->GetComponent<OrderUI>()->StartMovingToStart();
		}
	}
}

int GameManager::ChangeCurrentLevelScore(const int& aChange, bool aFailedOrder)
{
	myLastChange = aChange;
	myShouldShowChange = true;

	if (myCurrentLevelTotalScore == 0 && aFailedOrder)
	{
		myLastChange = -0;
		return -0;
	}

	if ((myCurrentLevelTotalScore + aChange) <= 0)
	{
		int change = -myCurrentLevelTotalScore;
		myLastChange = change;

		myCurrentLevelTotalScore = 0;
		return change;
	}
	else
	{
		myLastChange = aChange;
		myCurrentLevelTotalScore += aChange;
		return aChange;
	}
}

void GameManager::AddFailedOrderScore(const int& aFailedScore)
{
	if(aFailedScore != 0)
	{
		myCurrentLevelFailedOrders += abs(aFailedScore);
	}
}

void GameManager::ResetLevel()
{
	myShouldGenerateOrders = false;
	ResetCurrentLevelScore();
	myCurrentLevelManager.ResetLevel();
	myOrderHandler.Reset();
	myOrderSprites.clear();
	myTutorialFreezeTime = 0.0f;
	StampManager::GetInstance()->Reset();
}

CommonUtilities::XboxControllerInput* GameManager::GetPlayerController(int aPlayer)
{
	switch (aPlayer)
	{
	case 1:
		return myP1Controller;
		break;
	case 2:
		return myP2Controller;
		break;
	case 3:
		return myP3Controller;
		break;
	case 4:
		return myP4Controller;
		break;

	default:
		return myP1Controller;
		break;
	}
}

PlayerControls& GameManager::GetPlayerControls(int aPlayer)
{
	switch (aPlayer)
	{
	case 1:
		return myP1Controls;
		break;
	case 2:
		return myP2Controls;
		break;
	case 3:
		return myP3Controls;
		break;
	case 4:
		return myP4Controls;
		break;

	default:
		return myP1Controls;
		break;
	}
}

void GameManager::CreateTutorialBoard()
{
	int levelNumber = LevelSelectManager::GetInstance()->GetCurrentLevel();

	GameObject* tutorialBoard = InstantiatePrefab("TutorialBoard");

	tutorialBoard->GetComponent<TutorialBoard>()->ActivateMe(levelNumber);
	myTutorialBoard = tutorialBoard;
	Engine::GetInstance()->SetGamePaused(true);
}

void GameManager::ResetCurrentLevelScore()
{
	myCurrentLevelTotalScore = 0;
	myCurrentLevelFailedOrders = 0;
	myCurrentLevelTips = 0;
	myCurrentOrdersDelivered = 0;
}
