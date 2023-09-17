#include "Game.pch.h"
#include "Teleporter.h"
#include "Components/Physics/Collisions/Collider.h"
#include "Components/Player/Player.h"
#include "../Game/Managers/GameManager.h"
#include "Components/Interactables/Holdables/HoldableItem.h";
#include "Components/Environment/MovingInteractableItems.h";

#include "Assets/Material.h"
#include "Components/3D/ModelInstance.h"
void Teleporter::Awake()
{
	myAnimTime = myAnimTimeStandard;
	vector<Transform*> positions = myTransform->GetParent()->GetChildren();

	for (int i = 0; i < positions.size(); i++)
	{
		if (!(myTransform->GetGameObject()->GetObjectInstanceID() == positions[i]->GetGameObject()->GetObjectInstanceID()))
		{
			myOtherTeleporter = positions[i]->GetGameObject()->GetTransform();
		}
	}

	if (myTransform->GetChildren().size() > 0)
	{
		auto portalTransform = myTransform->GetChildren()[0]->GetGameObject()->GetTransform();
		portalTransform->SetLocalPos(0.f, 0.2f, 0.5f);
		portalTransform->SetLocalRot(-16.4f, 5.3f, 5.8f);
		portalTransform->SetLocalScale(0.65f, 1.f, 1.f);
	}
}


void Teleporter::FixedUpdate()
{
	if (goOutCoolDown)
	{
		timer += fixedDeltaTime;
		if (myGoOutWaitTime < timer)
		{
			timer = 0;
			goOutCoolDown = false;
			object = nullptr;
		}
		return;
	}


	if (teleport && object)
	{
		if (coolDown == false)
		{
			StartCooldown();
		}
		else if (coolDown)
		{
			timer += fixedDeltaTime;
			if (myAnimTime < timer)
			{
				if (myDoAnimeOnce)
				{
					if (object->GetComponent<Player>())
					{
						object->GetComponent<ModelInstance>()->SetEnabled(false);
					}
					myDoAnimeOnce = false;
					myAnimTime = myAnimTimeStandard;
				}
			}
			if (myWaitTime < timer)
			{
				FinishTeleporting();
			}
		}
	}
	else if (teleport && !object)
	{
		teleport = false;
	}

	std::vector<Player*> players = GameManager::GetInstance()->GetPlayers();

	bool hasPlayerNearby = false;
	for (auto player : players)
	{
		//auto rigid = player->GetGameObject().GetComponent<RigidBody>();

		//if (rigid)
		{
			Vector3f diff = player->GetTransform()->worldPos() - myTransform->worldPos();
			float distSqr = diff.LengthSqr();
			const float effectReactRangeSqr = 9;
			//float dot = player->GetTransform()->forward().Dot(myTransform->forward());
			float playerZ = player->GetTransform()->worldPos().z;
			float myZ = myTransform->worldPos().z;


			if (distSqr < effectReactRangeSqr && playerZ < myZ)
			{
				hasPlayerNearby = true;
				break;
			}
		}
	}

	if (hasPlayerNearby)
	{
		myGameObject->SetCustomBool(true);
		myDistanceToPlayers += myVortexSlider * fixedDeltaTime;
	}
	else
	{
		myGameObject->SetCustomBool(false);
		myDistanceToPlayers -= myVortexSlider * fixedDeltaTime;
	}

	myDistanceToPlayers = std::clamp(myDistanceToPlayers, 0.2f, 0.5f);
	//print(std::to_string(myDistanceToPlayers));
	//myGameObject->SetCustomFloat(myDistanceToPlayers);
	if (myTransform->GetChildren().size() > 0)
	{
		auto portal = myTransform->GetChildren()[0]->GetGameObject();//->SetCustomFloat(myDistanceToPlayers);
		portal->SetCustomFloat(myDistanceToPlayers);
		//portal->GetTransform()->SetLocalRot(0.f, myTransform->)
	}
}

