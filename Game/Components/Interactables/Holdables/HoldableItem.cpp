#include "Game.pch.h"
#include "HoldableItem.h"
#include "Components\Physics\RigidBody.h"
#include "Components/Physics/Collisions/Collider.h"
#include "Components/Interactables/Surfaces/InteractableSurface.h"
#include "Physics/PhysicsEngine.h"
#include "Managers/StampManager.h"
#include "Components/Interactables/Holdables/Pot.h"
#include "Components/Interactables/Holdables/Skillet.h"

HoldableItem::~HoldableItem()
{
	Interactable::~Interactable();

	if (auto parent = myTransform->GetParent())
	{
		if (auto surface = parent->GetGameObject()->GetComponent<InteractableSurface>())
		{
			surface->ClearHeldItem();
		}
	}
}

void HoldableItem::Interact(Player* aPlayer)
{
	if (myCanInteract)
	{
		if (!aPlayer->GetHeldItem())
		{
			myIsBeingHeld = true;
			aPlayer->SetHeldItem(this);
		}
		else
		{
			myPlayerThrowing = nullptr;
			myIsBeingHeld = false;
			InteractWithHeldItem(aPlayer, aPlayer->GetHeldItem());
		}
	}
}

void HoldableItem::Awake()
{
	Interactable::Awake();
	myInteractPriority = 1;
	myPushDownStartValue = myPushDown;
	myAirResistenceStartValue = myAirResistence;
}


void HoldableItem::Update()
{
	Interactable::Update();
	if (myTeleportTimer > 0)
	{
		myTeleportTimer -= deltaTime;
	}
}

void HoldableItem::FixedUpdate()
{
	if (myIsBeingThrown)
	{
		RigidBody* throwableRB = GetGameObject().GetComponent<RigidBody>();

		Vector3f backArc = (-1.f) * myPlayerThrowingForward;
		throwableRB->AddForce(backArc, myAirResistence * fixedDeltaTime, physx::PxForceMode::eIMPULSE);
		Vector3f downArc = { 0,-1,0 };
		throwableRB->AddForce(downArc, myPushDown * fixedDeltaTime, physx::PxForceMode::eIMPULSE);

		myPushDown = myPushDown - fixedDeltaTime * myPushDownReductionSpeed;
		myAirResistence = myAirResistence + fixedDeltaTime * myAirResistenceAdditionSpeed;
		if (myPushDown < 0)
		{
			myPushDown = 0;
		}
		if (throwableRB->GetVelocity().Dot(myPlayerThrowingForward) < 0)
		{
			myAirResistence = 0;
		}

		myThrowingTimer = myThrowingTimer + fixedDeltaTime;
		if (myTimeUntilStopBeingThrown < myThrowingTimer)
		{
			myThrowingTimer = 0;
			myInteractableCollision = true;
		}

		if (!myIsSinking)
		{
			if (throwableRB->GetActorPosition().y < -5.f)
			{
				StampManager::GetInstance()->AddStampPoints(Stamp::FoodWaster, 1 , this->GetPlayerThrowing()->GetPlayerId() - 1);
				myPushDown = 0;
				myAirResistence = 0;
				GetGameObject().GetComponent<RigidBody>()->SetVelocity({ 0,1,0 });
				GetGameObject().GetComponent<RigidBody>()->SetDamping(5);
				myIsSinking = true;
			}
		}
		myCollisionTimer = myCollisionTimer + fixedDeltaTime;

	}
	else if (myTriggerCollision)
	{
		myTriggerCollision = false;
		StampManager::GetInstance()->AddStampPoints(Stamp::TrickShotter, 1, this->GetPlayerThrowing()->GetPlayerId() - 1);
		mySelectedInteractable->InteractWithHeldItem(nullptr, this);
		

		if(myTransform->GetParent() != nullptr)
		{
			GetGameObject().GetComponent<RigidBody>()->SetVelocity({ 0,0,0 });
			myTriggerStillActiveAfterCollision = false;
			myInteractableCollision = false;
			Reset();
			myPlayerThrowing = nullptr;
			mySelectedInteractable = nullptr;

			auto collide = InstantiatePrefab("ThrowingVFX");
			collide->GetTransform()->SetWorldPos((myTransform->worldPos()));

			if (myTransform->GetParent())
			{
				Engine::GetInstance()->GetPhysicsEngine()->RemoveShapeFromActor(GetGameObject().GetComponent<RigidBody>()->GetActor(), 1);
				GetGameObject().RemoveComponent(GetGameObject().GetComponent<RigidBody>());
			}
		}

	}
	else if (myInteractableCollision)
	{
		myInteractableCollision = false;
		
		if (RigidBody* rb = GetGameObject().GetComponent<RigidBody>())
		{
			rb->SetVelocity({ 0,0,0 });
			print("vel 0");
		}

		if (myHasHitAnimal)
		{
			myHasHitAnimal = false;
			auto funfun = InstantiatePrefab("DashIntoOtherPlayerVFX");
			funfun->GetTransform()->SetWorldPos(myTransform->worldPos());
		}
		else
		{
			auto collide = InstantiatePrefab("ThrowingVFX");
			collide->GetTransform()->SetWorldPos((myTransform->worldPos()));
		}

		Reset();
	}

	//Trigger on for a while after collission
	if(myTriggerStillActiveAfterCollision)
	{
		myTriggerStillActiveTimer = myTriggerStillActiveTimer + fixedDeltaTime;;
		if(myTriggerStillActiveTime < myTriggerStillActiveTimer)
		{
			myPlayerThrowing = nullptr;
			ResetTriggerStillActive();
		}
	}
}

