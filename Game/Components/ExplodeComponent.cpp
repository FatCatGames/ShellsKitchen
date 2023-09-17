#include "Game.pch.h"
#include "ExplodeComponent.h"
#include "Components/Physics/RigidBody.h"

ExplodeComponent::ExplodeComponent()
{
	//myExplosionOrigin = myTransform->worldPos();
}

void ExplodeComponent::Update()
{
	if (!myHasExploded)
	{
		for (size_t i = 0; i < myTransform->GetChildren().size(); i++)
		{
			if (myTransform->GetChildren()[i]->GetGameObject()->GetComponent<RigidBody>())
			{
				myTransform->GetChildren()[i]->GetGameObject()->GetComponent<RigidBody>()->SetActorPosition(myTransform->worldPos());
				myTransform->GetChildren()[i]->GetGameObject()->GetComponent<RigidBody>()->SetActorRotation(myTransform->worldRot());
				myTransform->GetChildren()[i]->GetGameObject()->GetComponent<RigidBody>()->AddForceAtPos(myForce, { myCollisionOrigin.x + myExplosionOrigin.x, myCollisionOrigin.y + myExplosionOrigin.y, myCollisionOrigin.z + myExplosionOrigin.z }, physx::PxForceMode::eFORCE);
			}
		}
		myHasExploded = true;
	}

}

void ExplodeComponent::SetCollisionOrigin(Vector3f aCollisionOrigin)
{
	myCollisionOrigin = aCollisionOrigin;
}

void ExplodeComponent::RenderInProperties(std::vector<Component*>& /*aComponentList*/)
{
	if (myTransform->GetChildren().empty())
	{
		ImGui::Text("It'll have no effect without any Children!");
	}
	else
	{
		ImGui::DragFloat("Force", &myForce);
		ImGui::Text("Force Origin");
		ImGui::InputFloat("X", &myExplosionOrigin.x);
		ImGui::InputFloat("Y", &myExplosionOrigin.y);
		ImGui::InputFloat("Z", &myExplosionOrigin.z);
	}
	ImGui::Text("Make Sure the Children have Rigidbodies attached to them otherwise this thing isn't going to do anything!");
}

void ExplodeComponent::Save(rapidjson::Value& /*aComponentData*/)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();

	wrapper.SaveValue<DataType::Float>("Explosion Force", myForce);

	auto forceOriginPosition = wrapper.CreateArray();

	rapidjson::Value typeID;
	typeID.SetFloat(myExplosionOrigin.x);
	forceOriginPosition.PushBack(typeID, alloc);
	typeID.SetFloat(myExplosionOrigin.y);
	forceOriginPosition.PushBack(typeID, alloc);
	typeID.SetFloat(myExplosionOrigin.z);
	forceOriginPosition.PushBack(typeID, alloc);

	wrapper.SaveObject("Explosion Origin", forceOriginPosition);
}

void ExplodeComponent::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Explosion Force"))
	{
		myForce = aComponentData["Explosion Force"].GetFloat();
	}
	if (aComponentData.HasMember("Explosion Origin"))
	{
		myExplosionOrigin.x = aComponentData["Explosion Origin"][0].GetFloat();
		myExplosionOrigin.y = aComponentData["Explosion Origin"][1].GetFloat();
		myExplosionOrigin.z = aComponentData["Explosion Origin"][2].GetFloat();
	}
}
