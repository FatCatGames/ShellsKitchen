#include "Game.pch.h"
#include "RippleSpawner.h"
#include "Components/Physics/Collisions/Collider.h"

void RippleSpawner::Awake()
{
	OnEdited();
}

void RippleSpawner::Update()
{

	if (myTransform && myRipples.size() > 0)
	{
		//if (myPreviousSpin != myTransform->localRot().y)
		//{
		//	mySpawnRippleTimer += fixedDeltaTime;
		//}
		//else
		//{
		//	mySpawnRippleTimer -= fixedDeltaTime;
		//}
		//mySpawnRippleTimer = clamp(mySpawnRippleTimer, 0.f, 1.f);
		mySpawnRippleTimer += fixedDeltaTime;

		if (mySpawnRippleTimer > myRespawnTimer)
		{
			//mySpawnRippleTimer = 0.f;
			//auto meme = InstantiatePrefab("SharkRipplesVFX");
			//Vector3f initPos = myTransform->worldPos();
			//meme->GetTransform()->SetWorldPos(initPos + Vector3f(0.f,0.3f,0.f));

			//meme->GetTransform()->SetLocalRot(0.f, myTransform->localRot().y, 0.f);
			//meme->GetTransform()->SetLocalScale(1.f, 1.f, 1.4f);
			Vector3f initPos = myTransform->worldPos();
			initPos.y = 0.3f;
			//initPos = Vector3f(initPos.x, 0.2f, initPos.z);
			//if (myIsBobbing)
			//{
				//myRipples[myCurrentRipple]->GetTransform()->SetLocalRot(0.f, myTransform->localRot().y, 0.f);
				//myRipples[myCurrentRipple]->SetAliveTime(0.f);
			//}
			//if (Vector3f(initPos - myPrevPos).LengthSqr() > 0.01f)
			//{
			//	initPos = Vector3f(initPos.x, 0.3f, initPos.z);
			//	myRipples[myCurrentRipple]->GetTransform()->SetWorldPos(initPos);
			//	myRipples[myCurrentRipple]->SetAliveTime(0.f);
			//}
			if (abs(initPos.x - myPrevPos.x) > 0.1f || abs(initPos.z - myPrevPos.z) > 0.1f)
			{
				initPos = Vector3f(initPos.x, 0.3f, initPos.z);
				float fun = 0.f;
				if (myFunnyFin)
				{
					fun = myFunnyFin->GetTransform()->worldPos().y * 0.3f;
				}
				myRipples[myCurrentRipple]->GetTransform()->SetWorldPos(initPos);
				myRipples[myCurrentRipple]->SetAliveTime(0.f);
				myRipples[myCurrentRipple]->SetCustomFloat(initPos.Distance(myPrevPos) + fun);
			}


			

			myCurrentRipple++;
			myCurrentRipple %= 20;
			mySpawnRippleTimer = 0.f;
			myPrevPos = myTransform->worldPos();
			myPrevPos.y = 0.3f;
		}
		//myPreviousSpin = myTransform->localRot().y;
	}

}

void RippleSpawner::RenderInProperties(std::vector<Component*>& aComponentList)
{
	myHasChangedSettings |= Catbox::DragFloat("Spawn Timer", &myRespawnTimer, 0.01f, 0.1f, 1.f);
	//myHasChangedSettings |= Catbox::Checkbox("Bobbing", &myIsBobbing);
}

void RippleSpawner::OnCollisionEnter(Collider* aCollider)
{
	if (aCollider->GetGameObject().GetName() == "Boat")
	{
		auto meme = InstantiatePrefab("WaterSplashVFX_axol");
		meme->GetTransform()->SetWorldPos(myTransform->worldPos().x, 0.2f, myTransform->worldPos().z);
	}

}

void RippleSpawner::OnDestroy()
{
	if (myRipples.size() > 0)
	{
		for (auto ripple : myRipples)
		{
			ripple->Destroy();
		}
		myRipples.clear();
	}
}

void RippleSpawner::RunInEditor()
{
	//if (myHasChangedSettings)
	//{
	//	OnEdited();
	//}
	Update();
}

void RippleSpawner::Save(rapidjson::Value& aComponentData)
{
	int version = 0;
	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveValue<DataType::Float>("RespawnTimer", myRespawnTimer);
	//wrapper.SaveValue<DataType::Bool>("Bobbing", myIsBobbing);
}

void RippleSpawner::Load(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();


	myRespawnTimer = aComponentData["RespawnTimer"].GetFloat();
	//myIsBobbing = aComponentData["Bobbing"].GetBool();
}

void RippleSpawner::OnEdited()
{
	if (myTransform)
	{
		if (myRipples.size() > 0)
		{
			for (auto ripple : myRipples)
			{
				ripple->Destroy();
			}
			myRipples.clear();
		}
		if (myTransform->GetParent() != nullptr)
		{
			myFunnyFin = myTransform->GetParent()->GetGameObject();
			if (myFunnyFin)
			{
				if (myFunnyFin->GetName() == "Sharkfin_prefab")
				{
					myPrefabName = "SharkRipplesVFX";
				}
			}
		}


		for (int i = 0; i < 20; i++)
		{
			GameObject* ripple = nullptr;
			Vector3f initPos = myTransform->worldPos();
			//if (myIsBobbing)
			//{
				//ripple = InstantiatePrefab("SharkRipplesVFX");
				//ripple->GetTransform()->SetLocalScale(1.f, 1.f, 1.4f);
				//initPos += Vector3f(0.f, 0.f, -2.3f);
			//}
			//else
			//{
				ripple = InstantiatePrefab(myPrefabName);
			//}
			//auto meme = InstantiatePrefab("SharkRipplesVFX");

			ripple->GetTransform()->SetWorldPos(initPos + Vector3f(0.f, 0.3f, 0.f));


			myRipples.push_back(ripple);
		}
	}
	myHasChangedSettings = false;
}