void Teleporter::Update()
{
	//Check obj in range
	/*if (myObjectsInRange.size() != 0)
	{
		print("Start:");
		for (GameObject* objects : myObjectsInRange)
		{
			print("C" + objects->GetName());
		}
		print("End");
	}*/

	if (teleport) return;
	int index = 0;
	for (auto objectInRange : myObjectsInRange)
	{
		//check if item is on counter
		if (HoldableItem* holdItem = objectInRange->GetComponent<HoldableItem>())
		{
			if (objectInRange->GetTransform()->GetParent() != nullptr)
			{
				myObjectsInRange.erase(myObjectsInRange.begin() + index);
			}
		}
		index++;

		float dot = (myTransform->worldPos() + myTransform->back() * 0.5f - objectInRange->GetTransform()->worldPos()).GetNormalized().Dot(myTransform->back());
		bool isInFrontCheck = dot > 0.2f;
		if (!isInFrontCheck) continue;


		bool isPlayer = objectInRange->GetComponent<Player>();

		if (isPlayer)
		{
			bool rotationCheck = objectInRange->GetTransform()->forward().Dot(myTransform->back()) > 0.2f;
			if (!rotationCheck)
			{
				continue;
			}
		}
		else
		{
			if (auto holdableItem = objectInRange->GetComponent<HoldableItem>())
			{
				if (holdableItem->GetIsBeingTeleported()) continue;
			}
		}

		if (coolDown == false && goOutCoolDown == false)
		{
			if (RigidBody* rb = objectInRange->GetComponent<RigidBody>())
			{
				if (isPlayer)
				{
					if (objectInRange->GetComponent<Player>()->GetIsDashing())
					{
						myAnimTime = myAnimTimeDash;
					}
					if (myWaitTime < myAnimTime)
					{
						myWaitTime = myAnimTime;
					}
				}
				else
				{
					myWaitTime = myWaitTimeBase;
				}

			}
		}

		if (object)
		{
			if(!coolDown) StartCooldown();
			FinishTeleporting();
		}

		print("teleport " + objectInRange->GetName());
		StartTeleporting(objectInRange);
		break;
	}
}


void Teleporter::OnTriggerEnter(Collider* aCollider)
{
	bool addItem = false;

	if (aCollider->GetCollisionLayer() == 1 || aCollider->GetCollisionLayer() == 5)
	{
		if(HoldableItem* holdItem = aCollider->GetGameObject().GetComponent<HoldableItem>())
		{
			if(aCollider->GetTransform()->GetParent() == nullptr)
			{
				myObjectsInRange.push_back(&aCollider->GetGameObject());
			}
			else
			{
				holdItem->ResetAll();
			}
		
		}
		else
		{
			myObjectsInRange.push_back(&aCollider->GetGameObject());
		
		}

	}
}

void Teleporter::OnTriggerExit(Collider* aCollider)
{
	for (size_t i = 0; i < myObjectsInRange.size(); i++)
	{
		if (aCollider->GetGameObject().GetObjectInstanceID() == myObjectsInRange[i]->GetObjectInstanceID())
		{
			myObjectsInRange.erase(myObjectsInRange.begin() + i);
		}
	}
}

void Teleporter::RenderInProperties(std::vector<Component*>& aComponentList)
{
	ImGui::InputFloat("WaitingTime", &myWaitTimeBase);
	ImGui::InputFloat("PlayerMovingDistanceAfterTele", &myPosAddition);
	ImGui::InputFloat("Shader vortex value", &myVortexSlider);
	ImGui::InputFloat("throwingForce", &myThrowingForce);
	ImGui::InputFloat("playerForce", &myPlayerForce);
}

void Teleporter::OnObjectFinishedLoading()
{

}

void Teleporter::setOtherTelporterCoolDown()
{
	goOutCoolDown = true;
}

void Teleporter::StartTeleporting(GameObject* anObject)
{
	teleport = true;
	object = anObject;
	teleportPos = myOtherTeleporter;
	Vector3f myPos = myTransform->worldPos() + myTransform->forward() * 0.5f;
	Vector3f portalPos = myTransform->worldPos();
	Vector3f myRot = myTransform->worldRot();

	auto snurr = InstantiatePrefab("PortalSuckInSnurr");
	auto swoosh = InstantiatePrefab("TeleportFlipBookPrefab");
	auto blow = InstantiatePrefab("TeleportFlipBookPrefabCW");
	snurr->GetTransform()->SetWorldPos(Vector3f(portalPos.x, portalPos.y + 0.75f, portalPos.z - 1.f));
	snurr->GetTransform()->SetWorldRot(180.f, 0, 0);
	swoosh->GetTransform()->SetWorldPos(portalPos.x, portalPos.y + 1.0f, portalPos.z - 0.6f);
	swoosh->GetTransform()->SetWorldRot(90, 0, 0);
	blow->GetTransform()->SetWorldPos(teleportPos->worldPos().x, teleportPos->worldPos().y + 1.0f, teleportPos->worldPos().z - 0.6f);
	blow->GetTransform()->SetWorldRot(90, 0, 0);
}

