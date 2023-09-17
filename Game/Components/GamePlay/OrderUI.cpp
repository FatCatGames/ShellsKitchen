#include "Game.pch.h"
#include "OrderUI.h"
#include "Components\UI\UISprite.h"
#include "Shaders\UI\ProgressBar\UIProgressBarPS.h"
#include "Shaders\UI\Order\UIOrderPS.h"

void OrderUI::Update()
{
	if(myHasSetup == false)
	{
		myHasSetup = true;

		myOrderDuration = OrderTime;
		myOrderTime = myOrderDuration;
	}

	myOrderTime -= deltaTime;

	const float timeLeft = myOrderTime / myOrderDuration;

	auto timeLeftShader = dynamic_cast<UIProgressBarPS*>(myTransform->GetChildren()[1]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
	auto OrderBackground = dynamic_cast<UIOrderPS*>(myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
	if(timeLeftShader)
	{
		timeLeftShader->myUIProgressData.Resource = timeLeft;
		timeLeftShader->SetData();
	}
	if (OrderBackground)
	{
		OrderBackground->myUIOrderData.Resource = timeLeft;
		OrderBackground->SetData();
	}

	if(myIsLerping)
	{
		SpawnScaleLerp();
	}
	if(myIsMovingToStartPos == true)
	{
		MovingToStartLocation();
	}
	testtimer -= deltaTime;

}

void OrderUI::MovingToStartLocation()
{
	const float speed = 8.0f;
	const float xPosChange = deltaTime * speed;
	const Vector3f pos = myTransform->worldPos();

	if(pos.x - myStartPos.x < 0)
	{
		float test = pos.x - myStartPos.x;
		print(to_string(test));
		myIsMovingToStartPos = false;
		myIsLerping = true;
		myIsLerpingDown = true;
		myTransform->SetWorldPos(myStartPos);
	}
	else
	{
		myTransform->SetWorldPos({ pos.x - xPosChange, pos.y, pos.z });
	}
}

void OrderUI::StartMovingToStart()
{
	myIsMovingToStartPos = true;
}

void OrderUI::SetTargetPos(const Vector3f& aStartLocation)
{
	myStartPos = aStartLocation;
}

void OrderUI::SetOrderIndex(const int& aIndex)
{
	myOrderIndex = aIndex;
}

void OrderUI::SpawnScaleLerp()
{
	if(myIsLerpingDown == true)
	{
		const float timeLimit = 0.12f;

		myLerpingTime += deltaTime;

		const float percentage = Catbox::Lerp(1.0f, 0.9f, myLerpingTime / timeLimit);
		const float posPercentage = Catbox::Lerp(0.0f, 1.0f, myLerpingTime / timeLimit);
		const Vector3f pos = myTransform->worldPos();
		const Vector3f barPos = myTransform->GetChildren()[1]->GetGameObject()->GetTransform()->worldPos();

		UISprite* sprite = myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>();
		sprite->GetTransform()->SetWorldScale({ percentage * 200.0f, 200.0f, 1.0f});
		sprite->GetTransform()->SetWorldPos({ pos.x - posPercentage * 0.01f, pos.y, pos.z});

		UISprite* barSprite = myTransform->GetChildren()[1]->GetGameObject()->GetComponent<UISprite>();
		barSprite->GetTransform()->SetWorldScale({ percentage * 164.5f, 30.0f, 1.0f });
		barSprite->GetTransform()->SetWorldPos({ pos.x - posPercentage * 0.01f, barPos.y, barPos.z });

		if(myLerpingTime > timeLimit)
		{
			myIsLerpingDown = false;
			myIsLerpingUp = true;
			myLerpingTime = 0.0f;
		}
	}
	if(myIsLerpingUp == true)
	{
		const float timeLimit = 0.12f;

		myLerpingTime += deltaTime;

		const float percentage = Catbox::Lerp(0.9f, 1.0f, myLerpingTime / timeLimit);
		const float posPercentage = Catbox::Lerp(1.0f, 0.0f, myLerpingTime / timeLimit);
		const Vector3f pos = myTransform->worldPos();
		const Vector3f barPos = myTransform->GetChildren()[1]->GetGameObject()->GetTransform()->worldPos();

		UISprite* sprite = myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>();
		sprite->GetTransform()->SetWorldScale({ percentage * 200.0f, 200.0f, 1.0f });
		sprite->GetTransform()->SetWorldPos({ pos.x - posPercentage * 0.01f, pos.y, pos.z });

		UISprite* barSprite = myTransform->GetChildren()[1]->GetGameObject()->GetComponent<UISprite>();
		barSprite->GetTransform()->SetWorldScale({ percentage * 164.5f, 30.0f, 1.0f });
		barSprite->GetTransform()->SetWorldPos({ pos.x - posPercentage * 0.01f, barPos.y, barPos.z });

		if(myLerpingTime > timeLimit)
		{
			myIsLerpingUp = false;
			myIsLerping = false;
			myLerpingTime = 0.0f;
		}
	}
}
