#include "Game.pch.h"
#include "CoinCounterComponent.h"
#include "Shaders\UI\Counter\UICounterPS.h"
#define MAX_VALUE_TO_SHOW 9999

void CoinCounterComponent::Awake()
{
	auto counterShader = dynamic_cast<UICounterPS*>(myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
	if(counterShader)
	{
		counterShader->myUICounterData.Driver = 1;
		counterShader->myUICounterData.Points = 0;
		counterShader->myUICounterData.TotalPoints = 0;
		counterShader->SetData();
	}
}

void CoinCounterComponent::AddValue(const int& aValue)
{
	SetValue(myValueToShow + aValue);
}

void CoinCounterComponent::Update()
{

}

void CoinCounterComponent::SetValue(const int& aValue)
{
	myValueToShow = aValue;
	myUnits = myValueToShow % 10;
	myTens = (myValueToShow / 10) % 10;
	myHundreds = (myValueToShow / 100) % 10;
	myThousands = (myValueToShow / 1000) % 10;

	//const std::string unitsString = to_string(units);
	//const std::string tensString = to_string(tens);
	//const std::string hundredsString = to_string(hundreds);
	//const std::string thousandsString = to_string(thousands);
	
	//if(thousandsString == "0")
	//{
	//	myThousandSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Empty"));
	//}
	//else
	//{
	//	myThousandSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>(thousandsString));
	//}

	//if(thousandsString == "0" && hundredsString == "0")
	//{
	//	myHundredsSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Empty"));
	//}
	//else
	//{
	//	myHundredsSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>(hundredsString));
	//}

	//if(thousandsString == "0" && hundredsString == "0" && tensString == "0")
	//{
	//	myTensSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Empty"));
	//}
	//else
	//{
	//	myTensSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>(tensString));
	//}

	//myUnitsSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>(unitsString));
}
