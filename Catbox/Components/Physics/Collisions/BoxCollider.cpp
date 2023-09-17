#include "stdafx.h"
#include "BoxCollider.h"
#include "CollisionManager.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Model.h"
#include "Components/Physics/RigidBody.h"
#include "Components/Physics/CharacterController.h"
#include "MuninGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"
#include "Physics/PhysicsEngine.h"



BoxCollider::BoxCollider()
{
	mySize = Vector3f::one();
	myOffset = Vector3f::zero();
	myColliderType = ColliderType::Box;
	myShouldWaitForRigidbody = false;
}

BoxCollider::~BoxCollider()
{
	if (myStatic) 
	{
		//myStatic->release();
		Engine::GetInstance()->GetPhysicsEngine()->RemoveActor(myStatic);
	}
}

void BoxCollider::Init()
{
	//if (myGameObject->GetComponent<ModelInstance>())
	//{
	//	mySize = (myGameObject->GetComponent<ModelInstance>()->GetModel()->GetCenter() * 2.0f);
	//	mySize.x = std::abs(mySize.x);
	//	mySize.y = std::abs(mySize.y);
	//	mySize.z = std::abs(mySize.z);
	//}
}

void BoxCollider::Awake()
{
	if (myShouldWaitForRigidbody == false)
	{
		if (mySize.x < 0.01f)
		{
			mySize.x = 0.01f;
		}
		else if (mySize.y < 0.01f)
		{
			mySize.y = 0.01f;
		}
		else if (mySize.z < 0.01f)
		{
			mySize.z = 0.01f;
		}
		if (!myGameObject->GetComponent<RigidBody>() && !myGameObject->GetComponent<CharacterController>())
		{
			if (myGameObject->GetComponent<ModelInstance>())
			{
				myStatic = Engine::GetInstance()->GetPhysicsEngine()->CreateStaticActor(Shape::PxS_Box, mySize, myGameObject->GetComponent<ModelInstance>()->GetModel().get(), myGameObject->GetComponent<ModelInstance>()->GetMaterial(myPhysxMaterialIndex)->GetName()
					, myDebugMode, myTransform->worldScale(), myTransform->worldRot(), myIsTrigger);
			}
			else
			{
				myStatic = Engine::GetInstance()->GetPhysicsEngine()->CreateStaticActor(Shape::PxS_Box, mySize, nullptr, "Default", myDebugMode, myTransform->worldScale(), myTransform->worldRot(), myIsTrigger);
			}
			if (myStatic)
			{
				physx::PxTransform aTransfrom = myStatic->getGlobalPose();
				aTransfrom.p.x = myTransform->worldPos().x;
				aTransfrom.p.y = myTransform->worldPos().y;
				aTransfrom.p.z = myTransform->worldPos().z;
				//Catbox::Quaternion aRot = Catbox::ToQuaternion({0,0,0});
				//aTransfrom.q.x = aRot.x;
				//aTransfrom.q.y = aRot.y;
				//aTransfrom.q.z = aRot.z;
				//aTransfrom.q.w = aRot.w;
				myStatic->setGlobalPose(aTransfrom);
				Engine::GetInstance()->GetPhysicsEngine()->AddActor(myStatic, myGameObject->GetObjectInstanceID(), myLayer);
			}
		}
	}
	//Collider::Awake();
}

void BoxCollider::SetSize(const Vector3f& aSize)
{
	mySize = aSize;
}

void BoxCollider::SetOffset(const Vector3f& anOffset)
{
	myOffset = anOffset;
}

bool BoxCollider::IsInside(const Vector3f& aPoint) const
{
	float epsilon = 0.0001f;
	const Vector3f min = GetMinCorner();
	const Vector3f max = GetMaxCorner();

	if (aPoint.x < min.x - epsilon) return false;
	if (aPoint.x > max.x + epsilon) return false;
	if (aPoint.y < min.y - epsilon) return false;
	if (aPoint.y > max.y + epsilon) return false;
	if (aPoint.z < min.z - epsilon) return false;
	if (aPoint.z > max.z + epsilon) return false;

	return true;
}


const Vector3f BoxCollider::GetMinCorner() const
{
	return myTransform->worldPos() + myOffset - (mySize * myTransform->worldScale() / 2.f);
}

const Vector3f BoxCollider::GetMaxCorner() const
{
	return myTransform->worldPos() + myOffset + (mySize * myTransform->worldScale() / 2.f);
}

void BoxCollider::RunInEditor()
{
	if (myGameObject->IsSelected(0))
	{
		DebugDraw();
	}
}

void BoxCollider::Update()
{
	if (myGameObject->IsSelected(0))
	{
		DebugDraw();
	}
}

void BoxCollider::DebugDraw()
{
	if (myGameObject->GetComponent<ModelInstance>() && myGameObject->GetComponent<ModelInstance>()->GetModel())
	{
		DebugDrawer::DrawCube(myTransform->worldPos(), myTransform->worldScale() * mySize, myTransform->worldRot(), myCurrentCollisions.empty() ? Color::Green() : Color::Red(), (myGameObject->GetComponent<ModelInstance>()->GetModel()->GetCenter()) * myTransform->worldScale());
	}
	else 
	{
		DebugDrawer::DrawCube(myTransform->worldPos() + myOffset, myTransform->worldScale() * mySize, Vector3f::zero(), myCurrentCollisions.empty() ? Color::Green() : Color::Red());
	}
}