void HoldableItem::ResetCatch()
{
	Engine::GetInstance()->GetPhysicsEngine()->RemoveShapeFromActor(GetGameObject().GetComponent<RigidBody>()->GetActor(), 1);
	GetGameObject().RemoveComponent(GetGameObject().GetComponent<RigidBody>());
	ResetTriggerStillActive();
	ResetAll();
}

void HoldableItem::ResetTriggerStillActive()
{
	mySelectedInteractable = nullptr;
	myTriggerStillActiveAfterCollision = false;
	myCollisionTimer = 0.f;
	myTriggerStillActiveTimer = 0.f;
}

void HoldableItem::Reset()
{
	myCollisionTimer = 0.f;
	if (!myTriggerStillActiveAfterCollision)
	{
		myPlayerThrowing = nullptr;
	}
	myPlayerThrowingForward = { 0,0,0 };
	myIsBeingThrown = false;

	myAirResistence = myAirResistenceStartValue;
	myPushDown = myPushDownStartValue;
}

void HoldableItem::ResetAll()
{
	myCollisionTimer = 0.f;
	myTriggerCollision = false;
	myInteractableCollision = false;
	Reset();
}


void HoldableItem::SetmyCanTriggerCollide(const bool& aBool)
{
	myTriggerCollision = aBool;
}

void HoldableItem::OnCollisionEnter(Collider* aCollider)
{
	if (myCollisionTimer > myNoCollisionTime)
	{
		if (myPlayerThrowing != nullptr)
		{
			if (aCollider == myPlayerThrowing->GetGameObject().GetComponent<Collider>())
			{
				return;
			}
		}

		myTriggerStillActiveAfterCollision = true;
		myIsBeingThrown = false;
		myInteractableCollision = true;

		
		auto colParent = aCollider->GetTransform()->GetParent();
		if (colParent)
		{
			if (colParent->GetGameObject()->GetName() == "Shark" || colParent->GetGameObject()->GetName() == "Axolotl")
			{
				myHasHitAnimal = true;
			}

		}
		
	}
}

void HoldableItem::OnTriggerEnter(Collider* aCollider)
{
	if(myCanTriggerCollide)
	{
		if (myCollisionTimer > myNoCollisionTime || myTriggerStillActiveAfterCollision)
		{
			if (Interactable* interactComponent = aCollider->GetGameObject().GetComponent<InteractableSurface>())
			{
				mySelectedInteractable = interactComponent;
				myTriggerCollision = true;
				myIsBeingThrown = false;
			}
		}
	}


}

void HoldableItem::OnTriggerExit(Collider* aCollider)
{

}


void HoldableItem::SetPlayerForward(const Vector3f& aForward)
{
	myPlayerThrowingForward = aForward;
}

void HoldableItem::RenderInProperties(std::vector<Component*>& aComponentList)
{
	ImGui::InputFloat("Mass##HoldableItem", &myMass);
}

void HoldableItem::Save(rapidjson::Value& aComponentData)
{
	int version = 0;
	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveValue<DataType::Float>("Mass", myMass);
}

void HoldableItem::Load(rapidjson::Value& aComponentData)
{
	int version = aComponentData["Version"].GetInt();

	if (version >= 0)
	{
		myMass = aComponentData["Mass"].GetFloat();
	}
}
