#include "Game.pch.h"
#include "Player.h"
#include "Components\Interactables\Interactable.h"
#include "Components\Interactables/Holdables\Ingredient.h"
#include "Components\Interactables/Surfaces\IngredientContainer.h"
#include "Components\Interactables\Holdables\HoldableItem.h"
#include "Components\Physics\Collisions\Collider.h"
#include "Components\Physics\RigidBody.h"
#include "Components/Physics/Collisions/BoxCollider.h"
#include "Physics/PhysicsEngine.h"
#include "Components\Animator.h"
#include "Graphics/Animations/SkeletonData.h"
#include "..\Catbox\Audio\AudioMonoComponent.h"
#include "Components\Interactables\Holdables\Plate.h"
#include "Components/Interactables/Holdables/HoldableItem.h"
#include "Components\ParticleSystem.h"
#include "Components/Environment/MovingInteractableItems.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Model.h"
#include "Components\ParticleSystem.h"
#include "Graphics\Particles\ParticleEmitterSettings.h"
#include "Managers/StampManager.h"

Player::~Player()
{
	if (PLAYMODE)
	{
		GameManager::GetInstance()->RemovePlayer(this);
	}
}

void Player::OnTriggerEnter(Collider* aCollider)
{
	const int& id = aCollider->GetGameObject().GetObjectInstanceID();

	for (size_t i = 0; i < myInteractablesInRange.size(); i++)
	{
		if (id == myInteractablesInRange[i]->GetGameObject().GetObjectInstanceID())
		{
			printerror("whut da heeeell");
			return;
		}
	}

	if (Interactable* interactComponent = aCollider->GetGameObject().GetComponent<Interactable>())
	{
		myInteractablesInRange.push_back(interactComponent);
	}
}

void Player::OnTriggerExit(Collider* aCollider)
{
	const int& id = aCollider->GetGameObject().GetObjectInstanceID();

	for (size_t i = 0; i < myInteractablesInRange.size(); i++)
	{
		if (id == myInteractablesInRange[i]->GetGameObject().GetObjectInstanceID())
		{
			if (myInteractablesInRange[i] == mySelectedInteractable)
			{
				if (myIsPerformingAction)
				{
					mySelectedInteractable->StopPerformingActionOn(this);
				}
				mySelectedInteractable = nullptr;
			}

			myInteractablesInRange[i]->UnHighlight();
			myInteractablesInRange.erase(myInteractablesInRange.begin() + i);
			break;
		}
	}
}

void Player::OnCollisionEnter(Collider* aCollider)
{
	if (aCollider->GetCollisionLayerName() == "Player")
	{
		Player* otherPlayer = aCollider->GetGameObject().GetComponent<Player>();
		if (otherPlayer != nullptr)
		{
			if (otherPlayer->myIsDashing == true)
			{
				if (myRigidBody != nullptr)
				{
					myBounce.shouldBounce = true;
					myBounce.bounceTimer = 0.0f;
					myBounce.bounceDirection = (myTransform->worldPos() - otherPlayer->GetTransform()->worldPos()).GetNormalized() * myBounce.bounceStrength * deltaTime;

					auto collide = InstantiatePrefab("DashIntoOtherPlayerVFX");
					collide->GetTransform()->SetWorldPos((myTransform->worldPos()));

					StampManager::GetInstance()->AddStampPoints(Stamp::Annoying, 1, myPlayerId - 1);

					/*RigidBody* otherRb = otherPlayer->GetGameObject().GetComponent<RigidBody>();
					if (otherRb != nullptr)
					{
						myRigidBody->AddForce((myTransform->worldPos() - otherPlayer->GetTransform()->worldPos()).GetNormalized(), myBounce.bounceStrength, physx::PxForceMode::eIMPULSE);
					}*/
				}
			}
		}
	}
}

void Player::OnCollisionStay(Collider* aCollider)
{
	ContactReport aContactReport = aCollider->GetContactReport();
	for (int i = 0; i < aContactReport.ContactPointsNum; i++)
	{
		if (aContactReport.CollisionDirection & CD_Down)
		{
			myIsGrounded = true;
		}
		else
		{
			//print("NO GROUND");
		}
		if (aContactReport.CollisionDirection & CD_Right || aContactReport.CollisionDirection & CD_Left || aContactReport.CollisionDirection & CD_Forward || aContactReport.CollisionDirection & CD_Back)
		{
			myCollidingWithSide = true;
		}
	}

}

