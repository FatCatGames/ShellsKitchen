#include "Game.pch.h"
#include "LevelTimer.h"
#include "Components\UI\UISprite.h"
#include "Managers\GameManager.h"
#include "Components\MainMenu\LevelCounterComponent.h"

void LevelTimer::Awake()
{
	GameManager::GetInstance()->SetLevelTimer(myGameObject->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<LevelCounterComponent>());

	myTransform->SetWorldScale({ 400,400,400 });
	myTransform->SetWorldPos({ 0.77f, -0.8f, 0 });
}