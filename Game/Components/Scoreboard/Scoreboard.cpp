#include "Game.pch.h"
#include "Scoreboard.h"
#include "Components\UI\UISprite.h"
#include "Managers\GameManager.h"
#include "..\Catbox\Audio\AudioManager.h"

void Scoreboard::Awake()
{
	myTransform->SetWorldPos({ 0,0,0 });
}

void Scoreboard::SetValues(int score, int tips, int failed, int bronze, int silver, int gold)
{
	myOrdersDeliveredScore = score;
	myTips = tips;
	myOrdersFailedScore = failed;
	myTotalScore = myOrdersDeliveredScore + myTips - myOrdersFailedScore;

	myBronzeScore = bronze;
	mySilverScore = silver;
	myGoldScore = gold;

	myGameObject->GetTransform()->GetChildren()[1]->GetGameObject()->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<CounterComponent>()->SetValue(myOrdersDeliveredScore);
	myGameObject->GetTransform()->GetChildren()[2]->GetGameObject()->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<CounterComponent>()->SetValue(myTips);
	myGameObject->GetTransform()->GetChildren()[3]->GetGameObject()->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<CounterComponent>()->SetValue(myOrdersFailedScore);
	myGameObject->GetTransform()->GetChildren()[4]->GetGameObject()->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<CounterComponent>()->SetValue(myTotalScore);
	myGameObject->GetTransform()->GetChildren()[5]->GetGameObject()->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<CounterComponent>()->SetValue(myBronzeScore);
	myGameObject->GetTransform()->GetChildren()[6]->GetGameObject()->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<CounterComponent>()->SetValue(mySilverScore);
	myGameObject->GetTransform()->GetChildren()[7]->GetGameObject()->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<CounterComponent>()->SetValue(myGoldScore);

	if(myTotalScore >= gold)
	{
		myGameObject->GetTransform()->GetChildren()[8]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Score3"));
	}
	else if(myTotalScore >= silver)
	{
		myGameObject->GetTransform()->GetChildren()[8]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Score2"));
	}
	else if(myTotalScore >= bronze)
	{
		myGameObject->GetTransform()->GetChildren()[8]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Score1"));
	}
	else
	{
		myGameObject->GetTransform()->GetChildren()[8]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Score0"));
	}
}