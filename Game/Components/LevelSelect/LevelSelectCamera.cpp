#include "Game.pch.h"
#include "LevelSelectCamera.h"
#include "Managers\GameManager.h"

void LevelSelectCamera::Awake()
{
	GameManager::GetInstance()->SetLevelSelectCamera(this);

}

void LevelSelectCamera::Update()
{
	Vector3f playerPosition = { 0,0,0 };

	if(GameManager::GetInstance()->GetLevelSelectPlayer())
	{
		playerPosition = GameManager::GetInstance()->GetLevelSelectPlayer()->GetTransform()->worldPos();
	}
	Vector3f newPosition = playerPosition + myOffset;
	if (myTransform->worldPos().Distance(newPosition) < 10.0f) 
	{
		newPosition = Catbox::Lerp<Vector3f>(myTransform->worldPos(), newPosition, 3.0f * deltaTime);
	}

	myTransform->SetWorldPos(newPosition);
	myTransform->SetWorldRot(myRotation);
}