#include "Game.pch.h"
#include "LevelSelectLevel.h"
#include "ComponentTools\SceneManager.h"
#include "ComponentTools\UIEventHandler.h"
#include "..\Catbox\Audio\AudioManager.h"
#include "..\Catbox\Components\3D\ModelInstance.h"
#include "..\Catbox\Assets\Material.h"

void LevelSelectLevel::Awake()
{
	if(myGameObject->GetTransform()->GetChildren().size() != 0)
	{
		myGameObject->GetTransform()->GetChildren()[0]->GetGameObject()->SetActive(false);
	}

	SetFlagStarMaterial();
}

void LevelSelectLevel::Update()
{
	if(myCanStartLevel == true)
	{
		if((Input::GetKeyPress(KeyCode::SPACE) || Input::GetController().IsPressed(CommonUtilities::Xbox::A)) && UIManager::GetInstance()->GetIsInPauseMenu() == false)
		{
			UIManager::GetInstance()->ClearUIManager();
			UIEventHandler::SetNextLevel(myLevelName);
			GameManager::GetInstance()->ResetLevel();
			LevelSelectManager::GetInstance()->SetCurrentLevel(myLevelNumber);

			GameManager::GetInstance()->SetIsShowingTutorial(true);
			GameManager::GetInstance()->GenerateOrders(true);
			GameManager::GetInstance()->SetBoatPos(GameManager::GetInstance()->GetLevelSelectPlayer()->GetTransform()->worldPos());
		}
	}
}

void LevelSelectLevel::OnTriggerEnter(Collider* aCollider)
{
	if(myLevelName != "NoLevel")
	{
		myCanStartLevel = true;
		if(myGameObject->GetTransform()->GetChildren().size() > 0)
		{
			myGameObject->GetTransform()->GetChildren()[0]->GetGameObject()->SetActive(true);
			Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_LEVEL_BOX_POP_UP));
		}
	}
}
void LevelSelectLevel::OnTriggerExit(Collider* aCollider)
{
	if(myLevelName != "NoLevel")
	{
		myCanStartLevel = false;
		if(myGameObject->GetTransform()->GetChildren().size() > 0)
		{
			myGameObject->GetTransform()->GetChildren()[0]->GetGameObject()->SetActive(false);
		}
	}
}

void LevelSelectLevel::RenderInProperties(std::vector<Component*>& aComponentList)
{
	ImGui::InputInt("Level", &myLevelNumber);
	ImGui::InputText("Name", &myLevelName);
}

void LevelSelectLevel::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Int>("LevelNumber", myLevelNumber);
	wrapper.SaveString("LevelName", (myLevelName).c_str());
}

void LevelSelectLevel::Load(rapidjson::Value& aComponentData)
{
	if(aComponentData.HasMember("LevelNumber"))
	{
		myLevelNumber = aComponentData["LevelNumber"].GetInt();
	}
	if(aComponentData.HasMember("LevelName"))
	{
		myLevelName = aComponentData["LevelName"].GetString();
	}
}

void LevelSelectLevel::SetFlagStarMaterial()
{
	bool reachedBronze = LevelSelectManager::GetInstance()->GetHasBronze(myLevelNumber-1);
	bool reachedSilver = LevelSelectManager::GetInstance()->GetHasSilver(myLevelNumber-1);
	bool reachedGold = LevelSelectManager::GetInstance()->GetHasGold(myLevelNumber-1);

	if(reachedGold)
	{
		myGameObject->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>("Flag_3_star_mat"), 0);
	}
	else if(reachedSilver)
	{
		myGameObject->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>("Flag_2_star_mat"), 0);
	}
	else if(reachedBronze)
	{
		myGameObject->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>("Flag_1_star_mat"), 0);
	}
	else
	{
		myGameObject->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>("Flag_mat"), 0);
	}


}