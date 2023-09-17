#include "stdafx.h"
#include "CinematicCamera.h"
#include "CameraController.h"

CinematicCamera::CinematicCamera()
{

}

CinematicCamera::~CinematicCamera()
{
}

void CinematicCamera::RunInEditor()
{
	Camera::RunInEditor();

	if (myGameObject->IsSelected(0))
	{
		DebugDrawer::DrawSphere(myTransform->worldPos(), 1.0f, { 0.0f, 1.0f, 0.0f, 1.0f }, 30);

		int startCamera = Engine::GetInstance()->GetCameraController()->GetCameraId(this);
		int currentCamera = startCamera;;
		int nextCamera = myNextCamera;

		vector<Camera*> cameras = Engine::GetInstance()->GetCameraController()->GetCameras();
		int recursiveFailSafeCounter = 0;
		bool shouldUseCurve = myUseCurve;

		while (currentCamera != -1 && nextCamera != -1)
		{
			if (shouldUseCurve)
			{
				vector<Camera*> cameras = Engine::GetInstance()->GetCameraController()->GetCameras();
				std::array<Vector3f, 4> curve;
				curve[0] = myTransform->worldPos();


				bool functionalCurve = true;

				for (size_t i = 0; i < 3; i++)
				{
					if (nextCamera == -1)
					{
						functionalCurve = false;
						break;
					}
					curve[i + 1] = cameras[nextCamera]->GetTransform()->worldPos();
					currentCamera = nextCamera;
					nextCamera = static_cast<CinematicCamera*>(cameras[nextCamera])->GetNextCamera();
					if (nextCamera < 0 || nextCamera >= cameras.size()) { break; }
				}

				DebugDrawer::DrawCurve(curve, { 0.0, 1.0f, 0.0f, 1.0f });
				shouldUseCurve = false;
			}

			if (nextCamera < 0 || nextCamera >= cameras.size()) { break; }

			DebugDrawer::DrawLine(cameras[currentCamera]->GetTransform()->worldPos(), cameras[nextCamera]->GetTransform()->worldPos(), { 0.0f, 1.0f, 0.0f, 1.0f });
			currentCamera = nextCamera;
			nextCamera = static_cast<CinematicCamera*>(cameras[nextCamera])->GetNextCamera();
			if (currentCamera == startCamera) { break; }
			if (currentCamera == nextCamera) { break; }
			if (recursiveFailSafeCounter++ > cameras.size()) { break; }
		}
	}
}

