#include "Game.pch.h"
#include "IngredientIcon.h"
#include "Components\Physics\RigidBody.h"

void IngredientIcon::Awake()
{
	myCamera = GameManager::GetInstance()->GetRayPicker()->GetGameObject().GetComponent<Camera>();
	if(myGameObject->GetTransform()->GetChildren().size() > 0)
	{
		myFirstChild = myGameObject->GetTransform()->GetChildren()[0];
		if (myDeactivateInAwake)
		{
			myFirstChild->GetGameObject()->SetActive(false);
		}
	}

	myParentPos = myGameObject->GetTransform()->worldPos();
	myRayPickerComponent = GameManager::GetInstance()->GetRayPicker();
}

void IngredientIcon::Update()
{
	if(myGameObject->GetTransform()->GetChildren().size() > 0)
	{
		if(auto sprite = myFirstChild->GetGameObject()->GetComponent<UISprite>())
		{
			Catbox::Matrix4x4<float> projection = myCamera->GetProjectionMatrix();
			Catbox::Matrix4x4<float> viewMatrix = Catbox::Matrix4x4<float>::GetFastInverse(myRayPickerComponent->GetTransform()->GetWorldTransformMatrix());

			Vector3f parentPos = myGameObject->GetTransform()->worldPos();

			Catbox::Vector4<float> parentPosWorld = { parentPos.x, parentPos.y, parentPos.z , 1 };
			parentPosWorld = parentPosWorld * viewMatrix;
			parentPosWorld = parentPosWorld * projection;
			parentPosWorld = parentPosWorld / parentPosWorld.w;

			parentPos = { parentPosWorld.x, parentPosWorld.y, parentPosWorld.z };
			parentPos += myOffset;
			sprite->UnlockTransform();
			myFirstChild->SetWorldPos(parentPos);
			sprite->LockTransform();
		}
	}
}

void IngredientIcon::SetOffset(Vector3f anOffset)
{
	myOffset = anOffset;
}

void IngredientIcon::RenderInProperties(std::vector<Component*>& aComponentList)
{
	ImGui::Checkbox("myDeactivateInAwake", &myDeactivateInAwake);
}

void IngredientIcon::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();

	wrapper.SaveValue<DataType::Bool>("myDeactivateInAwake", myDeactivateInAwake);
}

void IngredientIcon::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("myDeactivateInAwake"))
	{
		myDeactivateInAwake = aComponentData["myDeactivateInAwake"].GetBool();
	}
}
