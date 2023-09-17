#include "Game.pch.h"
#include "AutoFoam.h"

void AutoFoam::Awake()
{
	if (myTransform)
	{
		if (myTransform->GetParent())
		{
			myParent = myTransform->GetParent();
			myFin = myParent->GetGameObject()->GetComponent<Sharkfin>();
			//boatPos = GameManager::GetInstance()->GetBoatPos();
		}
	}
}

void AutoFoam::Update()
{
	if (myParent)
	{
		myGameObject->SetCustomFloat(myFin->GetTransform()->worldPos().y);
		auto coolBoat = GameManager::GetInstance()->GetBoatPos();
		auto myPos = myTransform->worldPos();
		myTransform->SetWorldPos(myPos.x, 0.2f, myPos.z);
		

		//if (myParent->worldPos().Distance(coolBoat) < 10.f)
		//{
		//}
	}
}

void AutoFoam::RunInEditor()
{
	Update();
}
