#include "stdafx.h"
#include "RotateAroundParent.h"

void RotateAroundParent::Awake()
{
	myXRotationRadians = 0.0f;
	myYRotationRadians = 0.0f;
}

void RotateAroundParent::Init()
{
}

void RotateAroundParent::Update()
{
	GameObject* parent = myGameObject->GetTransform()->GetParent()->GetGameObject();

	myXRotationRadians += -myXSpeed * deltaTime;

	myYRotationRadians -= myYSpeed * deltaTime;
	// myYRotationRadians = Catbox::Clamp<float>(myYRotationRadians, -1.4f, 1.4f);


	float xRotation = sinf(myXRotationRadians) * cosf(myYRotationRadians) * myOffset.z;
	float yRotation = sinf(myYRotationRadians) * myOffset.y;
	float zRotation = cosf(myXRotationRadians) * cosf(myYRotationRadians) * myOffset.z;

	myTransform->SetWorldPos(parent->GetTransform()->worldPos() + Vector3f{xRotation, yRotation, zRotation});
	if (myShouldLookAtTarget == true) { myTransform->LookAt(parent->GetTransform()->worldPos()); }
}

void RotateAroundParent::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto& components = ComponentVectorCast<RotateAroundParent>(aComponentList);

	if (Catbox::Checkbox("Should Look At Target", &myShouldLookAtTarget))
	{
		for (auto& component : components)
		{
			component->myShouldLookAtTarget = myShouldLookAtTarget;
		}
	}

	ImGui::Text("Offset to parent");
	if (Catbox::InputFloat("OffsetX", &myOffset.x))
	{
		for (auto& component : components)
		{
			component->myOffset.x = myOffset.x;
		}
	}
	if (Catbox::InputFloat("OffsetY", &myOffset.y))
	{
		for (auto& component : components)
		{
			component->myOffset.y = myOffset.y;
		}
	}
	if (Catbox::InputFloat("OffsetZ", &myOffset.z))
	{
		for (auto& component : components)
		{
			component->myOffset.z = myOffset.z;
		}
	}

	ImGui::Text("Camera Rotation Speed");
	if (Catbox::InputFloat("XSpeed", &myXSpeed))
	{
		for (auto& component : components)
		{
			component->myXSpeed = myXSpeed;
		}
	}
	if (Catbox::InputFloat("YSpeed", &myYSpeed))
	{
		for (auto& component : components)
		{
			component->myYSpeed = myYSpeed;
		}
	}
}

void RotateAroundParent::Save(rapidjson::Value& aComponentData)
{

	int version = 0;
	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveValue<DataType::Bool>("ShouldLookAt", myShouldLookAtTarget);
	wrapper.SaveValue<DataType::Float>("XSpeed", myXSpeed);
	wrapper.SaveValue<DataType::Float>("YSpeed", myYSpeed);

	wrapper.SaveValue<DataType::Float>("OffsetX", myOffset.x);
	wrapper.SaveValue<DataType::Float>("OffsetY", myOffset.y);
	wrapper.SaveValue<DataType::Float>("OffsetZ", myOffset.z);
}

void RotateAroundParent::Load(rapidjson::Value& aComponentData)
{
	int version = aComponentData["Version"].GetInt();

	if (version == 0)
	{
		myShouldLookAtTarget = aComponentData["ShouldLookAt"].GetBool();

		myXSpeed = aComponentData["XSpeed"].GetFloat();
		myYSpeed = aComponentData["YSpeed"].GetFloat();

		myOffset.x = aComponentData["OffsetX"].GetFloat();
		myOffset.y = aComponentData["OffsetY"].GetFloat();
		myOffset.z = aComponentData["OffsetZ"].GetFloat();
	}
	else
	{
		printerror("Wrong json version on Rotate Around Parent component.");
	}
}
