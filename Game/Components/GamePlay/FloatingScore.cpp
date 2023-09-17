#include "Game.pch.h"
#include "FloatingScore.h"
#include "Components\UI\UISprite.h"
#include "Shaders\UI\Points\UIPointsPS.h"

void FloatingScore::Update()
{
	myAliveTime += deltaTime;

	auto pointsShader = dynamic_cast<UIPointsPS*>(myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
	if(pointsShader)
	{
		pointsShader->myUIPointsData.Points = myPoints;
		pointsShader->myUIPointsData.Driver = myAliveTime;
		pointsShader->SetData();
	}
}

void FloatingScore::SetPoints(const int& aPoints)
{
	myPoints = aPoints;
	myGameObject->GetTransform()->SetWorldPos({ -0.8f, -0.45f, 0 });
}
