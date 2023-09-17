#include "Game.pch.h"
#include "Sharkfin.h"

void Sharkfin::Awake()
{
	if (GameManager::GetInstance()->GetLevelSelectPlayer())
		myLevelSelectPlayer = &GameManager::GetInstance()->GetLevelSelectPlayer()->GetGameObject();

	if (myTransform)
	{
		if (myTransform->GetChildren()[0]->GetGameObject()->GetName() == "Ripples")
		{
			myRipples = myTransform->GetChildren()[0];
		}
		else if (myTransform->GetChildren()[1]->GetGameObject()->GetName() == "Ripples")
		{
			myRipples = myTransform->GetChildren()[1];
		}
	}

}

void Sharkfin::Update()
{
	if (!myLevelSelectPlayer) return;
		
	Vector3f sharkPos = myTransform->worldPos() - (20.f * myTransform->right());
	float currentHeight = sharkPos.y;
	sharkPos.y = 0;
	Vector3f playerPos = myLevelSelectPlayer->GetTransform()->worldPos();
	playerPos.y = 0;
	float distanceSqrd = (playerPos - sharkPos).LengthSqr();

	if (distanceSqrd < (myDiveDistance * myDiveDistance)) //diving
	{
		currentHeight -= myDiveSpeed * deltaTime;
	}
	else //going up
	{
		currentHeight += myDiveSpeed * deltaTime;
	}
	currentHeight = Catbox::Clamp(currentHeight, myStartY - myDiveDepth, myStartY);
	Vector3f pos = myTransform->worldPos();
	pos.y = currentHeight;

	myTransform->SetWorldPos(pos);
	if (abs(currentHeight) > myDiveDepth - 1.0f && abs(currentHeight) < myDiveDepth - 0.5f && mySplashTimer > 0.75f)
	{
		if (myRipples)
		{
			auto meme = InstantiatePrefab("WaterSplashVFX_axol");
			meme->GetTransform()->SetWorldPos(myRipples->worldPos().x, 0.2f, myRipples->worldPos().z - 1.f);
		}
		mySplashTimer = 0.f;
	}
	mySplashTimer += deltaTime;

	//test code
	//print("Shark pos should be " + std::to_string(sharkpos.x) + ", " + std::to_string(sharkpos.z));
}

void Sharkfin::RenderInProperties(std::vector<Component*>& aComponentList)
{
	ImGui::InputFloat("DiveSpeed##Sharkfin", &myDiveSpeed);
	ImGui::InputFloat("DiveDistance##Sharkfin", &myDiveDistance);
	ImGui::InputFloat("DiveDepth##Sharkfin", &myDiveDepth);
}

void Sharkfin::Save(rapidjson::Value& aComponentData)
{
	int version = 1;
	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Int>("Version", version);

	wrapper.SaveValue<DataType::Float>("DiveSpeed", myDiveSpeed);
	wrapper.SaveValue<DataType::Float>("DiveDistance", myDiveDistance);
	wrapper.SaveValue<DataType::Float>("DiveDepth", myDiveDepth);
}

void Sharkfin::Load(rapidjson::Value& aComponentData)
{
	int version = aComponentData["Version"].GetInt();

	if (version >= 1)
	{
		myDiveSpeed = aComponentData["DiveSpeed"].GetFloat();
		myDiveDistance = aComponentData["DiveDistance"].GetFloat();
		myDiveDepth = aComponentData["DiveDepth"].GetFloat();
	}
}
