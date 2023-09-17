#include "Game.pch.h"
#include "DistanceBasedZoomCamera.h"

void DistanceBasedZoomCamera::Awake()
{
	myManagerRefrence = GameManager::GetInstance();
	myStartPosition = myTransform->worldPos();
	auto camera = myGameObject->GetComponent<Camera>();
	if (camera) 
	{
		myStartingZoom = camera->GetFoV();
		myCamera = camera;
	}
	
	myTargetsStartMiddlePoint = Vector3f::zero();
	auto players = myManagerRefrence->GetPlayers();
	for (size_t i = 0; i < players.size(); i++)
	{
		myTargetsStartMiddlePoint += players[i]->GetTransform()->worldPos();
		myTargets.push_back(players[i]->GetTransform());
	}
	myTargetsStartMiddlePoint /= static_cast<float>(myTargets.size());
}

void DistanceBasedZoomCamera::Update()
{
	if (myManagerRefrence->GetPlayers().size() != myTargets.size())
	{
		myTargetsStartMiddlePoint = Vector3f::zero();
		myTargets.clear();
		auto players = myManagerRefrence->GetPlayers();
		for (size_t i = 0; i < players.size(); i++)
		{
			myTargetsStartMiddlePoint += players[i]->GetTransform()->worldPos();
			myTargets.push_back(players[i]->GetTransform());
		}
		myTargetsStartMiddlePoint /= static_cast<float>(myTargets.size());
		return;
	}
	if (myStartingZoom == -1 || myTargets.size() <= 1) 
	{
		return;
	}
	float distance = -1;

	for (int i = 0; i < myTargets.size(); i++)
	{
		myTargetLookPosition += myTargets[i]->worldPos();
		for (int j = 0; j < myTargets.size(); j++)
		{
			//Dont check agaist itself
			if (i == j) 
			{
				continue;
			}
			float compareDistance = myTargets[i]->worldPos().Distance(myTargets[j]->worldPos());
			if (compareDistance > distance)
			{
				myFurthestTargets[0] = myTargets[i]->worldPos();
				myFurthestTargets[1] = myTargets[j]->worldPos();
				distance = compareDistance;
			}
		}
	}
	if (myTargetFov != myCamera->GetFoV())
	{
		myCamera->SetFoV(Catbox::Lerp<float>(myCamera->GetFoV(), myTargetFov, 2.0f * deltaTime));
	}

	myTargetLookPosition /= static_cast<float>(myTargets.size());

	float targetX = myTransform->worldPos().x;
	if (myTargetLookPosition.x > myTargetsStartMiddlePoint.x) 
	{
		targetX = Catbox::Lerp<float>(targetX, myStartPosition.x + myOffsetMax.x, 1.0f * deltaTime);
	}
	else 
	{
		targetX = Catbox::Lerp<float>(targetX, myStartPosition.x + myOffsetMin.x, 1.0f * deltaTime);
	}

	float targetZ = myTransform->worldPos().z;
	if (myTargetLookPosition.z > myTargetsStartMiddlePoint.z)
	{
		targetZ = Catbox::Lerp<float>(targetZ, myStartPosition.z + myOffsetMax.y, 1.0f * deltaTime);
	}
	else
	{
		targetZ = Catbox::Lerp<float>(targetZ, myStartPosition.z + myOffsetMin.y, 1.0f * deltaTime);
	}

	myTransform->SetWorldPos({ targetX, myTransform->worldPos().y, targetZ });

	float percent = FLT_MAX;

	if (distance < myStartMinDistance && distance > myMinDistance)
	{
		percent = (distance - myMinDistance) / (myStartMinDistance - myMinDistance);
		percent -= 1.0f;
		percent = std::abs(percent);

		myTargetFov = (myStartingZoom - ((myStartingZoom - myMinFov) * percent));
	}
	else if (distance > myStartMaxDistance && distance < myMaxDistance)
	{
		percent = (distance - myStartMaxDistance) / (myMaxDistance - myStartMaxDistance);

		myTargetFov = (myStartingZoom - ((myStartingZoom - myMaxFov) * percent));

	}
}

void DistanceBasedZoomCamera::Save(rapidjson::Value& aComponentData)
{
	int version = 2;	
	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Int, int>("Version", version);

	wrapper.SaveValue<DataType::Float, float>("Max Fov", myMaxFov);
	wrapper.SaveValue<DataType::Float, float>("Min Fov", myMinFov);

	wrapper.SaveValue<DataType::Float, float>("End Zoom Max", myMaxDistance);
	wrapper.SaveValue<DataType::Float, float>("End Zoom Min", myMinDistance);

	wrapper.SaveValue<DataType::Float, float>("Start Zoom Max", myStartMaxDistance);
	wrapper.SaveValue<DataType::Float, float>("Start Zoom Min", myStartMinDistance);

	wrapper.SaveValue<DataType::Float, float>("Start Zoom Max", myStartMaxDistance);
	wrapper.SaveValue<DataType::Float, float>("Start Zoom Min", myStartMinDistance);

	wrapper.SaveValue<DataType::Float, float>("Offset Max X", myOffsetMax.x);
	wrapper.SaveValue<DataType::Float, float>("Offset Max Y", myOffsetMax.y);

	wrapper.SaveValue<DataType::Float, float>("Offset Min X", myOffsetMin.x);
	wrapper.SaveValue<DataType::Float, float>("Offset Min Y", myOffsetMin.y);
}

void DistanceBasedZoomCamera::Load(rapidjson::Value& aComponentData)
{
	int version = -1;
	if (aComponentData.HasMember("Version")) 
	{
		version = aComponentData["Version"].GetInt();
	}
	if (version > 0) 
	{
		myMaxFov = aComponentData["Max Fov"].GetFloat();
		myMinFov = aComponentData["Min Fov"].GetFloat();

		myMaxDistance = aComponentData["End Zoom Max"].GetFloat();
		myMinDistance = aComponentData["End Zoom Min"].GetFloat();

		myStartMaxDistance = aComponentData["Start Zoom Max"].GetFloat();
		myStartMinDistance = aComponentData["Start Zoom Min"].GetFloat();
		if (version > 1) 
		{
			myOffsetMax.x = aComponentData["Offset Max X"].GetFloat();
			myOffsetMax.y = aComponentData["Offset Max Y"].GetFloat();

			myOffsetMin.x = aComponentData["Offset Min X"].GetFloat();
			myOffsetMin.y = aComponentData["Offset Min Y"].GetFloat();
		}
	}
}

void DistanceBasedZoomCamera::RenderInProperties(std::vector<Component*>& someComponents)
{
	ImGui::InputFloat("Max FoV", &myMaxFov);
	ImGui::InputFloat("Min FoV",  &myMinFov);

	ImGui::InputFloat("End Zoom Max", &myMaxDistance);
	ImGui::InputFloat("End Zoom Min", &myMinDistance);

	ImGui::InputFloat("Start Zoom Max", &myStartMaxDistance);
	ImGui::InputFloat("Start Zoom Min", &myStartMinDistance);

	ImGui::InputFloat2("Max Offset", &myOffsetMax.x);
	ImGui::InputFloat2("Min Offset", &myOffsetMin.x);

}
