#include "Game.pch.h"
#include "Stove.h"
#include "..\Holdables\Skillet.h"


void Stove::Awake()
{
	InteractableSurface::Awake();
	if (myHeldItem) mySkillet = dynamic_cast<Skillet*>(myHeldItem);
	GameManager::GetInstance()->GetCurrentLevelManager().AddStove(this);
}

void Stove::Interact(Player* aPlayer)
{
	if (myHeldItem)
	{
		if (!aPlayer->GetHeldItem())
		{
			aPlayer->SetHeldItem(myHeldItem);
			myHeldItem = nullptr;
			mySkillet = nullptr;
		}
	}
}

void Stove::InteractWithHeldItem(Player* aPlayer, HoldableItem* anItem)
{
	if (mySkillet)
	{
		mySkillet->InteractWithHeldItem(aPlayer, anItem);
	}
	else if (!myHeldItem)
	{
		if (Skillet* skillet = dynamic_cast<Skillet*>(anItem))
		{
			if(aPlayer) aPlayer->SetHeldItem(nullptr);
			AddSkillet(skillet);
			myHeldItem->Highlight();
		}
	}
}

void Stove::Update()
{
	InteractableSurface::Update();
	if (mySkillet)
	{
		mySkillet->Cook();
	}
}

void Stove::AddSkillet(Skillet* aSkillet)
{
	aSkillet->GetTransform()->SetParent(myTransform);
	aSkillet->GetTransform()->SetLocalPos(Vector3f::up() * 0.5f);
	aSkillet->GetTransform()->SetLocalRot(0, 0, 0);
	myHeldItem = aSkillet;
	mySkillet = aSkillet;
}
