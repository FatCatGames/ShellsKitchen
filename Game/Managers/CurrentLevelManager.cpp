#include "Game.pch.h"
#include "CurrentLevelManager.h"
#include "Components\Interactables\Surfaces\Stove.h"
#include "Components\Interactables\Surfaces\Counter.h"

CurrentLevel* CurrentLevel::Instance;

CurrentLevel::CurrentLevel()
{
	Instance = this;
}

void CurrentLevel::ResetLevel()
{
	myStoves.clear();
	myDeliveryDesk = nullptr;
	myPlateHolder = nullptr;
}

Stove* CurrentLevel::GetAvailableStove()
{
	for (auto stove : myStoves)
	{
		if (stove->HasItem()) continue;
		return stove;
	}
	return nullptr;
}