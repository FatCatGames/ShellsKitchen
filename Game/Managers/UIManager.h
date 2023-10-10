#pragma once

class NumberOfPlayersComponent;
class InteractableUIObjectComponent;
class P1MenuComponent;
class P2MenuComponent;
class MenuButton;
class P1CustomizeMenuComponent;
class P2CustomizeMenuComponent;
class PauseMenuComponent;
class SettingsMenuComponent;
class CountdownComponent;

namespace CommonUtilities
{
	class XboxControllerInput;
}

class UIManager
{
public:
	static inline UIManager* GetInstance() { return Instance; }
	UIManager();
	~UIManager() = default;
	void ClearUIManager();
	void LoadLevelSelect();
	void Update();
	void Init();
	inline GameObject* GetPlayer1Object() { return myPlayer1Object; }
	inline GameObject* GetPlayer2Object() { return myPlayer2Object; }

	void HandlePauseMenu();
	void UpdateP1Keyboard();
	void UpdateP2Keyboard();
	void UpdateP1XboxController();
	void UpdateP2XboxController();
	//setters
	void AddCountdownComponent(CountdownComponent* aComponent) { myCountdownComponent = aComponent; }
	void AddPauseMenuComponent(PauseMenuComponent* aComponent) { myPauseMenuComponent = aComponent; }
	void AddSettingsMenuComponent(SettingsMenuComponent* aComponent) { mySettingsMenuComponent = aComponent; }
	void AddNumberOfPlayerComponent(NumberOfPlayersComponent* aComponent) { myNumberOfPlayersComponent = aComponent; };
	void AddInteractableUIObjectComponent(InteractableUIObjectComponent* aComponent);
	void AddP1MenuComponent(P1MenuComponent* aComponent) { myP1MenuComponent = aComponent; }
	void AddP2MenuComponent(P2MenuComponent* aComponent) { myP2MenuComponent = aComponent; }
	void AddP1CustomizeMenuComponent(P1CustomizeMenuComponent* aComponent) { myP1CustomizeMenuComponent = aComponent; }
	void AddP2CustomizeMenuComponent(P2CustomizeMenuComponent* aComponent) { myP2CustomizeMenuComponent = aComponent; }
	void AddMenuButtonToManager(MenuButton* aButton, int aPlayerID);
	void AddCustomizeMenuButtonToManager(MenuButton* aButton, int aPlayerID);
	void AddPauseMenuButtonToManager(MenuButton* aButton);
	void AddSettingsMenuButtonToManager(MenuButton* aButton);
	void SetP1IsInCustomize(bool aValue) { myP1IsInCustomize = aValue; }
	void SetP2IsInCustomize(bool aValue) { myP2IsInCustomize = aValue; }
	void SetP1IsInMainMenu(bool aValue) { myP1IsInMainMenu = aValue; }
	void SetP2IsInMainMenu(bool aValue) { myP2IsInMainMenu = aValue; }
	void SetP1IsInSettings(bool aValue) { myP1IsInSettings = aValue; }
	void SetP2IsInSettings(bool aValue) { myP2IsInSettings = aValue; }
	void SetP1CustomizeIndex(int aIndex) { myP1CustomizeMenuIndex = aIndex; }
	void SetP2CustomizeIndex(int aIndex) { myP2CustomizeMenuIndex = aIndex; }
	void SetIsInPauseMenu(bool aValue) { myIsInPauseMenu = aValue; }
	void SetPauseMenuIndex(int aIndex) { myPauseMenuIndex = aIndex; }
	void SetSettingMenuIndex(int aIndex) { mySettingsMenuIndex = aIndex; }
	void SetP1IsInControls(bool aValue) { myP1IsInControls = aValue; }
	void SetP2IsInControls(bool aValue) { myP2IsInControls = aValue; }
	void SetP1IsInCredits(bool aValue) { myP1IsInCredits = aValue; }
	void SetP2IsInCredits(bool aValue) { myP2IsInCredits = aValue; }


	//Getters
	PauseMenuComponent* GetPauseMenuComponent() { return myPauseMenuComponent; }
	SettingsMenuComponent* GetSettingsMenuComponent() { return mySettingsMenuComponent; }
	NumberOfPlayersComponent* GetNumberOfPlayerComponent() { return myNumberOfPlayersComponent; }
	InteractableUIObjectComponent* GetInteractableUIObjectComponent() { return myInteractableUIObjectComponent; }
	std::vector<MenuButton*> GetP1MenuButtons() { return myP1MenuButtons; }
	std::vector<MenuButton*> GetP2MenuButtons() { return myP2MenuButtons; }
	std::vector<MenuButton*> GetP1CustomizeMenuButtons() { return myP1CustomizeMenuButtons; }
	std::vector<MenuButton*> GetP2CustomizeMenuButtons() { return myP2CustomizeMenuButtons; }
	std::vector<MenuButton*> GetPauseMenuButtons() { return myPauseMenuButtons; }
	std::vector<MenuButton*> GetSettingsMenuButtons() { return mySettingsButtons; }
	P1MenuComponent* GetP1MenuComponent() { return myP1MenuComponent; }
	P2MenuComponent* GetP2MenuComponent() { return myP2MenuComponent; }
	P1CustomizeMenuComponent* GetP1CustomizeMenuComponent() { return myP1CustomizeMenuComponent; }
	P2CustomizeMenuComponent* GetP2CustomizeMenuComponent() { return myP2CustomizeMenuComponent; }
	CountdownComponent* GetCountdownComponent() { return myCountdownComponent; }
	bool GetP1IsInCustomize() { return myP1IsInCustomize; }
	bool GetP2IsInCustomize() { return myP2IsInCustomize; }
	bool GetP1IsInMainMenu() { return myP1IsInMainMenu; }
	bool GetP2IsInMainMenu() { return myP2IsInMainMenu; }
	bool GetP1IsInSettings() { return myP1IsInSettings; }
	bool GetP2IsInSettings() { return myP2IsInSettings; }
	bool GetIsInPauseMenu() { return myIsInPauseMenu; }
	bool GetP1IsInControls() { return myP1IsInControls; }
	bool GetP2IsInControls() { return myP2IsInControls; }
	bool GetP1IsInCredits() { return myP1IsInCredits; }
	bool GetP2IsInCredits() { return myP2IsInCredits; }
	void P1CustomizeInput();
	void P2CustomizeInput();
	void P1SettingsInput();
	void P2SettingsInput();

