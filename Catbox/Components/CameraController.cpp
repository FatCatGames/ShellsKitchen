#include "stdafx.h"
#include "CameraController.h"
#include "Physics/PhysXUtilities.h"

CameraController::CameraController()
{
	myCameras = {};
	myCurrentCamera = nullptr;
	myDelayStartTime = 0.0f;
	myTempStartTime = 0.0f;
	myTemporaryCamera = nullptr;
	myIsTransitioning = false;
	myIsCurving = false;
	myIsDone = false;
}

CameraController::~CameraController()
{
	myTemporaryCamera = nullptr;
}

void CameraController::AddCamera(Camera* aCamera)
{
	myCameras.push_back(aCamera);
}

std::vector<Camera*>& CameraController::GetCameras()
{
	return myCameras;
}

void CameraController::RemoveCamera(Camera* aCamera)
{
	if (myTemporaryCamera != nullptr)
	{
		if (myTemporaryCamera->GetComponent<Camera>() == aCamera)
		{
			myTemporaryCamera->Destroy();
		}
	}

	if (myCameras.size() > 0)
	{
		// Could be important to add back.
		/*if (Engine::GetInstance()->GetMainCamera() == aCamera)
		{
			Engine::GetInstance()->SetMainCamera(myCameras[0]);
			myCurrentCamera = Engine::GetInstance()->GetActiveCamera();
		}

		if (Engine::GetInstance()->GetActiveCamera() == aCamera)
		{
			Engine::GetInstance()->SetActiveCamera(Engine::GetInstance()->GetMainCamera());
			myCurrentCamera = Engine::GetInstance()->GetActiveCamera();
		}*/
	}

	for (int i = 0; i < myCameras.size(); i++)
	{
		if (aCamera == myCameras[i])
		{
			myCameras.erase(myCameras.begin() + i);
		}
	}
}

//void CameraController::SetPlayerCamera(PlayerCamera* aCamera)
//{
//	myPlayerCamera = aCamera;
//	myCurrentCamera = aCamera;
//}

//PlayerCamera* CameraController::GetPlayerCamera()
//{
//	if (myPlayerCamera == nullptr) { printerror("No Player Camera in scene"); }
//	return dynamic_cast<PlayerCamera*>(myPlayerCamera);
//}

void CameraController::Update()
{
	PerformanceManager::BeginTimer("Camera Controller", "Engine");
	CinematicCamera* currentCam = dynamic_cast<CinematicCamera*>(myCurrentCamera);

	if (currentCam != nullptr && myTemporaryCamera == nullptr)
	{
		if (currentCam->GetNextCamera() != -1)
		{
			if (currentCam->GetIsInstant())
			{
				TransitionTo(myCameras[currentCam->GetNextCamera()]);
			}
			else
			{
				if (currentCam->GetDelay() > 0.0f && myDelayStartTime < currentCam->GetDelay())
				{
					myDelayStartTime += deltaTime;
				}
				else
				{
					myDelayStartTime = 0.0f;
					if (currentCam->GetSpeed() > 0.0f)
					{
						if (currentCam->GetUseCurve())
						{
							int recursiveCam = GetCameraId(currentCam);
							std::array<Camera*, 4> curve;
							bool functionalCurve = true;
							for (size_t i = 0; i < 4; i++)
							{
								int nextCam = static_cast<CinematicCamera*>(myCameras[recursiveCam])->GetNextCamera();
								if (nextCam == -1)
								{
									functionalCurve = false;
									break;
								}
								else
								{
									curve[i] = myCameras[recursiveCam];
									recursiveCam = nextCam;
								}
							}
							CurveTransition(curve, currentCam->GetSpeed());
						}
						else
						{
							SmoothTransitionTo(myCameras[currentCam->GetNextCamera()], currentCam->GetSpeed());
						}

					}
					else
					{
						TransitionTo(myCameras[currentCam->GetNextCamera()]);
					}
				}
			}
		}
		else
		{
			myIsDone = true;
		}
	}


	if (myTemporaryCamera != nullptr)
	{
		myTempStartTime += deltaTime;
		myTempStartTime = Catbox::Clamp(myTempStartTime, 0.0f, mySecondsToTransition);

		if (myIsCurving)
		{
			const float percentOfCurve = myTempStartTime / mySecondsToTransition;

			// DeCasteljau lerping
			Vector3f a = Catbox::Lerp(myCurve[0], myCurve[1], percentOfCurve);
			Vector3f b = Catbox::Lerp(myCurve[1], myCurve[2], percentOfCurve);
			Vector3f c = Catbox::Lerp(myCurve[2], myCurve[3], percentOfCurve);
			Vector3f d = Catbox::Lerp(a, b, percentOfCurve);
			Vector3f e = Catbox::Lerp(b, c, percentOfCurve);
			Vector3f P = Catbox::Lerp(d, e, percentOfCurve);

			myTemporaryCamera->GetTransform()->SetWorldPos(P);

			LerpFoV();
			LerpRotation();

			if (mySecondsToTransition - myTempStartTime < 0.01f)
			{
				myIsTransitioning = false;
				myIsCurving = false;
				TransitionTo(myCurrentCamera);
			}
		}
		else
		{
			LerpFoV();
			LerpPosition();

			Vector3f rot = myTemporaryCamera->GetTransform()->worldRot();
			Vector3f targetRot = myCurrentCamera->GetTransform()->worldRot();

			if ((myTemporaryCamera->GetTransform()->worldRot() - myCurrentCamera->GetTransform()->worldRot()).Length() > 0.0f)
			{
				LerpRotation();
			}

			if (mySecondsToTransition - myTempStartTime < 0.01f)
			{
				myIsTransitioning = false;

				TransitionTo(myCurrentCamera);
			}
		}

		if (myShouldLookAt == true)
		{
			myTemporaryCamera->GetTransform()->LookAt(myLookAtPos);
		}
	}
	else
	{
		if (dynamic_cast<CinematicCamera*>(myCurrentCamera) == nullptr)
		{
			myIsDone = true;
		}
	}

	/*if (myCurrentCamera != nullptr)
	{
		print(myCurrentCamera->GetGameObject().GetName());
	}*/

	// For Debug purposes
	/*if (Input::GetKeyPress(KeyCode::G))
	{
		if (myCameras.size() > 0)
		{
			SmoothTransitionTo(myCameras[1], 3.0f);
		}
	}*/
	PerformanceManager::StopTimer("Camera Controller");
}