void Player::Awake()
{
	myAnimator = myGameObject->GetComponent<Animator>();
	myRigidBody = myGameObject->GetComponent<RigidBody>();
	myAudio = myGameObject->GetComponent<AudioMonoComponent>();

	for (auto& child : myTransform->GetChildren())
	{
		if (child->GetGameObject()->GetName() == "FootStepVFX")
		{
			myRunTrail = child->GetGameObject()->GetComponent<ParticleSystem>();
			myRunTrail->Pause();
		}
	}

	auto sk = myAnimator->GetSkeleton();
	if (auto deliverAnim = AssetRegistry::GetInstance()->GetAsset<Animation>("player2_deliver_anim"))
	{
		myDeliverAnimationFrameCount = static_cast<int>(deliverAnim->GetFrames().size());
	}

	for (auto& [boneName, id] : sk->sharedData->boneNameToIndex)
	{
		if (boneName.find("holdableItem") != std::string::npos)
		{
			myHoldableItemBone = &sk->boneTransforms[id];
			myHoldableItemBoneId = id;
		}
	}

	//
	if (myRigidBody != nullptr)
	{
		if (myRigidBody->GetActor() != nullptr)
		{
			Engine::GetInstance()->GetPhysicsEngine()->CreateAdditionalTriggerBox({ 0.8f, 3.0f, 0.8f }, myTransform->forward() * 0.15f, myRigidBody->GetActor(), 2); //PlayerInteractId
		}
		else
		{
			printerror("Make sure player component loads in after rigidbody component.");
		}
	}
	//
	else
	{
		printerror("No rigidbody on player.");
	}

	if (myRigidBody != nullptr)
	{
		myRigidBody->PutToSleep();
	}
}

void Player::Update()
{
	if (myHasSetupPlayer == false)
	{
		SetupPlayer();
	}

	SetAnimationBool("Throwing", false);

	if (myIsPerformingAction)
	{
		if (mySelectedInteractable)
		{
			mySelectedInteractable->PerformActionOn(this);
			myDirection = { 0.0f, 0.0f, 0.0f };
			return;
		}
		else myIsPerformingAction = false;
	}

	DetermineClosestInteractable();

	if (myGravityGraceTimer <= 0)
	{
		myRigidBody->WakeUp();
	}
	else
	{
		myGravityGraceTimer -= deltaTime;
		return;
	}

	if (myInputFrozenTimer > 0)
	{
		myInputFrozenTimer -= deltaTime;
		return;
	}

	myDirection = { 0.0f, 0.0f, 0.0f };
	if (myFreezeAllInput == false)
	{
		UpdateInput(myDirection);

		if (myInputs.pickup)
		{
			Pickup();
		}

		if (myInputs.interact && mySelectedInteractable != nullptr && myHeldInteractable == nullptr)
		{
			Interact();
		}
		else if (myIsPerformingAction && mySelectedInteractable)
		{
			mySelectedInteractable->StopPerformingActionOn(this);
			myIsPerformingAction = false;
			myCanThrow = false;
		}

		if (myInputs.interact && myHeldInteractable != nullptr && myHeldInteractable->GetIsThrowable())
		{
			myIsThrowing = true;
		}

		if (myInputs.interactHeld && myHeldInteractable != nullptr && myHeldInteractable->GetIsThrowable() && myIsThrowing == true)
		{
			Aim(true);
		}
		else
		{
			Aim(false);
		}

		if (myInputs.interactReleased && myIsThrowing == true)
		{
			if (myCanThrow)
			{
				Throw();
			}
			else
			{
				myCanThrow = true;
			}
		}

		if (myCanMove && myDashTimer > myDashDuration + myDashCooldown)
		{
			if (myInputs.dash && !myIsDashing)
			{
				Dash();
			}
		}

		if (myInputs.emote)
		{
			Emote();
		}
	}

	myDirection.Normalize();

	if (mySelectedInteractable != nullptr && mySelectedInteractable->GetIsBeingThrown() == true && mySelectedInteractable->GetPlayerThrowing() != this)
	{
		Catch();
	}

	UpdateRespawn();

	//Deliver plate
	if (myDeliverTimer > 0)
	{
		myDeliverTimer -= deltaTime;
		if (myDeliverTimer <= 0)
		{
			auto orderResult = OrderHandler::GetInstance()->DeliverOrder(dynamic_cast<Plate*>(myHeldInteractable));
			if (orderResult.tips > 0)
			{
				auto vfx = InstantiatePrefab("OrderDeliveryVFX");
				vfx->GetTransform()->SetWorldPos(myHeldInteractable->GetTransform()->worldPos());
				vfx->GetComponent<ParticleSystem>()->GetEmitters()[0].GetSharedData()->GetParticlesPerEmission().SetDefaultValue(orderResult.tips);
			}

			OrderHandler::GetInstance()->AddPlateToTransit(dynamic_cast<Plate*>(myHeldInteractable));
			SetHeldItem(nullptr);
			myFreezeAllInput = false;
			SetAnimationBool("PlaceItem", false);
		}
	}
}

