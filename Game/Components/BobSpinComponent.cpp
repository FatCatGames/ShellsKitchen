#include "Game.pch.h"
#include "BobSpinComponent.h"

BobSpinComponent::BobSpinComponent()
{
}

BobSpinComponent::~BobSpinComponent()
{
}

void BobSpinComponent::Awake()
{
	myShouldCull = true;
	myOriginalPosition = myGameObject->GetTransform()->worldPos();
	//myTransform->SetLocalPos(myTransform->localPos().x, myOriginalYPos, myTransform->localPos().z);
}

void BobSpinComponent::Update()
{
	if (myShouldBob)
	{
		myTransform->SetWorldPos(myOriginalPosition.x + (sin(myTotalTime * myBobSpeeds[0]) * myBobHeights[0]), myOriginalPosition.y + (sin(myTotalTime * myBobSpeeds[1]) * myBobHeights[1]), myOriginalPosition.z + (sin(myTotalTime * myBobSpeeds[2]) * myBobHeights[2]));
		myTotalTime += deltaTime;
	}

	if (myShouldSpin)
	{
		myTransform->Rotate(mySpinSpeeds[0] * deltaTime, mySpinSpeeds[1] * deltaTime, mySpinSpeeds[2] * deltaTime);
	}
}

void BobSpinComponent::RenderInProperties(std::vector<Component*>& /*aComponentList*/)
{
	//ImGui::Checkbox("Affected by Character Controllers", &myIsAffectedByCharacterControllers);
	ImGui::Checkbox("Bob", &myShouldBob);
	ImGui::DragFloat3("Bob Speed", myBobSpeeds);
	ImGui::DragFloat3("Bob Height", myBobHeights);
	ImGui::Checkbox("Spin", &myShouldSpin);
	ImGui::DragFloat3("Spin Speed", mySpinSpeeds);
}

void BobSpinComponent::RunInEditor()
{
	/*
	if (myShouldBob)
	{
		myTransform->SetWorldPos(myTransform->worldPos().x, myOriginalYPos + (sin(myTotalTime * myBobSpeeds[1]) * myBobHeights[1]), myTransform->localPos().z);
		myTotalTime += deltaTime;
	}

	if (myShouldSpin)
	{
		myTransform->Rotate(mySpinSpeeds[0] * deltaTime, mySpinSpeeds[1] * deltaTime, mySpinSpeeds[2] * deltaTime);
	}
	*/
}

void BobSpinComponent::SetStartPosition(const Vector3f& position)
{
	myOriginalPosition = position;
}

void BobSpinComponent::SetShouldBob(bool aShouldBob)
{
	myOriginalPosition = myGameObject->GetTransform()->worldPos();
	myShouldBob = aShouldBob;
}

void BobSpinComponent::Save(rapidjson::Value& /*aComponentData*/)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();

	wrapper.SaveValue<DataType::Bool>("Should Bob", myShouldBob);
	wrapper.SaveValue<DataType::Bool>("Should Spin", myShouldSpin);

	auto bobSpeedArray = wrapper.CreateArray();
	for (size_t i = 0; i < 3; i++)
	{
		rapidjson::Value typeID;
		typeID.SetFloat(myBobSpeeds[i]);
		bobSpeedArray.PushBack(typeID, alloc);
	}
	wrapper.SaveObject("Bob Speeds", bobSpeedArray);

	auto bobHeightArray = wrapper.CreateArray();
	for (size_t i = 0; i < 3; i++)
	{
		rapidjson::Value typeID;
		typeID.SetFloat(myBobHeights[i]);
		bobHeightArray.PushBack(typeID, alloc);
	}
	wrapper.SaveObject("Bob Heights", bobHeightArray);

	auto spinSpeedArray = wrapper.CreateArray();
	for (size_t i = 0; i < 3; i++)
	{
		rapidjson::Value typeID;
		typeID.SetFloat(mySpinSpeeds[i]);
		spinSpeedArray.PushBack(typeID, alloc);
	}
	wrapper.SaveObject("Spin Speeds", spinSpeedArray);
}

void BobSpinComponent::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Should Bob"))
	{
		myShouldBob = aComponentData["Should Bob"].GetBool();
	}
	if (aComponentData.HasMember("Should Spin"))
	{
		myShouldSpin = aComponentData["Should Spin"].GetBool();
	}
	if (aComponentData.HasMember("Bob Speeds"))
	{
		for (size_t i = 0; i < 3; i++)
		{
			myBobSpeeds[i] = aComponentData["Bob Speeds"][i].GetFloat();
		}
	}
	if (aComponentData.HasMember("Bob Heights"))
	{
		for (size_t i = 0; i < 3; i++)
		{
			myBobHeights[i] = aComponentData["Bob Heights"][i].GetFloat();
		}
	}
	if (aComponentData.HasMember("Spin Speeds"))
	{
		for (size_t i = 0; i < 3; i++)
		{
			mySpinSpeeds[i] = aComponentData["Spin Speeds"][i].GetFloat();
		}
	}
}
