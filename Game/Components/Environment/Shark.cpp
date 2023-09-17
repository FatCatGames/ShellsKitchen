#include "Game.pch.h"
#include "Shark.h"
#include "Components\Animator.h"
#include "Components\Physics\Collisions\Collider.h"

void Shark::Awake()
{
	myAnimator = myGameObject->GetComponent<Animator>();

	myListener.action = [this](void* data) {LiveSharkReaction(reinterpret_cast<OrderResult*>(data)); };
	OrderHandler::GetInstance()->AddOrderDeliveredListener(myListener);

	myOrderTimedOutListener.action = [this](void* data) {LiveSharkReaction(reinterpret_cast<OrderResult*>(data)); };
	OrderHandler::GetInstance()->AddOrderTimedOutListener(myOrderTimedOutListener);

	myLevelCounterComponent = GameManager::GetInstance()->GetLevelTimer();

	myThirtySecondsListener.action = [this]() {myAnimator->SetBool("LongTime", true); };
	myLevelCounterComponent->AddThirtySecondsListener(myThirtySecondsListener);

	myIsClamsay = Catbox::ToLowerString(myGameObject->GetName()).find("gordon") != std::string::npos;
}

void Shark::Update()
{
	if (!myShouldUpdateTriggers) return;
	myShouldUpdateTriggers = false;
	myAnimator->SetBool("RightOrder", false);
	myAnimator->SetBool("WrongOrder", false);
	myAnimator->SetBool("IngredientHit", false);
}

void Shark::LiveSharkReaction(OrderResult* aResult)
{
	if (aResult->success)
	{
		myAnimator->SetBool("RightOrder", true);
	}
	else
	{
		myAnimator->SetBool("WrongOrder", true);
	}

	myShouldUpdateTriggers = true;
}

void Shark::OrderTimedOut()
{
	myAnimator->SetBool("WrongOrder", true);
	myShouldUpdateTriggers = true;
}


void Shark::OnCollisionEnter(Collider* aCollider)
{
	if (aCollider->GetCollisionLayerName() == "Ingredient")
	{
		Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_BUMP));
		
		if (myIsClamsay) 
		{
			aCollider->GetGameObject().Destroy();
		}
		myAnimator->SetBool("IngredientHit", true);
		myShouldUpdateTriggers = true;
	}
}