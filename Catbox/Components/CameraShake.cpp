#include "stdafx.h"
#include "CameraShake.h"
#include "CameraController.h"

CameraShake::~CameraShake()
{
	Stop();
}

void CameraShake::Awake()
{
	if (!myGameObject->GetComponent<Camera>())
	{
		printerror("Camera Shake needs to be attached to a camera");
	}
	myTempShakeDuration = myShakeDuration;
	myTempShakeIntensity = myCameraShakeIntensity;

	if (myPlayOnAwake)
	{
		Start();
	}
}

void CameraShake::Init()
{
	myCameraShakeActive = false;
}

void CameraShake::Update()
{
	if (Engine::GetInstance()->IsGamePaused())
	{
		myCameraShakeActive = false;
		return;
	}

	if (myCameraShakeActive == true)
	{
		myShakeTime += deltaTime;
		if (myShakeTime >= myTempShakeDuration)
		{
			Stop();
			return;
		}

		myCameraOrigin.offset = Engine::GetInstance()->GetMainCamera()->GetTransform()->worldPos();
		myCameraOrigin.rotation = Engine::GetInstance()->GetMainCamera()->GetTransform()->worldRot();
		myCameraOrigin.fov = Engine::GetInstance()->GetMainCamera()->GetFoV();

		Vector3f dir = { Catbox::GetRandom(-1.0f, 1.0f), 0, Catbox::GetRandom(-1.0f, 1.0f) };
		Vector3f shakeDistance = myTempShakeIntensity * dir * deltaTime;

		if (&myTemporaryCamera->GetGameObject() != nullptr)
		{
			myTemporaryCamera->GetTransform()->SetWorldPos(myCameraOrigin.offset + shakeDistance);
			myTemporaryCamera->GetTransform()->SetWorldRot(myCameraOrigin.rotation);
			myTemporaryCamera->SetFoV(myCameraOrigin.fov);
		}
	}
}

void CameraShake::Start()
{
	Stop();

	auto temporaryCameraObj = Instantiate();
	myTemporaryCamera = temporaryCameraObj->AddComponent<Camera>();
	auto myCam = Engine::GetInstance()->GetActiveCamera();
	if (!myCam) myCam = Engine::GetInstance()->GetMainCamera();

	myTemporaryCamera->SetFoV(myCam->GetFoV());
	myTemporaryCamera->GetTransform()->SetWorldPos(myCam->GetTransform()->worldPos());
	myTemporaryCamera->GetTransform()->SetWorldRot(myCam->GetTransform()->worldRot());
	myTemporaryCamera->SetPostProcessVolume(myCam->GetPostProcessingVolume());

	temporaryCameraObj->SetName("Temporary camera (DON'T CLICK)");

	Engine::GetInstance()->SetActiveCamera(myTemporaryCamera);

	myCameraShakeActive = true;
}

void CameraShake::Pause()
{
	myCameraShakeActive = false;
}

void CameraShake::Stop()
{
	if (!myCameraShakeActive) return;

	if (myTemporaryCamera != nullptr)
	{
		Engine::GetInstance()->SetActiveCamera(Engine::GetInstance()->GetMainCamera());
		Engine::GetInstance()->GetCameraController()->RemoveCamera(myTemporaryCamera);
		myTemporaryCamera->GetGameObject().Destroy();
	}

	myShakeTime = 0.0f;
	myTempShakeDuration = myShakeDuration;
	myTempShakeIntensity = myCameraShakeIntensity;
	myCameraShakeActive = false;
}

void CameraShake::RenderInProperties(std::vector<Component*>& aComponentList)
{
	if (PLAYMODE) 
	{
		if (ImGui::Button("Play"))
		{
			Start();
		}
		if (ImGui::Button("Pause"))
		{
			Pause();
		}
		if (ImGui::Button("Stop"))
		{
			Stop();
		}
	}

	ImGui::DragFloat("Intensity", &myCameraShakeIntensity, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Duration", &myShakeDuration, 0.1f, 0.0f, 780.0f);
	ImGui::Checkbox("Play on Awake", &myPlayOnAwake);
}

void CameraShake::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Float>("Intensity", myCameraShakeIntensity);
	wrapper.SaveValue<DataType::Float>("Duration", myShakeDuration);
	wrapper.SaveValue<DataType::Bool>("PlayOnAwake", myPlayOnAwake);
}

void CameraShake::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Intensity"))
	{
		myCameraShakeIntensity = aComponentData["Intensity"].GetFloat();
	}
	if (aComponentData.HasMember("Duration"))
	{
		myShakeDuration = aComponentData["Duration"].GetFloat();
	}
	if (aComponentData.HasMember("PlayOnAwake"))
	{
		myPlayOnAwake = aComponentData["PlayOnAwake"].GetBool();
	}
}