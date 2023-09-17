#include "stdafx.h"
#include "RigidBody.h"
#include "../Physics/Intersection.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Model.h"
#include <iostream>
#include "Collisions/BoxCollider.h"
#include "Collisions/SphereCollider.h"
#include "Physics/PhysicsEngine.h"
#include "Physics/PhysXUtilities.h"
#include "foundation/PxVecQuat.h"
#include "Collisions/CapsuleCollider.h"

RigidBody::~RigidBody()
{
	if (myActor)
	{
	//	myActor->release();
		Engine::GetInstance()->GetPhysicsEngine()->RemoveActor(myActor);
	}
}

RigidBody::RigidBody(physx::PxRigidActor* aActor)
{
	myActor = aActor;
	myShouldCull = false;
}

void RigidBody::Awake()
{
	if (myHasRunAwake == true) { return; }


	Model* aModel = nullptr;
	std::string aMaterialName = "Default";
	int myLayer = 0;
	if (myGameObject->GetComponent<ModelInstance>()) 
	{
		aModel = myGameObject->GetComponent<ModelInstance>()->GetModel().get();
		aMaterialName = myGameObject->GetComponent<ModelInstance>()->GetMaterial(0)->GetName();
	}
	if (myGameObject->GetComponent<BoxCollider>())
	{
		myDebugMode = myGameObject->GetComponent<BoxCollider>()->GetDebugMode();
		myActor = static_cast<physx::PxRigidActor*>(Engine::GetInstance()->GetPhysicsEngine()->CreateDynamicActor(Shape::PxS_Box, myGameObject->GetComponent<BoxCollider>()->GetSize(), aModel, aMaterialName, myMass, myDebugMode, myTransform->worldScale(), myGameObject->GetComponent<BoxCollider>()->GetIsTrigger()));
		myLayer = myGameObject->GetComponent<BoxCollider>()->GetCollisionLayer();
	}
	else if (myGameObject->GetComponent<SphereCollider>())
	{
		myDebugMode = myGameObject->GetComponent<SphereCollider>()->GetDebugMode();
		myActor = static_cast<physx::PxRigidActor*>(Engine::GetInstance()->GetPhysicsEngine()->CreateDynamicActor(Shape::PxS_Circle, Vector3f(myGameObject->GetComponent<SphereCollider>()->GetRadius(), 0.0f, 0.0f) , aModel, aMaterialName, myMass, myDebugMode, myTransform->worldScale(), myGameObject->GetComponent<SphereCollider>()->GetIsTrigger()));
		myLayer = myGameObject->GetComponent<SphereCollider>()->GetCollisionLayer();
	}
	else if (myGameObject->GetComponent<CapsuleCollider>())
	{
		myDebugMode = myGameObject->GetComponent<CapsuleCollider>()->GetDebugMode();
		myActor = static_cast<physx::PxRigidActor*>(Engine::GetInstance()->GetPhysicsEngine()->CreateDynamicActor(Shape::PxS_Capsule, Vector3f(myGameObject->GetComponent<CapsuleCollider>()->GetRadius(), myGameObject->GetComponent<CapsuleCollider>()->GetHeight(), 0.0f), aModel, aMaterialName, myMass, myDebugMode, myTransform->worldScale(), myGameObject->GetComponent<CapsuleCollider>()->GetIsTrigger()));
		myLayer = myGameObject->GetComponent<CapsuleCollider>()->GetCollisionLayer();
	}
	if (myMass <= 0 || !myGravity) 
	{
		myActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
	}
	if (myActor) 
	{
		physx::PxTransform aTransfrom = myActor->getGlobalPose();
		aTransfrom.p.x = myTransform->worldPos().x;
		aTransfrom.p.y = myTransform->worldPos().y;
		aTransfrom.p.z = myTransform->worldPos().z;
		Catbox::Quaternion aRot = Catbox::Quaternion::ToQuaternion(myTransform->worldRot());
		aTransfrom.q.x = aRot.x;
		aTransfrom.q.y = aRot.y;
		aTransfrom.q.z = aRot.z;
		aTransfrom.q.w = aRot.w;
		myActor->setGlobalPose(aTransfrom);
		//Sets Lock Flags set in Editor, if it doesn't do it here then changes will only work during playmode.
		physx::PxRigidDynamic* castedActor = static_cast<physx::PxRigidDynamic*>(myActor);
		if (castedActor) 
		{
			//castedActor->setMassSpaceInertiaTensor(physx::PxVec3(1.0f, 0.0f, 1.0f));
			//physx::PxTransform localTransfrom;
			//localTransfrom.p.x = myCenterOfMass.x;
			//localTransfrom.p.y = myCenterOfMass.y;
			//localTransfrom.p.z = myCenterOfMass.z;
			//castedActor->setCMassLocalPose(localTransfrom);

			castedActor->setLinearDamping(myDampening);
			castedActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, myLockTranslations[0]);
			castedActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, myLockTranslations[1]);
			castedActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, myLockTranslations[2]);
			castedActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, myLockRotations[0]);
			castedActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, myLockRotations[1]);
			castedActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, myLockRotations[2]);

			castedActor->setMass(myMass);

			//Vector3f centerOfMass = myGameObject->GetTransform()->GetCenter();
			//castedActor->setCMassLocalPose({ 0, 1, 0 });
		}
		Engine::GetInstance()->GetPhysicsEngine()->AddActor(myActor, myGameObject->GetObjectInstanceID(), myLayer);
	}

	myHasRunAwake = true;
}

