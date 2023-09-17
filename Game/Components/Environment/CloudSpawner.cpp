#include "Game.pch.h"
#include "CloudSpawner.h"
#include "Components/3D/ModelInstance.h"
#include "../Catbox/Assets/Model.h"

void CloudSpawner::Awake()
{
	myTransform = GetTransform();
	ResetClouds();
}

void CloudSpawner::Init()
{

}

void CloudSpawner::Update()
{
	CloudUpdate();

	//if (myActiveClouds < myActiveClouds)
	//{
	//	if (myTimeSinceLastSpawn < myCloudRespawnTimer)
	//	{
	//		//auto meme = InstantiatePrefab("mee");
	//	}
	//}
	
}

void CloudSpawner::RunInEditor()
{
	CloudUpdate();
	//if (!myTransform)
	//{
	//	myTransform = GetTransform();
	//}

	//if (myHasChangedSettings)
	//{
	//	ResetClouds();
	//	myHasChangedSettings = false;
	//}

	//if (myTransform && myClouds.size() > 0)
	//{
	//	if (myTimeSinceLastSpawn > myCloudRespawnTimer + 1.f)
	//	{
	//		float x = Catbox::GetRandom(0.f, 2.f) * myRadius;
	//		float y = Catbox::GetRandom(0.f, 2.f) * myRadius;
	//		float z = Catbox::GetRandom(0.f, 2.f) * myRadius;
	//		myClouds[myCloudIndex]->GetTransform()->SetWorldPos((myTransform->worldPos() - Vector3f(1.f, 0.f, 1.f) * myRadius) + Vector3f(x, 0.f, z));
	//		myClouds[myCloudIndex]->GetTransform()->SetLocalRot(0.f, Catbox::GetRandom(0.f, 360.f), 0.f);
	//		myClouds[myCloudIndex]->SetAliveTime(0.f);
	//		myCloudIndex++;
	//		myCloudIndex %= myMaxAmountOfClouds;
	//		myTimeSinceLastSpawn = 0.f;
	//	}

	//}


	//if (myClouds.size() > 0)
	//{
	//	for (auto cloud : myClouds)
	//	{

	//		float time = cloud->GetAliveTime();
	//		cloud->GetTransform()->SetLocalScale(Vector3f::one() * min(time * time, 1.f) * myCloudMaxSize);

	//		auto transform = cloud->GetTransform();

	//		transform->SetWorldPos(transform->worldPos() + (transform->forward() * myWindForce) * deltaTime);
	//	}
	//}

	//myTimeSinceLastSpawn += deltaTime;
}

void CloudSpawner::OnDestroy()
{
	if (myClouds.size() > 0)
	{
		for (auto cloud : myClouds)
		{
			cloud->Destroy();
		}
		myClouds.clear();
	}
}

void CloudSpawner::RenderInProperties(std::vector<Component*>& aComponentList)
{
	myHasChangedSettings |= Catbox::DragInt("Amount of clouds", &myMaxAmountOfClouds, 1, 1, 1000);	
	//myHasChangedSettings |= Catbox::DragFloat("RespawnTimer", &myCloudRespawnTimer, 0.1f, 0.1f, 50.f);
	myHasChangedSettings |= Catbox::DragFloat("SpawnRadius", &myRadius, 1.f, 1.f, 500.f);
	myHasChangedSettings |= Catbox::DragFloat("MaxSize", &myCloudMaxSize, 0.1f, 1.f, 10.f);
	myHasChangedSettings |= Catbox::DragFloat("WindForce", &myWindForce, 1.f, 1.f, 1000.f);
}

void CloudSpawner::Save(rapidjson::Value& aComponentData)
{
	int version = 0;
	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveValue<DataType::Int>("MaxAmount", myMaxAmountOfClouds);
	wrapper.SaveValue<DataType::Float>("Radius", myRadius);
	wrapper.SaveValue<DataType::Float>("MaxSize", myCloudMaxSize);
	wrapper.SaveValue<DataType::Float>("WindForce", myWindForce);

}

void CloudSpawner::Load(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();


	myMaxAmountOfClouds = aComponentData["MaxAmount"].GetInt();
	myRadius = aComponentData["Radius"].GetFloat();
	myWindForce = aComponentData["WindForce"].GetFloat();
	myCloudMaxSize = aComponentData["MaxSize"].GetFloat();
	
}

