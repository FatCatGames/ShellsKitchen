#include "Game.pch.h"
#include "Skillet.h"
#include "Ingredient.h"
#include "Components\ParticleSystem.h"
#include "Plate.h"
#include "Components\ProgressBar.h"
#include "Components\GamePlay\BurnIndicator.h"
#include "Audio\AudioMonoComponent.h"
#include "Components\Interactables\Surfaces\Stove.h"
#include "Components\3D\ModelInstance.h"
#include "Components\BobSpinComponent.h"
#include "Assets\Model.h"

void Skillet::Awake()
{
	HoldableItem::Awake();
	myAudioComponent = myGameObject->GetComponent<AudioMonoComponent>();
	myBurnIndicator = myGameObject->GetComponent<BurnIndicator>();

	for (auto& child : myTransform->GetChildren())
	{
		if (child->GetGameObject()->GetName() == "Steam")
		{
			mySteam = child->GetGameObject()->GetComponent<ParticleSystem>();
			mySteam->Pause();
		}
		else if (child->GetGameObject()->GetName() == "SteamTrail")
		{
			mySteamTrail = child->GetGameObject()->GetComponent<ParticleSystem>();
			mySteamTrail->Pause();
		}
		else if (child->GetGameObject()->GetName() == "Smoke")
		{
			mySmoke = child->GetGameObject()->GetComponent<ParticleSystem>();
			mySmoke->Pause();
		}
		else if (child->GetGameObject()->GetName() == "CookTimer")
		{
			myTimer = child->GetGameObject()->GetComponent<ProgressBar>();
		}
	}
}

void Skillet::Update()
{
	HoldableItem::Update();

	//Respawn
	if (myTransform->worldPos().y < -5.5f)
	{
		auto stove = GameManager::GetInstance()->GetCurrentLevelManager().GetAvailableStove();
		if (stove)
		{
			auto rb = myGameObject->GetComponent<RigidBody>();
			if (rb)
			{
				myGameObject->RemoveComponent(rb);
				stove->AddSkillet(this);
				auto smoke = InstantiatePrefab("RespawnSmokePoof");
				smoke->GetTransform()->SetWorldPos(myTransform->worldPos());
			}
		}
	}
}


void Skillet::Cook()
{
	if (myFood)
	{
		if (!myIsCooking)
		{
			StartCooking();
		}

		//Returns true if state of food has changed
		bool stateChange = false;
		if (GetIsPot()) stateChange |= myFood->Boil();
		else stateChange |= myFood->Cook();
		if (stateChange)
		{
			if (myFood->GetIsBurnt())
			{
				mySteamTrail->GetGameObject().SetActive(false);
				mySteam->GetGameObject().SetActive(false);
				mySmoke->Replay();
				mySmoke->GetGameObject().SetActive(true);
			}
			else
			{
				myFood->GetTransform()->SetWorldPos(myTransform->worldPos());

				myTimer->GetGameObject().SetActive(false);
				mySteam->GetGameObject().SetActive(false);
				mySteamTrail->Replay();
				mySteamTrail->GetGameObject().SetActive(true);

				if (auto bob = myGameObject->GetComponent<BobSpinComponent>())
				{
					bob->SetShouldBob(false);
				}
			}
		}

		if (myFood->GetIsBurnt())
		{
			myTimer->SetProgress(myFood->GetBurnPercent());
			if (myAudioComponent)
			{
				myAudioComponent->StopSoundEffect(0); // Frying
				myAudioComponent->StopSoundEffect(1); // Boiling
				myAudioComponent->StopSoundEffect(2); // Beeping
				myIsPlayingWarningSound = false;
			}
			if (myBurnIndicator) myBurnIndicator->Stop();
		}
		else
		{
			myTimer->SetProgress(myFood->GetCookPercent());
		}

		//print(std::to_string(myFood->GetBurnPercent()));
		if (!myFood->GetIsBurnt() && myFood->GetBurnPercent() > 0.3f)
		{
			if (myIsPlayingWarningSound == false)
			{
				myIsPlayingWarningSound = true;
				if (myAudioComponent) myAudioComponent->PlaySoundEffect(2); // Beeping
				if (myBurnIndicator) myBurnIndicator->Start();
				myBurnTimer = 0.5f;
			}
			else
			{
				myBurnTimer -= deltaTime;
				if (myBurnTimer < 0)
				{
					if (myBurnIndicator) myBurnIndicator->Switch();
					myBurnTimer = 0.5f * (1 - myFood->GetBurnPercent());
					//print("My new time = " + std::to_string(myBurnTimer));
					if (myBurnTimer < 0.1f) myBurnTimer = 0.1f;
				}
			}
		}
	}
}

