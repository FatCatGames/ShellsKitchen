#include "stdafx.h"
#include "SGNode_GetObjectName.h"
#include "Components\BlueprintComponent.h"

void SGNode_GetObjectName::Init()
{
	SetTitle("Get Object Name");
	SetCategory("GameObjects");

	CreateDataPin<int>("ObjectID", PinDirection::Input);
	CreateDataPin<std::string>("Name", PinDirection::Output);
}

size_t SGNode_GetObjectName::DoOperation()
{
	int id = myOwner->GetObjectID();
	int otherID = -1;
	GetPinData("ObjectID", otherID);
	if (otherID != -1)
	{
		id = otherID;
	}

	GameObject* object = Engine::GetGameObject(id);
	if (object)
	{
		SetPinData("Name", object->GetName());
		return Exit();
	}
	SetPinData("Name", "");

	return 0;
}