void CameraController::Clear()
{
	myCameras.clear();

	myTemporaryCamera = nullptr;

	myCurrentCamera = Engine::GetInstance()->GetMainCamera();
}

void CameraController::TransitionTo(Camera* aCamera)
{
	myIsDone = false;
	myShouldLookAt = false;
	myTempStartTime = 0.0f;

	for (size_t i = 0; i < myCameras.size(); i++)
	{
		if (myCameras[i] == aCamera)
		{
			myCurrentCamera = myCameras[i];
		}
	}

	Engine::GetInstance()->SetActiveCamera(aCamera);

	if (myTemporaryCamera != nullptr)
	{
		RemoveCamera(myTemporaryCamera->GetComponent<Camera>());
	}

	myTemporaryCamera = nullptr;

	/*if (dynamic_cast<CinematicCamera*>(aCamera))
	{
		CinematicCamera* cam = dynamic_cast<CinematicCamera*>(aCamera);

		if (cam->GetNextCamera() != -1)
		{
			SmoothTransitionTo(myCameras[cam->GetNextCamera()], dynamic_cast<CinematicCamera*>(aCamera)->GetDelay());
		}
	}*/
}

void CameraController::SmoothTransitionTo(Camera* aCameraTo, float someSeconds)
{
	myIsDone = false;
	if (myCurrentCamera == nullptr) { myCurrentCamera = Engine::GetInstance()->GetActiveCamera(); }
	CinematicCamera* cam = dynamic_cast<CinematicCamera*>(myCurrentCamera);
	if (cam != nullptr && cam->GetLookAt() != -1)
	{
		myShouldLookAt = true;
		myLookAtPos = *myCameras[dynamic_cast<CinematicCamera*>(myCurrentCamera)->GetLookAt()]->GetTransform();
	}
	else
	{
		myShouldLookAt = false;
	}
	myDelayStartTime = 0.0f;
	myTempStartTime = 0.0f;
	myIsTransitioning = true;
	myTemporaryCamera = nullptr;
	for (size_t i = 0; i < myCameras.size(); i++)
	{
		if (myCameras[i] == aCameraTo)
		{
			myTemporaryCamera = Instantiate();
			Camera* cameraComponent = myTemporaryCamera->AddComponent<Camera>();

			cameraComponent->SetFoV(myCurrentCamera->GetFoV());
			cameraComponent->SetPostProcessVolume(myCurrentCamera->GetPostProcessingVolume());
			myTemporaryCamera->GetTransform()->SetWorldPos(myCurrentCamera->GetTransform()->worldPos());
			myTemporaryCamera->GetTransform()->SetWorldRot(myCurrentCamera->GetTransform()->worldRot());
			myTemporaryCamera->SetName("Temporary camera");

			myTemporaryCameraOrigin.fov = cameraComponent->GetFoV();
			myTemporaryCameraOrigin.offset = myTemporaryCamera->GetTransform()->worldPos();
			myTemporaryCameraOrigin.rotation = myTemporaryCamera->GetTransform()->worldRot();

			myCurrentCamera = aCameraTo;

			mySecondsToTransition = someSeconds;
			Engine::GetInstance()->SetActiveCamera(myTemporaryCamera->GetComponent<Camera>());
		}
	}
}