void Skillet::StartCooking()
{
	if (myFood->GetIsBurnt()) return;
	myFood->GetTransform()->SetLocalRot(0, 0, 0);
	myFood->GetTransform()->SetLocalPos(0, 0, 0);

	if (auto bob = myGameObject->GetComponent<BobSpinComponent>())
	{
		bob->SetShouldBob(true);
	}

	if (myGameObject->GetComponent<AudioMonoComponent>())
	{
		if (GetIsPot() && !myFood->GetIsCooked() && !myFood->GetIsBurnt())
		{
			myFood->GetTransform()->SetLocalPos(0, 0.08f, 0);
			myGameObject->GetComponent<AudioMonoComponent>()->PlaySoundEffect(1); // Boiling
			myFood->SetBoilingMesh();
		}
		else
		{
			myGameObject->GetComponent<AudioMonoComponent>()->PlaySoundEffect(0); // Frying
		}
	}

	myIsCooking = true;
	if (!GetIsPot() && !myFood->GetIsCooked() || GetIsPot() && !myFood->GetIsBoiled())
	{
		mySteam->Replay();
		mySteam->GetGameObject().SetActive(true);
		myTimer->GetGameObject().SetActive(true);
	}
	else
	{
		mySteam->GetGameObject().SetActive(false);
		myTimer->GetGameObject().SetActive(false);
	}
}

void Skillet::StopCooking()
{
	if (auto bob = myGameObject->GetComponent<BobSpinComponent>())
	{
		bob->SetShouldBob(false);
	}

	if (myAudioComponent)
	{
		myAudioComponent->StopSoundEffect(0); // Frying
		myAudioComponent->StopSoundEffect(1); // Boiling
		myAudioComponent->StopSoundEffect(2); // Beeping
		myIsPlayingWarningSound = false;
	}
	if (myBurnIndicator) myBurnIndicator->Stop();

	myIsCooking = false;
	mySteam->GetGameObject().SetActive(false);
	myTimer->GetGameObject().SetActive(false);
}

void Skillet::InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem)
{
	if (Ingredient* ingredient = dynamic_cast<Ingredient*>(anitem))
	{
		if (AddFood(ingredient))
		{
			if (aPlayer)
			{
				aPlayer->SetHeldItem(nullptr);
				ingredient->Highlight();
			}
			
		}
	}
	else if (Plate* plate = dynamic_cast<Plate*>(anitem))
	{
		if (myFood && plate->CanPlateIngredient(myFood))
		{
			plate->PlateIngredient(myFood);
			myFood = nullptr;
			myHeldItem = nullptr;
			StopCooking();
			mySteamTrail->GetGameObject().SetActive(false);
		}
	}
	else if (Skillet* skillet = dynamic_cast<Skillet*>(anitem))
	{
		if (skillet->GetIsPot() && skillet->myFood && !myFood)
		{
			if (skillet->myFood->CanBeFried())
			{
				AddFood(skillet->TakeFood());
			}
		}
		else if (myFood && GetIsPot() && !skillet->GetIsPot())
		{
			if (myFood->CanBeFried() && !skillet->myFood)
			{
				skillet->AddFood(TakeFood());
			}
		}
	}
}

bool Skillet::AddFood(Ingredient* aFood)
{
	if (myFood) return false;
	if (aFood->GetIsBurnt()) return false;
	if (GetIsPot())
	{
		if (!aFood->CanBeBoiled())
		{
			return false;
		}
	}
	else if (!aFood->CanBeFried())
	{
		return false;
	}

	std::shared_ptr<Model> friedMesh = AssetRegistry::GetInstance()->GetAsset<Model>(IngredientTypeToName(aFood->GetIngredientType()) + "_fried_mesh");
	if (friedMesh)
	{
		aFood->GetGameObject().GetComponent<ModelInstance>()->SetModel(friedMesh);
	}

	myFood = aFood;
	myFood->ResetBurn();
	myHeldItem = myFood;


	myFood->SetMyCanInteract(false);
	myFood->GetTransform()->SetParent(myTransform);
	myFood->GetTransform()->SetLocalRot(0, 0, 0);
	myFood->GetTransform()->SetLocalPos(0, 0, 0);
	//myFood->GetTransform()->Reset();
	return true;
}

void Skillet::OnPickup()
{
	StopCooking();
}

Ingredient* Skillet::TakeFood()
{
	if (myFood)
	{
		myFood->SetMyCanInteract(true);
		Ingredient* food = myFood;
		myHeldItem = nullptr;
		myFood = nullptr;

		mySteam->GetGameObject().SetActive(false);
		mySmoke->GetGameObject().SetActive(false);
		mySteamTrail->GetGameObject().SetActive(false);
		StopCooking();
		return food;
	}
	return nullptr;
}
