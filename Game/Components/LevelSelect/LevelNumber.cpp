#include "Game.pch.h"
#include "LevelNumber.h"

void LevelNumber::Awake()
{
	LevelSelectManager::GetInstance()->SetCurrentLevel(myLevelNumber);
}

void LevelNumber::RenderInProperties(std::vector<Component*>& aComponentList)
{
	ImGui::InputInt("Level", &myLevelNumber);
}

void LevelNumber::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Int>("LevelNumber", myLevelNumber);
}

void LevelNumber::Load(rapidjson::Value& aComponentData)
{
	if(aComponentData.HasMember("LevelNumber"))
	{
		myLevelNumber = aComponentData["LevelNumber"].GetInt();
	}
}