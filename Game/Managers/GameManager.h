#pragma once
#include "..\Game\Components\LevelSelect\LevelSelectCamera.h"
#include "..\Game\Components\LevelSelect\LevelSelectPlayer.h"
#include "..\Game\Components\Camera\RayPickerComponent.h"
#include "..\Components\Player\PlayerData.h"
#include "..\OrderHandler.h"
#include "..\Game\Components\MainMenu\CounterComponent.h"
#include "..\Game\Components\MainMenu\LevelCounterComponent.h"
#include "..\Game\Components\GamePlay\TutorialBoard.h"
#include "CurrentLevelManager.h"
#include "..\Game\Components\Audio\AudioPlayer.h"
#include "..\Game\Components\Audio\TutorialBoardAudioPlayer.h"
#include "..\Game\Components\GamePlay\CoinCounterComponent.h"

#define MAX_NUMBERS_OF_PLAYERS 2

struct PlayerControls
{
	KeyCode up;
	KeyCode left;
	KeyCode right;
	KeyCode down;

	KeyCode pickup;
	KeyCode interact;
	KeyCode dash;
	KeyCode emote;

	CommonUtilities::Xbox::Button xboxPickup;
	CommonUtilities::Xbox::Button xboxInteract;
	CommonUtilities::Xbox::Button xboxDash;
	CommonUtilities::Xbox::Button xboxEmote;
};

class Player;
class GameManager
{
public:
	static inline GameManager* GetInstance() { return Instance; }

	// Basic stuff
	GameManager();
	~GameManager();

	void Init();
	void SetControls();
	void Update();

	// Hats?
	void LoadHatsToVector();
	std::vector<string>& GetMyHatsVector();

	// Player stuff
	std::vector<Player*> GetPlayers() { return myPlayers; }
	int GetNumberOfConnectedPlayers() { return myPlayers.size(); }
	int AddPlayer(Player* aPlayer);

	int ConnectControllerToID();
	void RemovePlayer(Player* aPlayer);

	PlayerData* GetPlayer1Data() { return myPlayer1Data; }
	PlayerData* GetPlayer2Data() { return myPlayer2Data; }

	PlayerControls& GetPlayerControls(int aPlayer);
	CommonUtilities::XboxControllerInput* GetPlayerController(int aPlayer);

	// UI
	void AddOrderSprite(Order anOrder, const int& anOrderIndex);
	void RemoveOrderSprite(const int& anIndex);
	void CreateTutorialBoard();

	CoinCounterComponent* GetCounterComponent() { if (myLevelCoinCounter) return myLevelCoinCounter; }
	LevelCounterComponent* GetLevelTimer() { if (myLevelTimer) return myLevelTimer; }

	void SetTutorialBoard(GameObject* aTutorialBoard) { myTutorialBoard = aTutorialBoard; }
	void SetLevelTimer(LevelCounterComponent* aTimerComponent) { myLevelTimer = aTimerComponent; }
	void SetLevelCoinCounter(CoinCounterComponent* aCounterComponent) { myLevelCoinCounter = aCounterComponent; }
	void SetIsShowingTutorial(const bool& aState) { myShowingTutorialBoard = aState; }

	bool GetIsShowingTutorial() { return myShowingTutorialBoard; }

	// Level stuff
	void AddTips(const int& aTip) { myCurrentLevelTips += aTip; }
	void AddDeliveredScore(const int& aScore) { myCurrentOrdersDelivered += aScore; }
	void AddFailedOrderScore(const int& aFailedScore);

	int GetTips() { return myCurrentLevelTips; }
	int GetDeliveredScore() { return myCurrentOrdersDelivered; }
	int GetFailedScore() { return myCurrentLevelFailedOrders; }

	void GenerateOrders(bool aValue) { myShouldGenerateOrders = aValue; }
	void SetLevelComplete(bool aValue) { myLevelCompleted = aValue; }
	void ResetLevel();

	int ChangeCurrentLevelScore(const int& aChange, bool aFailedOrder = false);

	const bool& IsInCountdown() { return myIsInCountdown; }
	void SetIsInCountdown(const bool& aValue) { myIsInCountdown = aValue; }

	// Audio
	void StartAudioPlayer() { if(myAudioPlayer) myAudioPlayer->StartAudio(); }
	void StopLevelMusic() { myAudioPlayer->GetTransform()->GetGameObject()->GetComponent<AudioMonoComponent>()->StopSoundEffect(1); }

