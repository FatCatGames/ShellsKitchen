#pragma once
#include "Components/UI/UISprite.h"
#include "Components/UI/UIElement.h"

class texture;
class MenuButton : public UIElement
{
public:
	enum class MenuButtonID
	{
		Customize,
		Settings,
		Credits,
		Quit_Game,
		Resume,
		Ready,
		ChangeMusicVol,
		ChangeSFXVol,
		ChangeResolution,
		CloseSettings,
		ReturnToMainMenu,
		Hat,
		closeCustomize,
		controls,
		ResetLevel,
		Count
	};
	enum class PlayerID
	{
		Player1,
		Player2,
		Count
	};

	MenuButton();
	~MenuButton();
	void Awake() override;
	void Render() override;
	void OnClick() override;
	void OnMouseEnter() override;
	void OnMouseExit() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	MenuButtonID& GetButtonID() { return myAbilityID; }
	PlayerID& GetPlayerID() { return myPlayerID; }

	void ButtonAction(MenuButtonID aID);

	void ActivateChild(int aIndex);
	void CloseAllChildren();

private:
	//Members
	MenuButtonID myAbilityID = MenuButtonID::Customize;
	PlayerID myPlayerID = PlayerID::Player1;

private:
	//Functions
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
};