	//Indexes
	int GetP1MenuIndex() { return myP1MenuIndex; }
	int GetP2MenuIndex() { return myP2MenuIndex; }
	int GetP1CustomizeMenuIndex() { return myP1CustomizeMenuIndex; }
	int GetP2CustomizeMenuIndex() { return myP2CustomizeMenuIndex; }
	int GetPauseMenuIndex() { return myPauseMenuIndex; }
	int GetSettingsMenuIndex() { return mySettingsMenuIndex; }
	//End

	//Resolution
	int GetResolution() { return myResolutionX; }
	void SetResolution(int aValue) { myResolutionX = aValue; }


private:
	//LevelSelectThings
	void LevelSelectShortcut();
	int myLevelToLoad = -1;

	//Menu Indexes
	void ChangeP1MenuIndex(int aChange);
	void ChangeP2MenuIndex(int aChange);
	void ChangeP1CustomizeMenuIndex(int aChange);
	void ChangeP2CustomizeMenuIndex(int aChange);
	void ChangeSettingsIndex(int aChange);

	int myP1MenuIndex = 0;
	int myP2MenuIndex = 0;
	int myP1CustomizeMenuIndex = 0;
	int myP2CustomizeMenuIndex = 0;
	int mySettingsMenuIndex = 0;

	int myPauseMenuIndex = 0;
	bool myIsInPauseMenu = false;

	bool myP1IsInMainMenu = false;
	bool myP2IsInMainMenu = false;
	bool myP1IsInSettings = false;
	bool myP2IsInSettings = false;
	bool myP1IsInCustomize = false;
	bool myP2IsInCustomize = false;

	bool myP1IsInCredits = false;
	bool myP2IsInCredits = false;
	bool myP1IsInControls = false;
	bool myP2IsInControls = false;

	bool myP1HasConnected = false;
	bool myP2HasConnected = false;

	bool mySplashScreenShows = false;
	bool myShowingFullscreenControlls = false;
	//Test for StickMovement
	bool myP1XboxUp = false;
	bool myP1XboxDown = false;
	bool myP1XboxLeft = false;
	bool myP1XboxRight = false;
	bool myP2XboxUp = false;
	bool myP2XboxDown = false;
	bool myP2XboxLeft = false;
	bool myP2XboxRight = false;
	float myP1InputCD = 0.0f;
	float myP1InputDelay = 0.3f;
	float myP2InputCD = 0.0f;
	float myP2InputDelay = 0.3f;
	float treshold = 0.2f;
	void CalculateXboxInputP1();
	void CalculateXboxInputP2();
	void resetP1XboxInput();
	void resetP2XboxInput();

	//Resolution
	int myResolutionX = 1080;

	//Timers
	float myReadyTimer = 4.0f;
	bool myShouldCountdown = false;

	//MenuButtons
	std::vector<MenuButton*> myP1MenuButtons;
	std::vector<MenuButton*> myP2MenuButtons;
	std::vector<MenuButton*> myP1CustomizeMenuButtons;
	std::vector<MenuButton*> myP2CustomizeMenuButtons;
	std::vector<MenuButton*> myPauseMenuButtons;
	std::vector<MenuButton*> mySettingsButtons;


	static UIManager* Instance;
	NumberOfPlayersComponent* myNumberOfPlayersComponent = nullptr;
	InteractableUIObjectComponent* myInteractableUIObjectComponent = nullptr;
	P1MenuComponent* myP1MenuComponent = nullptr;
	P2MenuComponent* myP2MenuComponent = nullptr;
	P1CustomizeMenuComponent* myP1CustomizeMenuComponent = nullptr;
	P2CustomizeMenuComponent* myP2CustomizeMenuComponent = nullptr;
	PauseMenuComponent* myPauseMenuComponent = nullptr;
	SettingsMenuComponent* mySettingsMenuComponent = nullptr;
	CountdownComponent* myCountdownComponent = nullptr;
	CommonUtilities::XboxControllerInput* ControllerP1;
	CommonUtilities::XboxControllerInput* ControllerP2;
	std::shared_ptr<AudioManager> myAudioManager;
	PlayerData* myPlayer1Data;
	PlayerData* myPlayer2Data;

	GameObject* myPlayer1Object;
	GameObject* myPlayer2Object;
};