void Teleporter::FinishTeleporting()
{
	timer = 0;
	coolDown = false;
	teleport = false;
	myDoAnimeOnce = true;

	Vector3f teleportationPos = teleportPos->worldPos();
	teleportationPos += teleportPos->forward() * myPosAddition;

	if (Player* player = object->GetComponent<Player>())
	{
		object->GetComponent<ModelInstance>()->SetEnabled(true);
		player->SetCanMove(true);
		player->SetFreezeInput(false);
		player->SetCanRotate(true);
		player->SetAnimationBool("Teleporting", false);
		player->SetInputFrozenTimer(0.3f);
		if (RigidBody* rb = object->GetComponent<RigidBody>())
		{
			rb->SetActorRotation(myTransform->worldRot());
			rb->SetActorPosition(teleportationPos);
			rb->AddForce({ 0,0,-1 }, myPlayerForce, physx::PxForceMode::eIMPULSE);
		}
	}
	else if (HoldableItem* item = object->GetComponent<HoldableItem>())
	{
		object->GetComponent<ModelInstance>()->SetEnabled(true);
		item->SetMyCanInteract(true);
		item->ResetTeleportTimer();
		item->ResetAll();

		if (RigidBody* rb = object->GetComponent<RigidBody>())
		{
			rb->SetActorPosition(teleportationPos);
			rb->AddForce({ 0,0,-1 }, myThrowingForce, physx::PxForceMode::eIMPULSE);
		}
		
	}

	object = nullptr;
}

void Teleporter::StartCooldown()
{
	coolDown = true;
	//posAddition
	if (Player* player = object->GetComponent<Player>())
	{
		player->SetCanMove(false);
		player->SetFreezeInput(true);
		player->SetCanRotate(false);
		player->SetAnimationBool("Teleporting", true);
		auto rb = object->GetComponent<RigidBody>();
		rb->SetActorPosition(myTransform->worldPos() + myTransform->forward());
		Vector3f teleportRot = myTransform->worldRot();
		rb->SetActorRotation(Vector3f(0, teleportRot.y - 180, 0));

		Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_TELEPORT));
	}
	else if (object->GetComponent<HoldableItem>())
	{
		//object->GetComponent<HoldableItem>()->SetMyCanInteract(false);
		object->GetComponent<ModelInstance>()->SetEnabled(false);
		if (auto rb = object->GetComponent<RigidBody>())
		{
			rb->SetVelocity({ 0,0,0 });
		}
	}
}

void Teleporter::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();

	wrapper.SaveValue<DataType::Float>("WaitingTime", myWaitTimeBase);
	wrapper.SaveValue<DataType::Float>("PlayerMovingDistanceAfterTele", myPosAddition);
	wrapper.SaveValue<DataType::Float>("playerForce", myPlayerForce);
	wrapper.SaveValue<DataType::Float>("throwingForce", myThrowingForce);
	wrapper.SaveValue<DataType::Float>("Shader vortex value", myVortexSlider);

}

void Teleporter::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("WaitingTime"))
	{
		myWaitTimeBase = aComponentData["WaitingTime"].GetFloat();
	}
	if (aComponentData.HasMember("PlayerMovingDistanceAfterTele"))
	{
		myPosAddition = aComponentData["PlayerMovingDistanceAfterTele"].GetFloat();
	}
	if (aComponentData.HasMember("throwingForce"))
	{
		myThrowingForce = aComponentData["throwingForce"].GetFloat();
	}
	if (aComponentData.HasMember("playerForce"))
	{
		myPlayerForce = aComponentData["playerForce"].GetFloat();
	}
	if (aComponentData.HasMember("Shader vortex value"))
	{
		myVortexSlider = aComponentData["Shader vortex value"].GetFloat();
	}
}
