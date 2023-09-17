#include "stdafx.h"
#include "CapsuleCollider.h"
#include "Components/Physics/CharacterController.h"
#include "Components/3D/ModelInstance.h"
#include "Components/Physics/RigidBody.h"
#include "Assets/Material.h"
#include "CollisionManager.h"

void CapsuleCollider::Awake()
{
	if (myShouldWaitForRigidbody == false)
	{
		if (myHeight < 0.01f)
		{
			myHeight = 0.01f;
		}
		else if (myRadius < 0.01f)
		{
			myRadius = 0.01f;
		}
		if (!myGameObject->GetComponent<RigidBody>() && !myGameObject->GetComponent<CharacterController>())
		{
			if (myGameObject->GetComponent<ModelInstance>())
			{
				myStatic = Engine::GetInstance()->GetPhysicsEngine()->CreateStaticActor(Shape::PxS_Capsule, Vector3f{ myRadius, myHeight, 0 }, myGameObject->GetComponent<ModelInstance>()->GetModel().get(), myGameObject->GetComponent<ModelInstance>()->GetMaterial(0)->GetName()
					, myDebugMode, myTransform->worldScale(), myTransform->worldRot(), myIsTrigger);
			}
			else
			{
				myStatic = Engine::GetInstance()->GetPhysicsEngine()->CreateStaticActor(Shape::PxS_Capsule, Vector3f{ myRadius, myHeight, 0 }, nullptr, "Default", myDebugMode, myTransform->worldScale(), myTransform->worldRot(), myIsTrigger);
			}
			if (myStatic)
			{
				physx::PxTransform aTransfrom = myStatic->getGlobalPose();
				aTransfrom.p.x = myTransform->worldPos().x;
				aTransfrom.p.y = myTransform->worldPos().y;
				aTransfrom.p.z = myTransform->worldPos().z;

				myStatic->setGlobalPose(aTransfrom);
				Engine::GetInstance()->GetPhysicsEngine()->AddActor(myStatic, myGameObject->GetObjectInstanceID(), myLayer);
			}
		}
	}
}

bool CapsuleCollider::IsInside(const Vector3f& aPoint) const
{
    return false;
}

void CapsuleCollider::DebugDraw()
{
	DebugDrawer::DrawLine(myTransform->worldPos(), myTransform->worldPos() + Vector3f::forward() * myRadius);
	DebugDrawer::DrawLine(myTransform->worldPos(), myTransform->worldPos() + Vector3f::down() * myHeight);
}

void CapsuleCollider::SetActorPosition(Vector3f aPosition)
{
	if (myStatic)
	{
		myTransform->SetWorldPos(aPosition);

		auto pose = myStatic->getGlobalPose();
		pose.p.x = aPosition.x;
		pose.p.y = aPosition.y;
		pose.p.z = aPosition.z;
		myStatic->setGlobalPose(pose);
	}
}

void CapsuleCollider::SetActorRotation(Vector3f aRotation)
{
	if (myStatic)
	{
		myTransform->SetWorldRot(aRotation);

		auto pose = myStatic->getGlobalPose();
		pose.q.x = aRotation.x;
		pose.q.y = aRotation.y;
		pose.q.z = aRotation.z;
		myStatic->setGlobalPose(pose);
	}
}

void CapsuleCollider::RenderInProperties(std::vector<Component*>& aComponentList)
{
	int layer = myLayer;

	std::vector<const char*> layerNames;
	auto& collisionLayers = Engine::GetInstance()->GetCollisionManager()->GetCollisionLayers();
	for (auto& layerName : collisionLayers)
	{
		layerNames.push_back(layerName.c_str());
	}

	ImGui::Checkbox("Is Trigger", &myIsTrigger);

	if (ImGui::Combo("Layer", &layer, layerNames.data(), static_cast<int>(layerNames.size())))
	{
		this->SetLayer(layer);
	}

	ImGui::InputFloat("Radius", &myRadius);
	ImGui::InputFloat("Height", &myHeight);

	Catbox::Checkbox("Should wait for RB", &myShouldWaitForRigidbody);
}

void CapsuleCollider::Update()
{
	if (myGameObject->IsSelected(0))
	{
		DebugDraw();
	}
}

void CapsuleCollider::RunInEditor()
{
	if (myGameObject->IsSelected(0))
	{
		DebugDraw();
	}
}


void CapsuleCollider::Save(rapidjson::Value& aComponentData)
{
	int version = 2;
	auto wrapper = RapidJsonWrapper::GetInstance();
	wrapper->SaveValue<DataType::Float>("Radius", myRadius);
	wrapper->SaveValue<DataType::Float>("Height", myHeight);
	wrapper->SaveValue<DataType::Int>("Version", version);
	wrapper->SaveValue<DataType::Int>("Layer", myLayer);
	wrapper->SaveValue<DataType::Bool>("Trigger", myIsTrigger);
	wrapper->SaveValue<DataType::Bool>("WaitForRB", myShouldWaitForRigidbody);
}

void CapsuleCollider::Load(rapidjson::Value& aComponentData)
{
	int version = aComponentData["Version"].GetInt();
	if (version >= 1)
	{
		myIsTrigger = aComponentData["Trigger"].GetBool();
	}
	if (version >= 2)
	{
		myShouldWaitForRigidbody = aComponentData["WaitForRB"].GetBool();
	}
	if (!myHasStarted)
	{
		myLayer = aComponentData["Layer"].GetInt();
	}
	else SetLayer(aComponentData["Layer"].GetInt());
	myHeight = aComponentData["Height"].GetFloat();
	myRadius = aComponentData["Radius"].GetFloat();

}
