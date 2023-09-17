#include "stdafx.h"
#include "SGNode_DestroyGameObject.h"
#include "Components\BlueprintComponent.h"

void SGNode_DestroyGameObject::Init()
{
	SetTitle("Destroy Object");
	SetCategory("GameObjects");
	CreateDataPin<int>("ObjectID", PinDirection::Input);
	CreateExecPin("In", PinDirection::Input, true); 
	CreateExecPin("Out", PinDirection::Output, true); 
}

size_t SGNode_DestroyGameObject::DoOperation()
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
		object->Destroy();
	}

	return ExitViaPin("Out");
}