void RigidBody::RenderInProperties(std::vector<Component*>& aComponentList)
{
	std::vector<RigidBody*>& aComponentVector = ComponentVectorCast<RigidBody>(aComponentList);
	if(ImGui::Checkbox("Use Gravity", &myGravity))
	{
		for (auto& component : aComponentVector)
		{
			component->myGravity = myGravity;
		}
	}
	if (ImGui::InputFloat("Mass", &myMass))
	{
		for (auto& component : aComponentVector)
		{
			if (PLAYMODE)
			{
				component->ChangeMass(myMass);
			}
			else
			{
				component->myMass = myMass;
			}
		}
	}
	if (Catbox::DragFloat3("Center of Mass", &myCenterOfMass)) 
	{
		for (auto& component : aComponentVector)
		{
			component->myCenterOfMass = myCenterOfMass;
		}
	}

	if (Catbox::DragFloat("Dampening", &myDampening)) 
	{
		for (auto& component : aComponentVector)
		{
			if (PLAYMODE) 
			{
				component->SetDamping(myDampening);
			}
			else 
			{
				component->myDampening = myDampening;
			}
		}
	}

	ImGui::Text("Lock Translation");
	if (ImGui::Checkbox("X", &myLockTranslations[0]))
	{
		if (myActor)
		{
			for (auto& component : aComponentVector)
			{
				component->myLockTranslations[0] = myLockTranslations[0];
				static_cast<physx::PxRigidDynamic*>(component->myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, myLockTranslations[0]);
				static_cast<physx::PxRigidDynamic*>(component->myActor)->wakeUp();
			}
		}
		else
		{
			for (auto& component : aComponentVector)
			{
				component->myLockTranslations[0] = myLockTranslations[0];
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Y", &myLockTranslations[1]))
	{
		if (myActor)
		{
			for (auto& component : aComponentVector)
			{
				component->myLockTranslations[1] = myLockTranslations[1];
				static_cast<physx::PxRigidDynamic*>(component->myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, myLockTranslations[1]);
				static_cast<physx::PxRigidDynamic*>(component->myActor)->wakeUp();
			}
		}
		else
		{
			for (auto& component : aComponentVector)
			{
				component->myLockTranslations[1] = myLockTranslations[1];
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Z", &myLockTranslations[2]))
	{
		if (myActor)
		{
			for (auto& component : aComponentVector)
			{
				component->myLockTranslations[2] = myLockTranslations[2];
				static_cast<physx::PxRigidDynamic*>(component->myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, myLockTranslations[2]);
				static_cast<physx::PxRigidDynamic*>(component->myActor)->wakeUp();
			}
		}
		else
		{
			for (auto& component : aComponentVector)
			{
				component->myLockTranslations[2] = myLockTranslations[2];
			}
		}
	}
	ImGui::Text("Lock Rotation");

	if (ImGui::Checkbox("X##", &myLockRotations[0]))
	{
		if (myActor)
		{
			for (auto& component : aComponentVector)
			{
				component->myLockRotations[0] = myLockRotations[0];
				static_cast<physx::PxRigidDynamic*>(component->myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, myLockRotations[0]);
				static_cast<physx::PxRigidDynamic*>(component->myActor)->wakeUp();
			}
		}
		else
		{
			for (auto& component : aComponentVector)
			{
				component->myLockRotations[0] = myLockRotations[0];
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Y##", &myLockRotations[1]))
	{
		if (myActor)
		{
			for (auto& component : aComponentVector)
			{
				component->myLockRotations[1] = myLockRotations[1];
				static_cast<physx::PxRigidDynamic*>(component->myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, myLockRotations[1]);
				static_cast<physx::PxRigidDynamic*>(component->myActor)->wakeUp();
			}
		}
		else
		{
			for (auto& component : aComponentVector)
			{
				component->myLockRotations[1] = myLockRotations[1];
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Z##", &myLockRotations[2]))
	{
		if (myActor)
		{
			for (auto& component : aComponentVector)
			{
				component->myLockRotations[2] = myLockRotations[2];
				static_cast<physx::PxRigidDynamic*>(component->myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, myLockRotations[2]);
				static_cast<physx::PxRigidDynamic*>(component->myActor)->wakeUp();
			}
		}
		else
		{
			for (auto& component : aComponentVector)
			{
				component->myLockRotations[2] = myLockRotations[2];
			}
		}
	}

	if (ImGui::Checkbox("Lerp Position", &myShouldLerp))
	{
		for (auto& component : aComponentVector)
		{
			component->myShouldLerp = myShouldLerp;
		}
	}
}

void RigidBody::Update()
{
	if (myShouldLerp) 
	{
		if (myTransform->worldPos().Distance(myNewPos) > 5.0f) 
		{
			myTransform->SetWorldPos(myNewPos);
		}
		else 
		{
			Vector3f lerpPos = Catbox::Lerp<Vector3f>(myTransform->worldPos(), myNewPos, 6.0f * deltaTime);
			myTransform->SetWorldPos(lerpPos);
		}
	}
	else 
	{
		myTransform->SetWorldPos(myNewPos);
	}
	//myTransform->SetWorldRot(myNewRot);
}

void RigidBody::FixedUpdate()
{
	if (myActor == nullptr) { return; }
	physx::PxRigidDynamic* castedActor = static_cast<physx::PxRigidDynamic*>(myActor);
	if (castedActor->isSleeping() && !myShouldSleep)
	{
		castedActor->wakeUp();
	}
	auto pose = myActor->getGlobalPose();
	float x = myActor->getGlobalPose().p.x /*+ (myTransform->worldScale().x * 0.5f)*/;
	float y = myActor->getGlobalPose().p.y /*- (myTransform->worldScale().y * 0.5f)*/;
	float z = myActor->getGlobalPose().p.z /*- (myTransform->worldScale().z * 0.5f)*/;
	Catbox::Quaternion aQuat;
	aQuat.w = myActor->getGlobalPose().q.w;
	aQuat.x = myActor->getGlobalPose().q.x;
	aQuat.y = myActor->getGlobalPose().q.y;
	aQuat.z = myActor->getGlobalPose().q.z;

	myNewRot = PhysXUtilities::quaternion2Euler(pose.q);
	/*if (myTransform->worldRot() != newRot)*/ myTransform->SetWorldRot(myNewRot);

	myNewPos = Vector3f(x, y, z);
	//myTransform->SetWorldPos(newPos);

}

void RigidBody::RunInEditor()
{

}

void RigidBody::SetDamping(float aValue)
{
	auto castedActor = static_cast<physx::PxRigidDynamic*>(myActor);
	if(castedActor) 
	{
		castedActor->setLinearDamping(aValue);
		castedActor->setAngularDamping(aValue);
	}
}

RigidBody::RigidBodyData& RigidBody::GetData()
{
	return myRbData;
}

Vector3f& RigidBody::GetVelocity()
{
	auto speed = static_cast<physx::PxRigidDynamic*>(myActor)->getLinearVelocity();
	myVelocity = { speed.x, speed.y, speed.z };
	return myVelocity;
}

void RigidBody::Rotate(const Vector3f& angles)
{
	physx::PxTransform transform = myActor->getGlobalPose();
	float cr = std::cos(Catbox::Deg2Rad(angles.x));
	float sr = std::sin(Catbox::Deg2Rad(angles.x));
	float cp = std::cos(Catbox::Deg2Rad(angles.y));
	float sp = std::sin(Catbox::Deg2Rad(angles.y));
	float cy = std::cos(Catbox::Deg2Rad(angles.z));
	float sy = std::sin(Catbox::Deg2Rad(angles.z));
	physx::PxMat33 rot;
	rot.column0 = (physx::PxVec3(cy * cp, cy * sp * sr - sy * cr, cy * sp * cr + sy * sr));
	rot.column1 = (physx::PxVec3(sy * cp, sy * sp * sr + cy * cr, sy * sp * cr - cy * sr));
	rot.column2 = (physx::PxVec3(-sp, cp * sr, cp * cr));
	transform.q *= physx::PxQuat(rot);
	myActor->setGlobalPose(transform);
}

void RigidBody::SetActorPosition(const Vector3f& aPos) 
{
	physx::PxTransform atransfrom;
	atransfrom.p.x = aPos.x;
	atransfrom.p.y = aPos.y;
	atransfrom.p.z = aPos.z;
	atransfrom.q = myActor->getGlobalPose().q;
	myActor->setGlobalPose(atransfrom);
	myNewPos = aPos;
}
Vector3f RigidBody::GetActorPosition()
{
	auto pos = myActor->getGlobalPose();
	return { pos.p.x, pos.p.y, pos.p.z };
}
void RigidBody::SetActorRotation(const Vector3f& aRot)
{
	physx::PxTransform atransfrom;
	Catbox::Quaternion aQuat = Catbox::Quaternion::ToQuaternion(aRot);
	atransfrom.q.x = aQuat.x;
	atransfrom.q.y = aQuat.y;
	atransfrom.q.z = aQuat.z;
	atransfrom.q.w = aQuat.w;

	atransfrom.p = myActor->getGlobalPose().p;
	myActor->setGlobalPose(atransfrom);
}


void RigidBody::SetVelocity(Vector3f aDirection, float aForce)
{
	physx::PxVec3 velocityDirection =
	{
		aDirection.x,
		aDirection.y,
		aDirection.z
	};
	velocityDirection = velocityDirection.getNormalized() * aForce;
	static_cast<physx::PxRigidDynamic*>(myActor)->setLinearVelocity(velocityDirection);
}

void RigidBody::SetVelocity(Vector3f aVelocity)
{
	physx::PxVec3 velocity =
	{
		aVelocity.x,
		aVelocity.y,
		aVelocity.z
	};
	static_cast<physx::PxRigidDynamic*>(myActor)->setLinearVelocity(velocity);
}

void RigidBody::SetAngularVelocity(Vector3f aDirection, float aForce)
{
	physx::PxVec3 velocityDirection =
	{
		aDirection.x,
		aDirection.y,
		aDirection.z
	};
	velocityDirection = velocityDirection.getNormalized() * aForce;
	static_cast<physx::PxRigidDynamic*>(myActor)->setAngularVelocity(velocityDirection);
}

void RigidBody::SetMaximumLinearVelocity(float aMaxVelocity)
{
	physx::PxRigidDynamic* castedActor = static_cast<physx::PxRigidDynamic*>(myActor);
	castedActor->setMaxLinearVelocity(aMaxVelocity);
}

void RigidBody::SetMaximumAngularVelocity(float aMaxVelocity)
{
	physx::PxRigidDynamic* castedActor = static_cast<physx::PxRigidDynamic*>(myActor);
	castedActor->setMaxAngularVelocity(aMaxVelocity);
}

Vector3f RigidBody::AddForce(Vector3f aDirection, float aForce, physx::PxForceMode::Enum aMode)
{
	static_cast<physx::PxRigidDynamic*>(myActor)->addForce(physx::PxVec3(aDirection.x, aDirection.y, aDirection.z) * aForce, aMode);
	return { aDirection.x * aForce, aDirection.y * aForce, aDirection.z * aForce };
}

Vector3f RigidBody::AddForceAtPos(const float aForce, const Vector3f aPosition, physx::PxForceMode::Enum aMode)
{
	physx::PxVec3 forceDirection =
	{
		myActor->getGlobalPose().p.x - aPosition.x,
		myActor->getGlobalPose().p.y - aPosition.y,
		myActor->getGlobalPose().p.z - aPosition.z
	};
	forceDirection = forceDirection.getNormalized() * aForce;
	physx::PxRigidBodyExt::addForceAtPos(*static_cast<physx::PxRigidBody*>(myActor), forceDirection, physx::PxVec3(aPosition.x, aPosition.y, aPosition.z), aMode);
	return { forceDirection.x, forceDirection.y, forceDirection.z };
}

void RigidBody::ScheduleForce(const Vector3f aPosition, physx::PxForceMode::Enum aMode)
{
	myScheduledForce = aPosition;
	myScheduledForceMode = aMode;
}

void RigidBody::ChangeGravityScale(bool aGravityScale)
{
	myGravity = aGravityScale;
	if (myActor) 
	{
		if (myMass <= 0 || !myGravity)
		{
			myActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
		}
		else 
		{
			myActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);

		}
	}
}

void RigidBody::ChangeMass(float aMass)
{
	static_cast<physx::PxRigidDynamic*>(myActor)->setMass(aMass);
}

void RigidBody::SetTranslationAxisLock(bool x, bool y, bool z, bool shouldWakeUp)
{
	static_cast<physx::PxRigidDynamic*>(myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, x);
	static_cast<physx::PxRigidDynamic*>(myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, y);
	static_cast<physx::PxRigidDynamic*>(myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, z);
	
	myLockTranslations[0] = x;
	myLockTranslations[1] = y;
	myLockTranslations[2] = z;

	if (shouldWakeUp)
	{
		static_cast<physx::PxRigidDynamic*>(myActor)->wakeUp();
	}
}

void RigidBody::SetRotationAxisLock(bool x, bool y, bool z, bool shouldWakeUp)
{
	static_cast<physx::PxRigidDynamic*>(myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, x);
	static_cast<physx::PxRigidDynamic*>(myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, y);
	static_cast<physx::PxRigidDynamic*>(myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, z);

	myLockRotations[0] = x;
	myLockRotations[1] = y;
	myLockRotations[2] = z;

	if (shouldWakeUp)
	{
		static_cast<physx::PxRigidDynamic*>(myActor)->wakeUp();
	}
}

bool RigidBody::GetTranslationAxisLock(Axis anAxis)
{
	physx::PxU8 flags = static_cast<physx::PxRigidDynamic*>(myActor)->getRigidDynamicLockFlags();
	switch (anAxis)
	{
	case Axis::x:
		return flags & physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
	case Axis::y:
		return flags & physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
	case Axis::z:
		return flags & physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
	default:
		return false;
	}
}

bool RigidBody::GetRotationAxisLock(Axis anAxis)
{
	physx::PxU8 flags = static_cast<physx::PxRigidDynamic*>(myActor)->getRigidDynamicLockFlags();
	switch (anAxis)
	{
	case Axis::x:
		return flags & physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
	case Axis::y:
		return flags & physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
	case Axis::z:
		return flags & physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
	default:
		return false;
	}
}

void RigidBody::WakeUp()
{
	if (myActor)
	{
		myShouldSleep = false;
		physx::PxRigidDynamic* castedActor = static_cast<physx::PxRigidDynamic*>(myActor);
		castedActor->wakeUp();
	}
}

void RigidBody::PutToSleep()
{
	if (myActor)
	{
		myShouldSleep = true;
		physx::PxRigidDynamic* castedActor = static_cast<physx::PxRigidDynamic*>(myActor);
		castedActor->putToSleep();
	}
}

void RigidBody::SetLayer(const int aLayerID, const int aShapeID)
{
	Engine::GetInstance()->GetPhysicsEngine()->SetShapeLayer(myActor, aShapeID, aLayerID);
	BoxCollider* aCollider = myGameObject->GetComponent<BoxCollider>();
	if (aCollider)
	{
		aCollider->SetLayer(aLayerID);
	}
}

void RigidBody::SetKinematic()
{
	physx::PxRigidDynamic* castedActor = static_cast<physx::PxRigidDynamic*>(myActor);
	if (castedActor)
	{
		castedActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	}
}

void RigidBody::OnDisable()
{
	if (!EDITORMODE)
	{
		DisableSimulation();
	}
}

void RigidBody::OnEnable()
{
	if (!EDITORMODE)
	{
		SetActorPosition(myGameObject->GetTransform()->worldPos());
		EnableSimulation();
	}
}

void RigidBody::DisableSimulation()
{
	//myPreviousVelocity = GetVelocity();
	myActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
}

void RigidBody::EnableSimulation()
{
	myActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);
	//SetVelocity(myPreviousVelocity);
}


void RigidBody::Save(rapidjson::Value& /*aComponentData*/)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Bool, bool>("Use Gravity", myGravity);
	wrapper.SaveValue<DataType::Float, float>("Mass", myMass);

	wrapper.SaveValue<DataType::Bool, bool>("Lock Translation X", myLockTranslations[0]);
	wrapper.SaveValue<DataType::Bool, bool>("Lock Translation Y", myLockTranslations[1]);
	wrapper.SaveValue<DataType::Bool, bool>("Lock Translation Z", myLockTranslations[2]);
	wrapper.SaveValue<DataType::Bool, bool>("Lock Rotation X", myLockRotations[0]);
	wrapper.SaveValue<DataType::Bool, bool>("Lock Rotation Y", myLockRotations[1]);
	wrapper.SaveValue<DataType::Bool, bool>("Lock Rotation Z", myLockRotations[2]);

	wrapper.SaveValue<DataType::Float, float>("Dampening", myDampening);
	wrapper.SaveValue<DataType::Bool, bool>("ShouldLerp", myShouldLerp);
}
void RigidBody::Load(rapidjson::Value& aComponentData)
{	
	if (aComponentData.HasMember("Use Gravity")) 
	{
		myGravity = aComponentData["Use Gravity"].GetBool();
		myMass = aComponentData["Mass"].GetFloat();
	}

	if (aComponentData.HasMember("Lock Translation X"))
	{
		myLockTranslations[0] = aComponentData["Lock Translation X"].GetBool();
		myLockTranslations[1] = aComponentData["Lock Translation Y"].GetBool();
		myLockTranslations[2] = aComponentData["Lock Translation Z"].GetBool();
		myLockRotations[0] = aComponentData["Lock Rotation X"].GetBool();
		myLockRotations[1] = aComponentData["Lock Rotation Y"].GetBool();
		myLockRotations[2] = aComponentData["Lock Rotation Z"].GetBool();
	}		
	if (aComponentData.HasMember("Dampening"))
	{
		myDampening = aComponentData["Dampening"].GetFloat();
	}
	if(aComponentData.HasMember("ShouldLerp")) 
	{
		myShouldLerp = aComponentData["ShouldLerp"].GetBool();
	}
}