void CinematicCamera::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto& cameras = ComponentVectorCast<CinematicCamera>(aComponentList);

	if (Catbox::Checkbox("View through camera", &myIsPreviewing))
	{
		if (myIsPreviewing)
		{
			Engine::GetInstance()->GetGraphicsEngine()->SetCamera(this);
		}
		else
		{
			Engine::GetInstance()->GetGraphicsEngine()->SetCamera(Editor::GetInstance()->GetEditorCamera().GetCamera());
		}
	}

	if (Catbox::DragFloat("Field of View", &myFoV, 0.1f, 45, 120))
	{
		for (auto& cam : cameras)
		{
			cam->myFoV = myFoV;
			cam->CreateProjectionMatrix();
		}
	}

	ImGui::Text("Post Processing");
	std::shared_ptr<PostProcessingVolume> sharedPPV = myPostProcessingVolume;
	for (auto& cam : cameras)
	{
		if (cam->myPostProcessingVolume != sharedPPV)
		{
			sharedPPV = nullptr;
			break;
		}
	}


	if (!myPostProcessingVolume)
	{
		if (ImGui::Button("Add New Volume"))
		{
			myPostProcessingVolume = std::make_shared<PostProcessingVolume>();
		}
	}
	else
	{
		myPostProcessingVolume->RenderInProperties();

		if (ImGui::Button("Remove Volume"))
		{
			myPostProcessingVolume = nullptr;
		}

	}

	auto& cams = Engine::GetInstance()->GetCameraController()->GetCameras();
	static const char* selectedCamera = "Select Camera";
	if (myNextCamera == -1)
	{
		selectedCamera = "Select Camera";
	}
	else
	{
		if (myNextCamera < cams.size())
		{
			selectedCamera = cams[myNextCamera]->GetGameObject().GetName().c_str();
		}
	}

	if (ImGui::BeginCombo("Next Camera", selectedCamera))
	{
		if (ImGui::Selectable("No Camera", false))
		{
			myNextCamera = -1;
			selectedCamera = "No Camera";
		}

		for (size_t i = 0; i < cams.size(); i++)
		{
			if (i != Engine::GetInstance()->GetCameraController()->GetCameraId(this))
			{
				if (ImGui::Selectable(cams[i]->GetGameObject().GetName().c_str(), false))
				{
					myNextCamera = i;
					selectedCamera = cams[i]->GetGameObject().GetName().c_str();
				}
			}
		}
		ImGui::EndCombo();
	}

	Catbox::Checkbox("Use Curve", &myUseCurve);

	if (myNextCamera >= 0)
	{
		Catbox::Checkbox("Instant transition", &myIsInstant);
		if (!myIsInstant)
		{
			Catbox::InputFloat("Delay before next camera", &myDelay);
			Catbox::InputFloat("Transition time in seconds", &mySpeed);

			if (mySpeed > 0.0f)
			{
				static const char* selectedLookAt = "Select Camera";
				if (myLookAtCamera == -1)
				{
					selectedLookAt = "Select Camera";
				}
				else
				{
					if (myLookAtCamera < cams.size())
					{
						selectedLookAt = cams[myLookAtCamera]->GetGameObject().GetName().c_str();
					}
				}

				if (ImGui::BeginCombo("Look At Camera", selectedLookAt))
				{
					if (ImGui::Selectable("No Camera", false))
					{
						myLookAtCamera = -1;
						selectedLookAt = "No Camera";
					}

					for (size_t i = 0; i < cams.size(); i++)
					{
						if (i != Engine::GetInstance()->GetCameraController()->GetCameraId(this))
						{
							if (ImGui::Selectable(cams[i]->GetGameObject().GetName().c_str(), false))
							{
								myLookAtCamera = i;
								selectedLookAt = cams[i]->GetGameObject().GetName().c_str();
							}
						}
					}
					ImGui::EndCombo();
				}
			}
		}
	}
}

void CinematicCamera::Save(rapidjson::Value& aComponentData)
{
	int version = 6;
	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveValue<DataType::Float>("FoV", myFoV);
	wrapper.SaveValue<DataType::Float>("NearPlane", myNearPlane);
	wrapper.SaveValue<DataType::Float>("FarPlane", myFarPlane);

	wrapper.SaveValue<DataType::Int>("NextCamera", myNextCamera);
	wrapper.SaveValue<DataType::Int>("LookAt", myLookAtCamera);
	wrapper.SaveValue<DataType::Bool>("Instant", myIsInstant);
	wrapper.SaveValue<DataType::Bool>("Curve", myUseCurve);
	wrapper.SaveValue<DataType::Float>("Delay", myDelay);
	wrapper.SaveValue<DataType::Float>("Speed", mySpeed);

	if (myPostProcessingVolume)
	{
		auto volume = wrapper.CreateObject();
		myPostProcessingVolume->Save(volume);
		wrapper.SetTarget(aComponentData);
		wrapper.SaveObject("Post Processing Volume", volume);
	}
}

void CinematicCamera::Load(rapidjson::Value& aComponentData)
{
	int version = aComponentData["Version"].GetInt();

	myFoV = aComponentData["FoV"].GetFloat();
	myNearPlane = aComponentData["NearPlane"].GetFloat();
	myFarPlane = aComponentData["FarPlane"].GetFloat();

	if (version == 6)
	{
		if (aComponentData.HasMember("Post Processing Volume"))
		{
			myPostProcessingVolume = std::make_shared<PostProcessingVolume>();
			myPostProcessingVolume->Load(aComponentData["Post Processing Volume"].GetObj());
		}

		myUseCurve = aComponentData["Curve"].GetBool();
		myNextCamera = aComponentData["NextCamera"].GetInt();
		myLookAtCamera = aComponentData["LookAt"].GetInt();
		myIsInstant = aComponentData["Instant"].GetBool();
		myDelay = aComponentData["Delay"].GetFloat();
		mySpeed = aComponentData["Speed"].GetFloat();
	}
	else
	{
		printerror("Wrong json version on camera");
	}
}