void Player::FixedUpdate()
{
	myDashTimer += fixedDeltaTime;
	myBounce.bounceTimer += fixedDeltaTime;

	if (myDashTimer > myDashDuration)
	{
		myIsDashing = false;
		SetAnimationBool("IsDashing", false);
	}

	if (myBounce.bounceTimer > myBounce.bounceDuration)
	{
		myBounce.bounceVelocity = { 0.0f, 0.0f, 0.0f };
		myBounce.bounceDirection = { 0.0f, 0.0f, 0.0f };
		myBounce.shouldBounce = false;
	}

	if (myHatModel && myRigidBody)
	{

		Vector3f actualVel = myRigidBody->GetVelocity();
		float Velocity = Vector3f(actualVel.x, 0.f, actualVel.z).Length();

		if (Velocity > 1.0f)
		{
			myThrottle += 20.f * deltaTime;
			if (myThrottle > 5)
			{
				myThrottle = 5;
			}
		}
		else if (Velocity < 1.0f)
		{
			myThrottle -= 10.f * deltaTime;

			if (myThrottle < 1)
			{
				myThrottle = 1;
			}
		}

		myHatModel->GetGameObject().SetCustomFloat(myThrottle);
	}


	if (!myIsGrounded && myCollidingWithSide)
	{
		myCanMove = false;
	}
	else
	{
		myCanMove = true;
	}

	Move(myDirection);

	myIsGrounded = false;
	myCollidingWithSide = false;
}

void Player::RenderInProperties(std::vector<Component*>& aComponentList)
{
	ImGui::InputFloat("Speed##Player", &mySpeed);
	ImGui::InputFloat("Rotation Speed##Player", &myRotationSpeed);
	ImGui::InputFloat("Controller Deadlock##Player", &myControllerDeadlock);
	ImGui::InputFloat3("Held offset##Player", &myHeldInteractableOffset.x);

	ImGui::Text("Dashing");
	ImGui::InputFloat("Speed##Dash", &myDashSpeed);
	ImGui::InputFloat("Duration##Dash", &myDashDuration);
	ImGui::InputFloat("Cooldown##Dash", &myDashCooldown);

	ImGui::Text("Throwing");
	ImGui::InputFloat("Strength##Throwing", &myThrowingStrength);
	ImGui::InputFloat("Arc##Throwing", &myThrowingArc);

	ImGui::Text("Bouncing");
	ImGui::InputFloat("Strength##Bounce", &myBounce.bounceStrength);
	ImGui::InputFloat("Duration##Bounce", &myBounce.bounceDuration);
}

void Player::Save(rapidjson::Value& aComponentData)
{
	int version = 4;
	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Int>("Version", version);

	wrapper.SaveValue<DataType::Float>("Speed", mySpeed);
	wrapper.SaveValue<DataType::Float>("DashSpeed", myDashSpeed);
	wrapper.SaveValue<DataType::Float>("DashDuration", myDashDuration);
	wrapper.SaveValue<DataType::Float>("DashCooldown", myDashCooldown);
	wrapper.SaveValue<DataType::Float>("ThrowingStrength", myThrowingStrength);
	wrapper.SaveValue<DataType::Float>("ControllerDeadlock", myControllerDeadlock);
	wrapper.SaveValue<DataType::Float>("RotationSpeed", myRotationSpeed);
	wrapper.SaveValue<DataType::Float>("BounceStrength", myBounce.bounceStrength);
	wrapper.SaveValue<DataType::Float>("BounceDuration", myBounce.bounceDuration);
	wrapper.SaveValue<DataType::Float>("ThrowingArc", myThrowingArc);
	wrapper.SaveValue<DataType::Float>("HeldOffsetX", myHeldInteractableOffset.x);
	wrapper.SaveValue<DataType::Float>("HeldOffsetY", myHeldInteractableOffset.y);
	wrapper.SaveValue<DataType::Float>("HeldOffsetZ", myHeldInteractableOffset.z);
}

