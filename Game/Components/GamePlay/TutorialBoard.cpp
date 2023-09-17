#include "Game.pch.h"
#include "TutorialBoard.h"
#include "Components\UI\UISprite.h"
#include "Managers\GameManager.h"

void TutorialBoard::Awake()
{
	myTransform->SetWorldPos({ 0, -0.9f ,0 });
}

void TutorialBoard::ActivateMe(const int& anIndex)
{
	myTutorialIndex = anIndex;

	switch(myTutorialIndex)
	{
		case 1:
			myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Tutorial01"));
			break;
		case 2:
			myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Tutorial02"));

			break;
		case 3:
			myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Tutorial03"));

			break;
		case 4:
			myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Tutorial04"));

			break;
		case 5:
			myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Tutorial05"));

			break;
		case 6:
			myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Tutorial06"));

			break;
		case 7:
			myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Tutorial07"));

			break;
		case 8:
			myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Tutorial08"));

			break;
		case 9:
			myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Tutorial09"));

			break;
		case 10:
			myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Tutorial10"));

			break;
		default:
			myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("Empty"));

			break;
	}
}