	void SetAudioPlayer(AudioPlayer* anAudioPlayer) { myAudioPlayer = anAudioPlayer; }
	void SetTutorialAudioPlayer(TutorialBoardAudioPlayer* anAudioPlayer) { myTutorialBoardAudioPlayer = anAudioPlayer; }

	TutorialBoardAudioPlayer* GetTutorialAudioPlayer() { return myTutorialBoardAudioPlayer; }
	AudioPlayer* GetAudioPlayer() { return myAudioPlayer; }

	// Level select
	LevelSelectCamera* GetLevelSelectCamera() { return myLevelSelectCamera; }
	LevelSelectPlayer* GetLevelSelectPlayer() { return myLevelSelectPlayer; }

	CurrentLevel& GetCurrentLevelManager() { return myCurrentLevelManager; }
	Vector3f GetBoatPos() { return myBoatPos; }
	Vector3f GetBoatStartPos() { return myBoatStartPos; }

	void SetLevelSelectCamera(LevelSelectCamera* aCamera);
	void SetBoatPos(Vector3f aPos) {myBoatPos = aPos;}
	void SetLevelSelectPlayer(LevelSelectPlayer* aPlayer);

	// Camera
	RayPickerComponent* GetRayPicker() { return myRayComp; };
	GameObject* GetCamera() { return myCameraPtr; };

	void SetRayPicker(RayPickerComponent& aRay) { myRayComp = &aRay; myCameraPtr = &myRayComp->GetGameObject(); };

private:
	void ResetCurrentLevelScore();

	static GameManager* Instance;

	LevelCounterComponent* myLevelTimer = nullptr;
	CoinCounterComponent* myLevelCoinCounter = nullptr;
	LevelSelectCamera* myLevelSelectCamera = nullptr;
	LevelSelectPlayer* myLevelSelectPlayer = nullptr;

	TutorialBoardAudioPlayer* myTutorialBoardAudioPlayer = nullptr;
	AudioPlayer* myAudioPlayer = nullptr;

	RayPickerComponent* myRayComp = nullptr;
	GameObject* myCameraPtr = nullptr;
	GameObject* myTutorialBoard = nullptr;

	PlayerData* myPlayer1Data = new PlayerData;
	PlayerData* myPlayer2Data = new PlayerData;

	OrderHandler myOrderHandler;
	CurrentLevel myCurrentLevelManager;

	CommonUtilities::XboxControllerInput* myP1Controller = nullptr;
	CommonUtilities::XboxControllerInput* myP2Controller = nullptr;
	CommonUtilities::XboxControllerInput* myP3Controller = nullptr;
	CommonUtilities::XboxControllerInput* myP4Controller = nullptr;
	PlayerControls myP1Controls;
	PlayerControls myP2Controls;
	PlayerControls myP3Controls;
	PlayerControls myP4Controls;

	Vector3f myBoatPos = { 0.0f,0.0f,0.0f };
	Vector3f myBoatStartPos = { -58.0f,1.645f,-40.0f };

	std::vector<Player*> myPlayers;
	std::vector<GameObject*> myOrderSprites;
	std::vector<string> myHats;
	std::vector<bool> myConnectedPlayers;
	std::array<std::string, 3> myCutScenesLevels = { "Third_Level_lvl", "Sixth_Level_lvl", "Ninth_Level_lvl"};
	std::array<int, 10> myHighScores;

	UINT myTimerId = 0;

	int myNumberOfConnectedPlayers = 0;
	int myCurrentOrdersDelivered = 0;
	int myCurrentLevelTips = 0;
	int myCurrentLevelFailedOrders = 0;
	int myCurrentLevelTotalScore = 0;
	int myLastChange = 0;

	const float myOrderUISpacing = 0.19f;
	float myChangeTimer = 0.0f;
	float myScoreBoardTimer = 0.0f;
	float myTutorialFreezeTime = 0.0f;

	bool myCanCloseTutorial = false;
	bool myShowingTutorialBoard = false;
	bool myLevelCompleted = false;
	bool myShouldGenerateOrders = false;
	bool myHasLoadedHatsVector = false;
	bool myShouldShowChange = false;
	bool myShouldRemoveTutorialAudioPlayer = false;

	bool myIsInCountdown = false;
};