void Player::Load(rapidjson::Value& aComponentData)
{
	int version = aComponentData["Version"].GetInt();

	if (version >= 1)
	{
		mySpeed = aComponentData["Speed"].GetFloat();
		myDashSpeed = aComponentData["DashSpeed"].GetFloat();
		myDashDuration = aComponentData["DashDuration"].GetFloat();
		myDashCooldown = aComponentData["DashCooldown"].GetFloat();
		myThrowingStrength = aComponentData["ThrowingStrength"].GetFloat();
		myControllerDeadlock = aComponentData["ControllerDeadlock"].GetFloat();
	}
	if (version >= 2)
	{
		myRotationSpeed = aComponentData["RotationSpeed"].GetFloat();
	}
	if (version >= 3)
	{
		myBounce.bounceStrength = aComponentData["BounceStrength"].GetFloat();
		myBounce.bounceDuration = aComponentData["BounceDuration"].GetFloat();
	}
	if (version >= 4)
	{
		myThrowingArc = aComponentData["ThrowingArc"].GetFloat();
		myHeldInteractableOffset.x = aComponentData["HeldOffsetX"].GetFloat();
		myHeldInteractableOffset.y = aComponentData["HeldOffsetY"].GetFloat();
		myHeldInteractableOffset.z = aComponentData["HeldOffsetZ"].GetFloat();
	}
}

void Player::SetCanMove(bool aCanMove)
{
	myCanMove = aCanMove;
}

void Player::SetCanRotate(bool aCanRotate)
{
	myCanRotate = aCanRotate;
}

void Player::SetFreezeInput(bool aFreezeInput)
{
	myFreezeAllInput = aFreezeInput;
}

void Player::SetAdditionalVelocity(Vector3f aVelocity)
{
	myAdditionalVelocity = aVelocity;
}

void Player::SetHeldItem(HoldableItem* anInteractable)
{
	if (myHeldInteractable)
	{
		myHeldInteractable->SetIsBeingHeld(false);
		myHeldInteractable->StopBeingHeld(this);
		myHeldInteractable->SetEnabled(true);
		if (myHeldInteractable->GetTransform()->GetRootParent() == myTransform)
		{
			myHeldInteractable->GetTransform()->SetParent(nullptr);
		}
	}

	////Place item
	//if (myHeldInteractable && !anInteractable && myAnimator)
	//{
	//	myAnimator->SetBool("PlaceItem", true);
	//}

	myHeldInteractable = anInteractable;
	myAnimator->SetActiveLayer("Default", !myHeldInteractable);
	myAnimator->SetActiveLayer("Holding", myHeldInteractable);

	if (myHeldInteractable)
	{
		myHeldInteractable->SetIsBeingHeld(true);
		myHeldInteractable->OnPickup();
		myHeldInteractable->SetEnabled(false);

		if (myHoldableItemBone)
		{
			myHeldInteractable->GetTransform()->ParentToBone(myHoldableItemBone, myHoldableItemBoneId);
		}
		else
		{
			myHeldInteractable->GetTransform()->SetParent(myTransform);
			myHeldInteractable->GetTransform()->SetWorldPos(myTransform->worldPos() + myTransform->up() * myHeldInteractableOffset.y + myTransform->forward() * myHeldInteractableOffset.z);
		}

		myHeldInteractable->UnHighlightInstant();
	}
}

void Player::DeliverItem()
{
	if (myDeliverTimer > 0) return;
	myFreezeAllInput = true;
	myDeliverTimer = myDeliverAnimationFrameCount / 24.f;
	myAnimator->SetActiveLayer("Default", true);
	myAnimator->SetActiveLayer("Holding", false);
	SetAnimationBool("PlaceItem", true);
}

void Player::SetAnimationBool(const std::string& aVariableName, bool aValue)
{
	if (!myAnimator) return;
	myAnimator->SetBool(aVariableName, aValue);
}

void Player::FinishAction()
{
	myIsPerformingAction = false;
}

void Player::RemoveInteractable(Interactable* anInteractable)
{
	for (size_t i = 0; i < myInteractablesInRange.size(); i++)
	{
		if (myInteractablesInRange[i] == anInteractable)
		{

			if (mySelectedInteractable == anInteractable)
			{
				mySelectedInteractable = nullptr;
			}

			myInteractablesInRange.erase(myInteractablesInRange.begin() + i);
			return;
		}
	}

}

