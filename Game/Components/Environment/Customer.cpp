#include "Game.pch.h"
#include "Customer.h"
#include "Physics/PhysicsEngine.h"
#include "Components\Physics\Collisions\Collider.h"
#include "Assets\AnimationController.h"
#include "Components\Animator.h"
#include "Components\3D\ModelInstance.h"
#include "Assets\Model.h"

void Customer::Awake()
{
	myAnimator = myGameObject->GetComponent<Animator>();

	if (myGameObject->GetComponent<RigidBody>())
	{
		myRigidBody = myGameObject->GetComponent<RigidBody>();
		myRigidBody->SetKinematic();
		myCastedActor = static_cast<physx::PxRigidDynamic*>(myRigidBody->GetActor());
		myRigidBody->SetActorPosition({ 0,-10000,0 });
	}

	GenerateAppearance();
}

void Customer::Update()
{
	if (myStunTimer > 0)
	{
		myStunTimer -= deltaTime;
		myAnimator->SetBool("Stun", false);
	}
}

void Customer::GenerateAppearance()
{
	myType = maybe;

	auto model = myGameObject->GetComponent<ModelInstance>();
	auto anim = myGameObject->GetComponent<Animator>();

	if (myType == 1)
	{
		int randMaterial = Catbox::GetRandom(1, 3);
		std::string matName = "p1_customer" + std::to_string(randMaterial) + "_mat";
		model->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>(matName), 0, true);
		model->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>("player1_sk"));
		anim->SetController(AssetRegistry::GetInstance()->GetAsset<AnimationController>("Customer2Animation"));
	}
	else
	{
		int randMaterial = Catbox::GetRandom(1, 5);
		std::string matName = "customer" + std::to_string(randMaterial) + "_mat";
		model->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>(matName), 0, true);
	}
}

void Customer::OnCollisionEnter(Collider* aCollider)
{
	if (!myReachedTarget) return;
	if (aCollider->GetCollisionLayerName() == "Ingredient")
	{
		Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_BUMP));
		aCollider->GetGameObject().Destroy();
		myStunTimer = 2;
		myAnimator->SetBool("Stun", true);
		myAnimator->SetInt("FallIndex", maybe);
		myAnimator->SetBool("Walking", false);
	}
}