void BoxCollider::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto colliders = ComponentVectorCast<BoxCollider>(aComponentList);
	Vector3f size = mySize;
	Vector3f offset = myOffset;
	int layer = myLayer;


	for (auto& collider : colliders)
	{

		if (collider->myLayer != myLayer)
		{
			layer = -1;
			break;
		}
	}

	std::vector<const char*> layerNames;
	auto& collisionLayers = Engine::GetInstance()->GetCollisionManager()->GetCollisionLayers();
	for (auto& layerName : collisionLayers)
	{
		layerNames.push_back(layerName.c_str());
	}

	if (ImGui::Checkbox("Is Trigger", &myIsTrigger))
	{
		for (auto& collider : colliders)
		{
			collider->myIsTrigger = myIsTrigger;
		}
	}

	if (ImGui::Combo("Layer", &layer, layerNames.data(), static_cast<int>(layerNames.size())))
	{
		for (auto& collider : colliders)
		{
			collider->SetLayer(layer);
		}
	}

	if (Catbox::DragFloat3("Size", &mySize, 0.01f, 0, 1000.f))
	{
		for (auto& collider : colliders)
		{
			if (mySize.x < 0.01f) 
			{
				mySize.x = 0.01f;
			}
			else if (mySize.y < 0.01f)
			{
				mySize.y = 0.01f;
			}
			else if (mySize.z < 0.01f)
			{
				mySize.z = 0.01f;
			}
			collider->mySize = mySize;
		}
	}

	if (Catbox::DragFloat3("Offset", &myOffset, 0.01f, 1000.f, 1000.f))
	{
		for (auto& collider : colliders)
		{
			collider->myOffset = myOffset;
		}
	}

	Catbox::Checkbox("Debug Mode", &myDebugMode);
	Catbox::Checkbox("Should wait for RB", &myShouldWaitForRigidbody);

	if (Catbox::InputInt("Physx Material Index", &myPhysxMaterialIndex)) 
	{
		auto modelInstance = myGameObject->GetComponent<ModelInstance>();
		if (modelInstance) 
		{
			if (modelInstance->GetMaterials().size() < myPhysxMaterialIndex) 
			{
				myPhysxMaterialIndex = modelInstance->GetMaterials().size() - 1;
			}
			else if (myPhysxMaterialIndex < 0) 
			{
				myPhysxMaterialIndex = 0;
			}
		}
	}
}

void BoxCollider::SetActorPosition(Vector3f aPosition)
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

void BoxCollider::SetActorRotation(Vector3f aRotation)
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

//void BoxCollider::OnTransformChanged()
//{
//	if (myStatic) 
//	{
//		auto pose = myStatic->getGlobalPose();
//		pose.p.x = myTransform->worldPos().x;
//		pose.p.y = myTransform->worldPos().y;
//		pose.p.z = myTransform->worldPos().z;
//		myStatic->setGlobalPose(pose);
//	}
//}

void BoxCollider::OnCollisionEnter(Collider* aCollider)
{
	//print("Enter");
}

void BoxCollider::OnTriggerEnter(Collider* aCollider)
{
	//print(aCollider->GetGameObject().GetName());
}

void BoxCollider::OnCollisionExit(Collider* aCollider)
{
	//print("Exit");
}

void BoxCollider::Save(rapidjson::Value& /*aComponentData*/)
{
	int version = 3;
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();
	wrapper.SaveValue<DataType::Int>("Version", version);

	wrapper.SaveValue<DataType::Int>("Layer", myLayer);

	wrapper.SaveValue<DataType::Bool>("Trigger", myIsTrigger);

	auto sizeArray = wrapper.CreateArray();
	sizeArray.SetArray();
	sizeArray.PushBack(mySize.x, alloc);
	sizeArray.PushBack(mySize.y, alloc);
	sizeArray.PushBack(mySize.z, alloc);
	wrapper.SaveObject("Size", sizeArray);

	auto offsetArray = wrapper.CreateArray();
	offsetArray.SetArray();
	offsetArray.PushBack(myOffset.x, alloc);
	offsetArray.PushBack(myOffset.y, alloc);
	offsetArray.PushBack(myOffset.z, alloc);
	wrapper.SaveObject("Offset", offsetArray);

	wrapper.SaveValue<DataType::Bool>("WaitForRB", myShouldWaitForRigidbody);
	wrapper.SaveValue<DataType::Int>("Material Index", myPhysxMaterialIndex);
}

void BoxCollider::Load(rapidjson::Value& aComponentData)
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
	if (version >= 3) 
	{
		myPhysxMaterialIndex = aComponentData["Material Index"].GetInt();
	}
	if (!myHasStarted)
	{
		myLayer = aComponentData["Layer"].GetInt();
	}
	else SetLayer(aComponentData["Layer"].GetInt());
	mySize.x = aComponentData["Size"][0].GetFloat();
	mySize.y = aComponentData["Size"][1].GetFloat();
	mySize.z = aComponentData["Size"][2].GetFloat();

	myOffset.x = aComponentData["Offset"][0].GetFloat();
	myOffset.y = aComponentData["Offset"][1].GetFloat();
	myOffset.z = aComponentData["Offset"][2].GetFloat();
}