bool Player::GetIsDashing()
{
	return myIsDashing;
}

void Player::UpdateInput(Vector3f& aDirection)
{
	bool controllerActive = myController && myController->IsConnected();
	if (controllerActive)
	{
		myInputs.pickup = myController->WasPressedThisFrame(myControls.xboxPickup);
		myInputs.interact = myController->WasPressedThisFrame(myControls.xboxInteract);
		myInputs.interactHeld = myController->IsPressed(myControls.xboxInteract);
		myInputs.interactReleased = myController->WasReleasedThisFrame(myControls.xboxInteract);
		myInputs.dash = myController->WasPressedThisFrame(myControls.xboxDash);
		myInputs.emote = myController->WasPressedThisFrame(myControls.xboxEmote);

		if (myController->GetLeftStickAxis().Length() > myControllerDeadlock)
		{
			aDirection.x = myController->GetLeftStickAxis().x;
			aDirection.z = myController->GetLeftStickAxis().y;
		}
	}
	else
	{
		myInputs.pickup = Input::GetKeyPress(myControls.pickup);
		myInputs.interact = Input::GetKeyPress(myControls.interact);
		myInputs.interactHeld = Input::GetKeyHeld(myControls.interact);
		myInputs.interactReleased = Input::GetKeyReleased(myControls.interact);
		myInputs.dash = Input::GetKeyPress(myControls.dash);
		myInputs.emote = Input::GetKeyPress(myControls.emote);

		aDirection.z += static_cast<float>(Input::GetKeyHeld(myControls.up));
		aDirection.x += static_cast<float>(Input::GetKeyHeld(myControls.right));
		aDirection.z -= static_cast<float>(Input::GetKeyHeld(myControls.down));
		aDirection.x -= static_cast<float>(Input::GetKeyHeld(myControls.left));
	}
}

void Player::Move(Vector3f& aDirection)
{
	float speed = mySpeed * fixedDeltaTime;

	Vector3f velocity = { 0.0f, myRigidBody->GetVelocity().y, 0.0f };

	if (velocity.y > 0)
	{
		velocity.y = 0;
	}

	if (myRigidBody != nullptr)
	{
		if (myIsDashing == false)
		{
			if (aDirection.x != 0.0f || aDirection.z != 0.0f)
			{
				if (myCanRotate)
				{
					SlowRotate(myRigidBody, aDirection);
				}

				if (myCanMove && !myIsAiming)
				{
					myRigidBody->AddForce(aDirection, speed, physx::PxForceMode::eIMPULSE);
					if (velocity.x > speed)
					{
						velocity.x = speed;
					}
					else if (velocity.x < -speed)
					{
						velocity.x = -speed;
					}

					if (velocity.z > speed)
					{
						velocity.z = speed;
					}
					else if (velocity.z < -speed)
					{
						velocity.z = -speed;
					}
				}

				if (myAnimator)
				{
					myAnimator->SetFloat("Velocity", speed);
				}
				if (myHeldInteractable)
				{
					myHeldInteractable->SetFoodTrailState(true);
				}
				if (myRunTrail)
				{
					myRunTrail->Play();
				}
			}
			else
			{
				if (myAnimator)
				{
					myAnimator->SetFloat("Velocity", 0.0f);
				}
				if (myHeldInteractable)
				{
					myHeldInteractable->SetFoodTrailState(false);
				}
				if (myRunTrail)
				{
					myRunTrail->Pause();
				}
			}


			if (myBounce.shouldBounce == true && myCanMove == true)
			{
				myBounce.bounceVelocity = myBounce.bounceDirection * myBounce.bounceStrength * fixedDeltaTime * (1.0f - (myBounce.bounceTimer / myBounce.bounceDuration));
				myRigidBody->SetVelocity(velocity + myAdditionalVelocity + Vector3f{ myBounce.bounceVelocity.x, 0.0f, myBounce.bounceVelocity.z });
			}
			else
			{
				myRigidBody->SetVelocity(velocity + myAdditionalVelocity);
			}
		}
		else
		{
			if (myCanMove == true)
			{
				if (aDirection.x != 0.0f || aDirection.z != 0.0f)
				{
					myRigidBody->AddForce(aDirection, myDashSpeed * fixedDeltaTime * (1.0f - (myDashTimer / myDashDuration)), physx::PxForceMode::eIMPULSE);
					if (myCanRotate)
					{
						SlowRotate(myRigidBody, aDirection);
					}
				}
				else
				{
					myRigidBody->AddForce(myTransform->forward(), myDashSpeed * fixedDeltaTime * (1.0f - (myDashTimer / myDashDuration)), physx::PxForceMode::eIMPULSE);
				}
			}
			myRigidBody->SetVelocity(velocity + myAdditionalVelocity);
		}
	}
}

