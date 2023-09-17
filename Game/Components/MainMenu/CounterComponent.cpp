#include "Game.pch.h"
#include "CounterComponent.h"
#define MAX_VALUE_TO_SHOW 9999

void CounterComponent::Awake()
{
	myThousandSprite = myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>();
	myHundredsSprite = myTransform->GetChildren()[1]->GetGameObject()->GetComponent<UISprite>();
	myTensSprite = myTransform->GetChildren()[2]->GetGameObject()->GetComponent<UISprite>();
	myUnitsSprite = myTransform->GetChildren()[3]->GetGameObject()->GetComponent<UISprite>();

	myThousandSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Empty"));
	myHundredsSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Empty"));
	myTensSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Empty"));
}

void CounterComponent::AddValue(const int& aValue)
{
	SetValue(myValueToShow + aValue);
}

void CounterComponent::SetValue(const int& aValue)
{
	myValueToShow = aValue;
	units = myValueToShow % 10;
	tens = (myValueToShow / 10) % 10;
	hundreds = (myValueToShow / 100) % 10;
	thousands = (myValueToShow / 1000) % 10;

	const std::string unitsString = to_string(units);
	const std::string tensString = to_string(tens);
	const std::string hundredsString = to_string(hundreds);
	const std::string thousandsString = to_string(thousands);

	if(thousandsString == "0")
	{
		myThousandSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Empty"));
	}
	else
	{
		myThousandSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>(thousandsString));
	}

	if(thousandsString == "0" && hundredsString == "0")
	{
		myHundredsSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Empty"));
	}
	else
	{
		myHundredsSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>(hundredsString));
	}

	if(thousandsString == "0" && hundredsString == "0" && tensString == "0")
	{
		myTensSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Empty"));
	}
	else
	{
		myTensSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>(tensString));
	}
	
	myUnitsSprite->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>(unitsString));
}
