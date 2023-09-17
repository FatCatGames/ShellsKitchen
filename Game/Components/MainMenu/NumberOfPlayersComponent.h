#pragma once
#include "Components/UI/UISprite.h"

class NumberOfPlayersComponent : public Component
{
public:
	NumberOfPlayersComponent();
	~NumberOfPlayersComponent();
	void Awake() override;
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList);
	void RunInEditor() override;
	void UpdateSprite();
	void AddPlayerToGame() { myNumberOfConnectedPlayers++; UpdateSprite(); };
	void RemovePlayerFromGame() { myNumberOfConnectedPlayers--; UpdateSprite(); };
	int GetNumberOfPlayers() { return myNumberOfConnectedPlayers; }

private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	int myNumberOfConnectedPlayers = 0;
};