void Player::Pickup()
{
	if (mySelectedInteractable != nullptr)
	{
		if (myHeldInteractable != nullptr)
		{
			// Dropping item

			mySelectedInteractable->InteractWithHeldItem(this, myHeldInteractable);

			if (myAudio)
			{
				myAudio->PlaySoundEffect(1); // Place on counter sound
			}
		}
		else
		{
			if (mySelectedInteractable->GetIsBeingHeld() == false)
			{
				RigidBody* pickupRB = mySelectedInteractable->GetGameObject().GetComponent<RigidBody>();


				if (myAudio && mySelectedInteractable->HasItem() == true)
				{
					myAudio->PlaySoundEffect(2); // Pick up sound
				}

				if (pickupRB != nullptr && pickupRB->GetGameObject().GetComponent<HoldableItem>())
				{
					if (mySelectedInteractable->GetGameObject().GetComponent<MovingInteractableItems>())
					{
						pickupRB->SetLayer(5);
					}
					//does this run from counter

					if (myAudio)
					{
						myAudio->PlaySoundEffect(2); // Pick up sound
					}

					pickupRB->SetActorPosition({ 0.0f, -1000.0f, 0.0f });


					mySelectedInteractable->GetGameObject().RemoveComponent(pickupRB);

					mySelectedInteractable->GetGameObject().GetTransform()->SetWorldPos(myTransform->worldPos() + myTransform->up() * myHeldInteractableOffset.y + myTransform->forward() * myHeldInteractableOffset.z);
				}


				mySelectedInteractable->SetIsBeingThrown(false);
				mySelectedInteractable->SetPlayerThrowing(nullptr);
				mySelectedInteractable->Interact(this);
			}
		}
	}
	else
	{
		if (myHeldInteractable != nullptr)
		{
			RigidBody* droppedItemRB = myHeldInteractable->GetGameObject().AddComponent<RigidBody>();

			if (droppedItemRB)
			{
				myHeldInteractable->SetMyCanInteract(true);
				myHeldInteractable->SetIsBeingHeld(false);

				HoldableItem* heldItem = dynamic_cast<HoldableItem*>(myHeldInteractable);
				if (heldItem != nullptr)
				{
					droppedItemRB->ChangeMass(heldItem->GetMass());
					if (heldItem->GetMass() != 1.0f)
					{
						droppedItemRB->SetRotationAxisLock(true, false, true);
					}
				}

				droppedItemRB->SetActorPosition(myTransform->worldPos() + myTransform->up() * myHeldInteractableOffset.y + myTransform->forward() * myHeldInteractableOffset.z);
				SetHeldItem(nullptr);
			}
			else
			{
				printerror("Droppable does not have a rigidbody.");
			}
		}
		else if (mySelectedInteractable)
		{

			mySelectedInteractable->PerformActionOn(this);
		}
	}

	myIsThrowing = false;
}

void Player::Interact()
{
	if (!myIsPerformingAction)
	{
		if (mySelectedInteractable->GetIsBeingUsed()) return;
		myIsPerformingAction = true;
		mySelectedInteractable->BeginPerformingActionOn(this);
	}
	else mySelectedInteractable->PerformActionOn(this);
}

void Player::Dash()
{
	if (myAudio)
	{
		myAudio->PlaySoundEffect(0); // Dash sound
	}

	myDashTimer = 0.0f;
	myIsDashing = true;
	myAnimator->SetBool("IsDashing", true);
}

void Player::Aim(bool anIsAiming)
{
	myIsAiming = anIsAiming;
}

