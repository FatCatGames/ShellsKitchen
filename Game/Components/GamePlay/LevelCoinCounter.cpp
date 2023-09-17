#include "Game.pch.h"
#include "LevelCoinCounter.h"
#include "Components\UI\UISprite.h"
#include "Managers\GameManager.h"

void LevelCoinCounter::Awake()
{
	GameManager::GetInstance()->SetLevelCoinCounter(myGameObject->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<CoinCounterComponent>());

	myTransform->SetWorldScale({ 400,400,400 });
	myTransform->SetWorldPos({ -0.78f, -0.8f, 0 });
}

void LevelCoinCounter::Update()
{
	UISprite* sprite = myGameObject->GetComponent<UISprite>();
	sprite->SetColor({ 1,1,1,1 });
}

void LevelCoinCounter::RenderInProperties(std::vector<Component*>& aComponentList)
{
	Catbox::InputInt("Bronze", &myBronzeScore);
	Catbox::InputInt("Silver", &mySilverScore);
	Catbox::InputInt("Gold", &myGoldScore);
}

void LevelCoinCounter::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Int>("Bronze", myBronzeScore);
	wrapper.SaveValue<DataType::Int>("Silver", mySilverScore);
	wrapper.SaveValue<DataType::Int>("Gold", myGoldScore);
}

void LevelCoinCounter::Load(rapidjson::Value& aComponentData)
{
	if(aComponentData.HasMember("Bronze"))
	{
		myBronzeScore = aComponentData["Bronze"].GetFloat();
	}
	if(aComponentData.HasMember("Silver"))
	{
		mySilverScore = aComponentData["Silver"].GetFloat();
	}
	if(aComponentData.HasMember("Gold"))
	{
		myGoldScore = aComponentData["Gold"].GetFloat();
	}
}