void CameraController::CurveTransition(std::array<Camera*, 4> someCurvedCameras, float someSeconds)
{
	myIsDone = false;
	if (myCurrentCamera == nullptr) { myCurrentCamera = Engine::GetInstance()->GetActiveCamera(); }
	CinematicCamera* cam = dynamic_cast<CinematicCamera*>(myCurrentCamera);
	if (cam != nullptr && cam->GetLookAt() != -1)
	{
		myShouldLookAt = true;
		myLookAtPos = *myCameras[static_cast<CinematicCamera*>(myCurrentCamera)->GetLookAt()]->GetTransform();
	}
	else
	{
		myShouldLookAt = false;
	}
	myDelayStartTime = 0.0f;
	myTempStartTime = 0.0f;
	myIsTransitioning = true;
	myTemporaryCamera = nullptr;
	myIsCurving = true;
	for (size_t i = 0; i < myCameras.size(); i++)
	{
		if (myCameras[i] == someCurvedCameras[0])
		{
			myTemporaryCamera = Instantiate();
			Camera* cameraComponent = myTemporaryCamera->AddComponent<Camera>();

			cameraComponent->SetFoV(myCurrentCamera->GetFoV());
			cameraComponent->SetPostProcessVolume(myCurrentCamera->GetPostProcessingVolume());
			myTemporaryCamera->GetTransform()->SetWorldPos(myCurrentCamera->GetTransform()->worldPos());
			myTemporaryCamera->GetTransform()->SetWorldRot(myCurrentCamera->GetTransform()->worldRot());
			myTemporaryCamera->SetName("Temporary camera (DON'T CLICK)");

			//myTemporaryCamera = myCurrentCamera->GetGameObject().Duplicate();
			//myTemporaryCamera->SetName("Temporary camera (DON'T CLICK)");

			myTemporaryCameraOrigin.fov = myTemporaryCamera->GetComponent<Camera>()->GetFoV();
			myTemporaryCameraOrigin.offset = myTemporaryCamera->GetTransform()->worldPos();
			myTemporaryCameraOrigin.rotation = myTemporaryCamera->GetTransform()->worldRot();

			myCurrentCamera = someCurvedCameras[3];

			mySecondsToTransition = someSeconds;
			Engine::GetInstance()->SetActiveCamera(myTemporaryCamera->GetComponent<Camera>());

			for (size_t j = 0; j < 4; j++)
			{
				myCurve[j] = someCurvedCameras[j]->GetTransform()->worldPos();
			}
		}
	}


}

int CameraController::GetCameraId(Camera* aCamera)
{
	for (size_t i = 0; i < myCameras.size(); i++)
	{
		if (myCameras[i] == aCamera)
		{
			return i;
		}
	}
	return -1;
}

float CameraController::LerpFoV()
{
	myTemporaryCamera->GetComponent<Camera>()->SetFoV(Catbox::Lerp(myTemporaryCameraOrigin.fov, myCurrentCamera->GetFoV(), myTempStartTime / mySecondsToTransition));

	float diff = myCurrentCamera->GetFoV() - myTemporaryCamera->GetComponent<Camera>()->GetFoV();

	return abs(diff);
}

float CameraController::LerpPosition()
{
	Vector3f cameraPos = myCurrentCamera->GetTransform()->worldPos();
	Vector3f newPos;
	newPos.x = Catbox::Lerp(myTemporaryCameraOrigin.offset.x, cameraPos.x, myTempStartTime / mySecondsToTransition);
	newPos.y = Catbox::Lerp(myTemporaryCameraOrigin.offset.y, cameraPos.y, myTempStartTime / mySecondsToTransition);
	newPos.z = Catbox::Lerp(myTemporaryCameraOrigin.offset.z, cameraPos.z, myTempStartTime / mySecondsToTransition);

	myTemporaryCamera->GetTransform()->SetWorldPos(newPos);

	float diff = (cameraPos - myTemporaryCamera->GetTransform()->worldPos()).Length();

	return diff;
}

float CameraController::LerpRotation()
{
	Catbox::Quaternion targetQuat = Catbox::Quaternion::ToQuaternion(myCurrentCamera->GetTransform()->worldRot());
	Catbox::Quaternion currectQuat = Catbox::Quaternion::ToQuaternion(myTemporaryCameraOrigin.rotation);
	physx::PxQuat targetPXQuat(targetQuat.x, targetQuat.y, targetQuat.z, targetQuat.w);
	physx::PxQuat currectPXQuat(currectQuat.x, currectQuat.y, currectQuat.z, currectQuat.w);
	physx::PxQuat myTargetRotation = physx::PxSlerp(myTempStartTime / mySecondsToTransition, currectPXQuat, targetPXQuat);

	Vector3f newRot = PhysXUtilities::quaternion2Euler(myTargetRotation);

	myTemporaryCamera->GetTransform()->SetWorldRot(newRot);

	float diff = abs(myTempStartTime - mySecondsToTransition);

	return diff;
}