void Player::Throw()
{
	if (myHeldInteractable)
	{
		if (myHeldInteractable->GetIsThrowable())
		{
			RigidBody* throwableRB = myHeldInteractable->GetGameObject().AddComponent<RigidBody>();

			if (throwableRB)
			{
				Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_THROW));

				SetAnimationBool("Throwing", true);
				myHeldInteractable->SetIsBeingThrown(true);
				myHeldInteractable->SetPlayerThrowing(this);
				myHeldInteractable->SetPlayerForward(myGameObject->GetTransform()->forward());

				throwableRB->SetActorPosition(myTransform->worldPos() + myTransform->up() * myHeldInteractableOffset.y + myTransform->forward() * myHeldInteractableOffset.z);
				Vector3f forwardArc = (myTransform->forward() + Vector3f{ 0.0f, myThrowingArc, 0.0f }).GetNormalized();
				throwableRB->AddForce(forwardArc, myThrowingStrength, physx::PxForceMode::eIMPULSE);
				myHeldInteractable->ResetTriggerStillActive();
				Engine::GetInstance()->GetPhysicsEngine()->CreateAdditionalTriggerBox({ 0.55f, 0.55f, 0.55f }, { 0,0,0 }, throwableRB->GetActor(), 6); //Add Trigger for holditem

				SetHeldItem(nullptr);
			}
			else
			{
				printerror("Throwable does not have a rigidbody.");
			}

			myIsThrowing == false;
		}
	}
}

void Player::Emote()
{

}

void Player::Catch()
{
	HoldableItem* thrownItem = dynamic_cast<HoldableItem*>(mySelectedInteractable);

	if (myHeldInteractable == nullptr)
	{
		if (mySelectedInteractable->GetIsBeingHeld() == false)
		{
			if (myPlayerId == 1)
			{
				StampManager::GetInstance()->AddStampPoints(Stamp::TrickShotter, 1, 2 - 1);
			}
			else
			{
				StampManager::GetInstance()->AddStampPoints(Stamp::TrickShotter, 1, 1 - 1);
			}
			RigidBody* thrownRB = mySelectedInteractable->GetGameObject().GetComponent<RigidBody>();
			thrownRB->SetVelocity({ 0.0f, 0.0f, 0.0f });

			thrownItem->ResetCatch();

			for (size_t i = 0; i < myInteractablesInRange.size(); i++)
			{
				if (myInteractablesInRange[i] == mySelectedInteractable)
				{
					myInteractablesInRange.erase(myInteractablesInRange.begin() + i);
					break;
				}
			}

			mySelectedInteractable->SetIsBeingThrown(false);
			mySelectedInteractable->SetPlayerThrowing(nullptr);

			mySelectedInteractable->Interact(this);
		}
	}
}

void Player::UpdateRespawn()
{
	if (myDrownTimer > 0)
	{
		myDrownTimer -= deltaTime;
		if (myDrownTimer <= 0)
		{
			SetFreezeInput(false);
			myRigidBody->SetActorPosition(mySpawnPoint);
			myAnimator->SetBool("IsDead", false);
			myRigidBody->ChangeGravityScale(1);
			auto smoke = InstantiatePrefab("RespawnSmokePoof");
			smoke->GetTransform()->SetWorldPos(mySpawnPoint);
		}
	}
	else if (myRigidBody->GetActorPosition().y < -5.2f)
	{
		if (myHeldInteractable)
		{
			Pickup();
		}

		StampManager::GetInstance()->AddStampPoints(Stamp::Clumsy, 1, myPlayerId - 1);
		SetFreezeInput(true);
		SetAnimationBool("IsDead", true);
		myDrownTimer = 2;
		myRigidBody->ChangeGravityScale(0.0f);
		myRigidBody->SetVelocity({ 0.0f, 0.f, 0.0f });
	}
}

void Player::SetupHat()
{
	int hatID = 0;
	if (myPlayerId == 1)
	{
		hatID = GameManager::GetInstance()->GetPlayer1Data()->GetHatID();
	}
	else if (myPlayerId == 2)
	{
		hatID = GameManager::GetInstance()->GetPlayer2Data()->GetHatID();
	}

	myHatModel = nullptr;
	if (hatID == -1)
		return;
	if (myGameObject->GetTransform()->GetChildren()[0] != nullptr)
	{
		for (Transform* child : myGameObject->GetTransform()->GetChildren())
		{
			if (child->GetGameObject()->GetComponent<ModelInstance>())
			{
				myHatModel = child->GetGameObject()->GetComponent<ModelInstance>();
			}
		}

		if (myHatModel != nullptr)
		{
			const std::string& hatName = GameManager::GetInstance()->GetMyHatsVector()[hatID];
			const string& matName = hatName.substr(0, hatName.size() - 4) + "mat";
			myHatModel->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>(hatName));
			myHatModel->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>(matName), 0);
			myHatModel->SetRenderToUse(Renderer::Forward);
		}
		{
			printerror("Childed gameobject to player does not have a modelinstance");
		}
	}
	else
	{
		printerror("No model hat childed to player");
	}
}