void CloudSpawner::ResetClouds()
{
	myFadeoutTime = myMaxAmountOfClouds * myCloudRespawnTimer;
	if (myClouds.size() > 0)
	{
		for (auto cloud : myClouds)
		{
			cloud->Destroy();
		}
		myClouds.clear();
	}
	for (int i = 0; i < myMaxAmountOfClouds; i++)
	{
		auto cloud = InstantiatePrefab("Cloud");

		//cloud->GetTransform()->SetWorldPos((myTransform->worldPos() - Vector3f(1.f, 0.f, 1.f) * myRadius));
		float x = Catbox::GetRandom(0.f, 2.f) * myRadius;
		float y = Catbox::GetRandom(-4.f, 4.f);
		float z = Catbox::GetRandom(0.f, 2.f) * myRadius;
		float randomSpeed = Catbox::GetRandom(0.f, 2.f);

		cloud->GetTransform()->SetWorldPos((myTransform->worldPos() - Vector3f(1.f, 0.f, 1.f) * myRadius) + Vector3f(x, y, z));
		cloud->GetTransform()->SetLocalRot(0.f, Catbox::GetRandom(0.f, 360.f), 0.f);
		cloud->GetTransform()->SetLocalScale(myCloudMaxSize, myCloudMaxSize, myCloudMaxSize);
		cloud->SetCustomFloat(randomSpeed);

		auto model = cloud->GetComponent<ModelInstance>();
		model->SetRenderToUse(Renderer::Forward);

		if (i % 3 == 1)
		{
			model->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>("CloudOne"));
			model->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>("CloudOne_mat"), 0);
		}
		else if (i % 3 == 2)
		{
			model->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>("CloudTwo"));
			model->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>("CloudsTwo_mat"), 0);
		}
		else
		{
			model->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>("CloudThree"));
			model->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>("CloudsThree_mat"), 0);
		}

		myClouds.push_back(cloud);

	}
}

void CloudSpawner::CloudUpdate()
{
	if (!myTransform)
	{
		myTransform = GetTransform();
	}

	if (myHasChangedSettings)
	{
		ResetClouds();
		myHasChangedSettings = false;
	}

	//if (myTransform && myClouds.size() > 0)
	//{
	//	if (myTimeSinceLastSpawn > myCloudRespawnTimer)
	//	{
	//		
	//	}

	//	if (myTimeSinceLastSpawn > myCloudRespawnTimer + 1.f)
	//	{
	//		float x = Catbox::GetRandom(0.f, 2.f) * myRadius;
	//		float z = Catbox::GetRandom(0.f, 2.f) * myRadius;
	//		myClouds[myCloudIndex]->GetTransform()->SetWorldPos((myTransform->worldPos() - Vector3f(1.f, 0.f, 1.f) * myRadius) + Vector3f(x, 0.f, z));
	//		myClouds[myCloudIndex]->GetTransform()->SetLocalRot(0.f, Catbox::GetRandom(0.f, 360.f), 0.f);
	//		myClouds[myCloudIndex]->SetAliveTime(0.f);
	//		myCloudIndex++;
	//		myCloudIndex %= myMaxAmountOfClouds;
	//		myTimeSinceLastSpawn = 0.f;
	//	}

	//}


	if (myClouds.size() > 0)
	{
		for (auto cloud : myClouds)
		{
			float aliveTime = cloud->GetAliveTime();
			//cloud->GetTransform()->SetLocalScale(Vector3f::one() * min(aliveTime * aliveTime, 1.f) * (myCloudMaxSize + cloud->GetCustomFloat() * 2.f));

			auto transform = cloud->GetTransform();

			transform->SetWorldPos(transform->worldPos() + (Vector3f(1,0,0) * max(myWindForce - cloud->GetCustomFloat(), 0.1f)) * deltaTime);


			if (transform->worldPos().x > 300.f)
			{
				transform->SetWorldPos(-300.f, transform->worldPos().y, transform->worldPos().z);
			}
			//if (aliveTime > myCloudRespawnTimer - cloud->GetCustomFloat() * 2.f)
			//{
			
				//cloud->SetCustomFloat(max(myCloudRespawnTimer - aliveTime, 0.f));
			
			
				//if (aliveTime > myCloudRespawnTimer)
				//{
				//	float x = Catbox::GetRandom(0.f, 2.f) * myRadius;
				//	float z = Catbox::GetRandom(0.f, 2.f) * myRadius;
				//	cloud->GetTransform()->SetWorldPos((myTransform->worldPos() - Vector3f(1.f, 0.f, 1.f) * myRadius) + Vector3f(x, 0.f, z));
				//	cloud->GetTransform()->SetLocalRot(0.f, Catbox::GetRandom(0.f, 360.f), 0.f);
				//	cloud->SetAliveTime(0.f);
				//	cloud->SetCustomBool(false);
				//}
			
			//}
			//else
			//{
			//	cloud->SetCustomFloat(min(aliveTime, 1.f));
			//}
		}
	}

	myTimeSinceLastSpawn += deltaTime;
}
