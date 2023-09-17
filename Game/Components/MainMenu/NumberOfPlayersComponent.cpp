#include "Game.pch.h"
#include "NumberOfPlayersComponent.h"

NumberOfPlayersComponent::NumberOfPlayersComponent()
{
}

NumberOfPlayersComponent::~NumberOfPlayersComponent()
{
}

void NumberOfPlayersComponent::Awake()
{
	UIManager::GetInstance()->AddNumberOfPlayerComponent(this);
}

void NumberOfPlayersComponent::Update()
{

}

void NumberOfPlayersComponent::RenderInProperties(std::vector<Component*>& aComponentList)
{
	//ImGui::Checkbox("Affected by Character Controllers", &myIsAffectedByCharacterControllers);
	//ImGui::Checkbox("Bob", &myShouldBob);
	//ImGui::DragFloat3("Bob Speed", myBobSpeeds);
	//ImGui::DragFloat3("Bob Height", myBobHeights);
	//ImGui::Checkbox("Spin", &myShouldSpin);
	//ImGui::DragFloat3("Spin Speed", mySpinSpeeds);
}

void NumberOfPlayersComponent::RunInEditor()
{
}

void NumberOfPlayersComponent::UpdateSprite()
{
	switch (myNumberOfConnectedPlayers)
	{
	case 0:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("ConnectedPlayers0"));
		break;
	case 1:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("ConnectedPlayers1"));
		break;
	case 2:
		myGameObject->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("ConnectedPlayers2"));
		break;
	default:
		break;
	}
}


void NumberOfPlayersComponent::Save(rapidjson::Value& aComponentData)
{

}

void NumberOfPlayersComponent::Load(rapidjson::Value& aComponentData)
{

}