void Player::SetupPlayer()
{
	if (myPlayerId == -1)
	{
		myPlayerId = GameManager::GetInstance()->AddPlayer(this);
	}

	myController = GameManager::GetInstance()->GetPlayerController(myPlayerId);
	myControls = GameManager::GetInstance()->GetPlayerControls(myPlayerId);

	myRigidBody != nullptr ? mySpawnPoint = myRigidBody->GetActorPosition() : mySpawnPoint = { 0.0f, 0.5f, 0.0f };

	SetupHat();

	myHasSetupPlayer = true;
}

void Player::SlowRotate(RigidBody* aRigidBody, Vector3f& aDirection)
{
	//if (aDirection.x == 0.0f && aDirection.z == 0.0f) { return; } // Optimization, might be redundant.
	Vector2f direction = { aDirection.x, aDirection.z };
	Vector2f forward = { myTransform->forward().x, myTransform->forward().z };
	Vector2f normalVector = { -aDirection.z, aDirection.x };

	forward.Normalize();

	float dot = direction.Dot(forward);
	if (dot > 1.0f) dot = 1.0f;
	if (dot < -1.0f) dot = -1.0f;
	float rotationValue = Catbox::Rad2Deg(acos(dot));

	if (myRotationSpeed * fixedDeltaTime < rotationValue)
	{
		rotationValue = myRotationSpeed * fixedDeltaTime;
	}

	if (direction.Dot(forward) < 0.0f)
	{
		if (normalVector.Dot(forward) > 0.0f)
		{
			aRigidBody->Rotate({ 0.0f, -rotationValue, 0.0f });
		}
		else
		{
			aRigidBody->Rotate({ 0.0f, rotationValue, 0.0f });
		}
	}
	else
	{
		if (normalVector.Dot(forward) < 0.0f)
		{
			aRigidBody->Rotate({ 0.0f, rotationValue, 0.0f });
		}
		else
		{
			aRigidBody->Rotate({ 0.0f, -rotationValue, 0.0f });
		}
	}
}

void Player::DetermineClosestInteractable()
{
	float closestDist = FLT_MAX;
	const Vector3f playerPos = myTransform->worldPos();
	const Vector3f playerForward = myTransform->forward();

	float bestDot = 0.2f;
	float bestDist = 4;
	int bestPrio = 0;

	Interactable* closestInteractable = nullptr;

	for (int i = myInteractablesInRange.size() - 1; i >= 0; i--)
	{
		HoldableItem* holdable = dynamic_cast<HoldableItem*>(myInteractablesInRange[i]);
		if (holdable && !holdable->GetGameObject().GetComponent<RigidBody>())
		{
			myInteractablesInRange.erase(myInteractablesInRange.begin() + i);
			continue;
		}
	}

	for (size_t i = 0; i < myInteractablesInRange.size(); i++)
	{
		if (!myInteractablesInRange[i]->IsEnabled() || !myInteractablesInRange[i]->GetCanInteract()) continue;
		Vector3f dirToInteractable = myInteractablesInRange[i]->GetTransform()->worldPos() - playerPos;
		float distSqr = dirToInteractable.LengthSqr();
		if (distSqr > 4)
		{
			continue;
		}

		dirToInteractable.y = 0;
		dirToInteractable.Normalize();
		float dot = dirToInteractable.Dot(playerForward);
		int prio = myInteractablesInRange[i]->GetInteractPriority();

		if (prio >= bestPrio && dot > 0.2f)
		{
			if (distSqr < bestDist)
			{
				bestDist = distSqr;
			}
			else
			{
				if (dot - bestDot > 0.5f)
				{
					bestDist = 0;
				}
				else if (prio == bestPrio) continue;
			}

			if (dot > bestDot)
			{
				bestDot = dot;
			}

			closestInteractable = myInteractablesInRange[i];
			bestPrio = prio;
		}
	}


	if (closestInteractable != mySelectedInteractable)
	{
		if (mySelectedInteractable)
		{
			mySelectedInteractable->UnHighlight();
		}

		mySelectedInteractable = closestInteractable;


		if (mySelectedInteractable)
		{
			mySelectedInteractable->Highlight();
		}
	}
}
