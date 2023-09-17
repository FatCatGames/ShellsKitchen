#include "stdafx.h"
#include "Camera.h"
#include "../CommonUtilities/UtilityFunctions.hpp"
#include "Components/3D/ModelInstance.h"
#include "Physics/Collisions/CollisionManager.h"
#include "Physics/Collisions/Collider.h"
#include "Assets/Model.h"
#include "CameraController.h"
#include "CommonUtilities\Sphere.hpp"

Camera::Camera()
{
	myShouldUpdateWhilePaused = true;

	CreateProjectionMatrix();
	CreateRenderTexture();

	myOnResolutionChangedListener.action = [this]
	{
		CreateProjectionMatrix();
		CreateRenderTexture();
	};

	DX11::AddResolutionChangedListener(myOnResolutionChangedListener);
}

Camera::Camera(const Camera& aCamera)
{
	myProjectionMatrix = aCamera.myProjectionMatrix;
	myFoV = aCamera.myFoV;
	myNearPlane = aCamera.myNearPlane;
	myFarPlane = aCamera.myFarPlane;

	myFrustum = aCamera.myFrustum;
	myXRotationRadians = aCamera.myXRotationRadians;
	myYRotationRadians = aCamera.myYRotationRadians;

	myPostProcessingVolume = aCamera.myPostProcessingVolume;

	myRenderTexture = aCamera.myRenderTexture;
	myDepthStencil = aCamera.myDepthStencil;
	myOnResolutionChangedListener = aCamera.myOnResolutionChangedListener;
	myCameraSettings = aCamera.myCameraSettings;
}

Camera::~Camera()
{
	DX11::RemoveResolutionChangedListener(myOnResolutionChangedListener);
	if (Engine::GetInstance()->GetActiveCamera() == this)
	{
		if (Engine::GetInstance()->GetMainCamera() != nullptr && Engine::GetInstance()->GetMainCamera() != this)
		{
			Engine::GetInstance()->SetActiveCamera(Engine::GetInstance()->GetMainCamera());
		}
		else
		{
			if (Engine::GetInstance()->GetCameraController()->GetCameras().size() > 0)
			{
				Engine::GetInstance()->SetActiveCamera(Engine::GetInstance()->GetCameraController()->GetCameras()[0]);
			}
			else
			{
				printerror("No Camera left in scene");
			}
		}
	}

	if (Engine::GetInstance()->GetCameraController() != nullptr)
	{
		Engine::GetInstance()->GetCameraController()->RemoveCamera(this);
	}
}

void Camera::Awake()
{
	if (!Engine::GetInstance()->GetMainCamera())
	{
		Engine::GetInstance()->SetMainCamera(this);
	}

	if (!Engine::GetInstance()->GetActiveCamera())
	{
		Engine::GetInstance()->SetActiveCamera(this);
	}

	Engine::GetInstance()->GetCameraController()->AddCamera(this);
}

void Camera::Init()
{
	Engine::GetInstance()->GetCameraController()->AddCamera(this);
}

void Camera::CreateProjectionMatrix()
{
	const float fovRad = myFoV * (3.14f / 180);
	const float vFoVRad = 2 * std::atan(std::tan(fovRad / 2) * (static_cast<float>(DX11::GetResolution().y) / static_cast<float>(DX11::GetResolution().x)));
	const float myXScale = 1 / std::tanf(fovRad * 0.5f);
	const float myYScale = 1 / std::tanf(vFoVRad * 0.5f);
	const float Q = myFarPlane / (myFarPlane - myNearPlane);

	myProjectionMatrix(1, 1) = myXScale;
	myProjectionMatrix(2, 2) = myYScale;
	myProjectionMatrix(3, 3) = Q;
	myProjectionMatrix(3, 4) = 1.0f / Q;
	myProjectionMatrix(4, 3) = -Q * myNearPlane;
	myProjectionMatrix(4, 4) = 0.0f;
}

void Camera::OnTransformChanged()
{
	myFrustum = CreateFrustum();
}


void Camera::CreateRenderTexture()
{
	myRenderTexture.CreateEmptyTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, DX11::GetResolution().x, DX11::GetResolution().y);
	myRenderTexture.CreateRenderTargetView();
	myDepthStencil.CreateDepthStencil(DX11::GetResolution().x, DX11::GetResolution().y);
}

void Camera::Save(rapidjson::Value& aComponentData)
{
	int version = 3;
	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveValue<DataType::Float>("FoV", myFoV);
	wrapper.SaveValue<DataType::Float>("NearPlane", myNearPlane);
	wrapper.SaveValue<DataType::Float>("FarPlane", myFarPlane);

	if (myPostProcessingVolume)
	{
		auto volume = wrapper.CreateObject();
		myPostProcessingVolume->Save(volume);
		wrapper.SetTarget(aComponentData);
		wrapper.SaveObject("Post Processing Volume", volume);
	}
}

void Camera::Load(rapidjson::Value& aComponentData)
{
	int version = aComponentData["Version"].GetInt();

	myFoV = aComponentData["FoV"].GetFloat();
	myNearPlane = aComponentData["NearPlane"].GetFloat();
	myFarPlane = aComponentData["FarPlane"].GetFloat();

	if (version == 3)
	{
		if (aComponentData.HasMember("Post Processing Volume"))
		{
			myPostProcessingVolume = std::make_shared<PostProcessingVolume>();
			myPostProcessingVolume->Load(aComponentData["Post Processing Volume"].GetObj());
		}
	}
	else
	{
		printerror("Wrong json version on camera");
	}
}

Vector3f Camera::ToCameraSpace(const Vector3f& aPosition)
{
	Vector4f cameraSpace = Vector4f(aPosition.x, aPosition.y, aPosition.z, 1) * Catbox::Matrix4x4<float>::GetFastInverse(GetTransform()->toWorldSpace());
	return Vector3f(cameraSpace.x, cameraSpace.y, cameraSpace.z);
}


Catbox::Matrix4x4<float>& Camera::GetProjectionMatrix()
{
	return myProjectionMatrix;
}

Vector4f Camera::ToProjectionSpace(const Vector3f& aPosition)
{
	return Vector4f(aPosition.x, aPosition.y, aPosition.z, 1) * myProjectionMatrix;
}

std::shared_ptr<PostProcessingVolume> Camera::GetPostProcessVolume()
{
	return myPostProcessingVolume;
}

void Camera::SetPostProcessVolume(std::shared_ptr<PostProcessingVolume> aPostProcessVolume)
{
	myPostProcessingVolume = aPostProcessVolume;
}

void Camera::RotateAroundObject(Transform& aTransform, const float someDegreesX, const float someDegreesY, const Vector2f& aYClamp, const Vector3f& anOffset, const bool shouldLookAtObject)
{
	RotateAroundObject(aTransform.worldPos(), someDegreesX, someDegreesY, aYClamp, anOffset, shouldLookAtObject);
}

void Camera::RotateAroundObject(const Vector3f& aPosition, const float someDegreesX, const float someDegreesY, const Vector2f& aYClamp, const Vector3f& anOffset, const bool shouldLookAtObject)
{
	myXRotationRadians -= someDegreesX * deltaTime;

	myYRotationRadians -= someDegreesY * deltaTime;
	myYRotationRadians = Catbox::Clamp<float>(myYRotationRadians, aYClamp.x, aYClamp.y);

	float xRotation = sinf(myXRotationRadians) * cosf(myYRotationRadians) * anOffset.z;
	float yRotation = sinf(myYRotationRadians) * anOffset.y;
	float zRotation = cosf(myXRotationRadians) * cosf(myYRotationRadians) * anOffset.z;

	myTransform->SetWorldPos(aPosition + Vector3f{ xRotation, yRotation, zRotation });
	if (shouldLookAtObject == true) { myTransform->LookAt(aPosition); }
}

void Camera::RunInEditor()
{
	if (!myGameObject->IsSelected(0)) return;
	const float aspectRatio = Engine::GetInstance()->GetWindowRatio();
	const float farPlane = myFarPlane;
	const float halfHeight = farPlane * tanf(Catbox::Deg2Rad(0.5f * myFoV));
	const float halfWidth = aspectRatio * halfHeight;
	const float nearHalfHeight = myNearPlane * tanf(Catbox::Deg2Rad(0.5f * myFoV));
	const float nearHalfWidth = aspectRatio * nearHalfHeight;

	auto& matrix = myTransform->GetWorldTransformMatrix();
	Vector4f farBottomLeftV4 = Vector4f(-halfWidth, -halfHeight, farPlane, 0.0f) * matrix;
	Vector4f farTopLeftV4 = Vector4f(-halfWidth, halfHeight, farPlane, 0.0f) * matrix;
	Vector4f farTopRightV4 = Vector4f(halfWidth, halfHeight, farPlane, 0.0f) * matrix;
	Vector4f farBottomRightV4 = Vector4f(halfWidth, -halfHeight, farPlane, 0.0f) * matrix;

	Vector4f nearBottomLeftV4 = Vector4f(-nearHalfWidth, -nearHalfHeight, myNearPlane, 0.0f) * matrix;
	Vector4f nearTopLeftV4 = Vector4f(-nearHalfWidth, nearHalfHeight, myNearPlane, 0.0f) * matrix;
	Vector4f nearTopRightV4 = Vector4f(nearHalfWidth, nearHalfHeight, myNearPlane, 0.0f) * matrix;
	Vector4f nearBottomRightV4 = Vector4f(nearHalfWidth, -nearHalfHeight, myNearPlane, 0.0f) * matrix;


	Vector3f farBottomLeftV3 = Vector3f(farBottomLeftV4.x, farBottomLeftV4.y, farBottomLeftV4.z);
	Vector3f farTopLeftV3 = Vector3f(farTopLeftV4.x, farTopLeftV4.y, farTopLeftV4.z);
	Vector3f farTopRightV3 = Vector3f(farTopRightV4.x, farTopRightV4.y, farTopRightV4.z);
	Vector3f farBottomRightV3 = Vector3f(farBottomRightV4.x, farBottomRightV4.y, farBottomRightV4.z);

	Vector3f nearBottomLeftV3 = Vector3f(nearBottomLeftV4.x, nearBottomLeftV4.y, nearBottomLeftV4.z);
	Vector3f nearTopLeftV3 = Vector3f(nearTopLeftV4.x, nearTopLeftV4.y, nearTopLeftV4.z);
	Vector3f nearTopRightV3 = Vector3f(nearTopRightV4.x, nearTopRightV4.y, nearTopRightV4.z);
	Vector3f nearBotttomRightV3 = Vector3f(nearBottomRightV4.x, nearBottomRightV4.y, nearBottomRightV4.z);
	auto& worldPos = myTransform->worldPos();

	DebugDrawer::DrawLine(nearBottomLeftV3 + worldPos, farBottomLeftV3.GetNormalized() * 10.0f + worldPos);
	DebugDrawer::DrawLine(nearTopLeftV3 + worldPos, farTopLeftV3.GetNormalized() * 10.0f + worldPos);
	DebugDrawer::DrawLine(nearTopRightV3 + worldPos, farTopRightV3.GetNormalized() * 10.0f + worldPos);
	DebugDrawer::DrawLine(nearBotttomRightV3 + worldPos, farBottomRightV3.GetNormalized() * 10.0f + worldPos);

	DebugDrawer::DrawLine(nearBotttomRightV3 + worldPos, nearBottomLeftV3 + worldPos);
	DebugDrawer::DrawLine(nearBotttomRightV3 + worldPos, nearTopRightV3 + worldPos);
	DebugDrawer::DrawLine(nearBottomLeftV3 + worldPos, nearTopLeftV3 + worldPos);
	DebugDrawer::DrawLine(nearTopLeftV3 + worldPos, nearTopRightV3 + worldPos);

	DebugDrawer::DrawLine(farBottomLeftV3.GetNormalized() * 10.0f + worldPos, farBottomRightV3.GetNormalized() * 10.0f + worldPos);
	DebugDrawer::DrawLine(farTopLeftV3.GetNormalized() * 10.0f + worldPos, farTopRightV3.GetNormalized() * 10.0f + worldPos);
	DebugDrawer::DrawLine(farTopRightV3.GetNormalized() * 10.0f + worldPos, farBottomRightV3.GetNormalized() * 10.0f + worldPos);
	DebugDrawer::DrawLine(farTopLeftV3.GetNormalized() * 10.0f + worldPos, farBottomLeftV3.GetNormalized() * 10.0f + worldPos);
}

void Camera::Update()
{
	if (Engine::GetInstance()->GetActiveCamera() == this)
	{
		//CreateProjectionMatrix();
	}
}

std::shared_ptr<PostProcessingVolume> Camera::GetPostProcessingVolume()
{
	return myPostProcessingVolume;
}

void Camera::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto& cameras = ComponentVectorCast<Camera>(aComponentList);

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

	if (Catbox::DragFloat("Near plane", &myNearPlane, 0.1f, 0.01f, 100))
	{
		for (auto& cam : cameras)
		{
			cam->myNearPlane = myNearPlane;
			cam->CreateProjectionMatrix();
		}
	}

	if (Catbox::DragFloat("Far plane", &myFarPlane, 10.0f, 1, 100000))
	{
		for (auto& cam : cameras)
		{
			cam->myFarPlane = myFarPlane;
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
	else if (sharedPPV)
	{
		myPostProcessingVolume->RenderInProperties();

		if (ImGui::Button("Remove Volume"))
		{
			myPostProcessingVolume = nullptr;
		}
	}
}


bool Camera::IsPostProcessingEnabled()
{
	if (!myPostProcessingVolume) return false;
	return myPostProcessingVolume->IsEnabled();
}

void Camera::AddCameraSettingsTemplate(CameraSettingsTemplate aCameraSettingsTemplate)
{
	myCameraSettings.push_back(aCameraSettingsTemplate);
}

//Only used for the editor camera
void Camera::OverrideTransform(Transform& aTransform)
{
	delete myTransform;
	myTransform = &aTransform;
}

Camera::Frustum Camera::CreateFrustum()
{
	Frustum frustum;
	if (this != nullptr)
	{
		Vector3f myPos = myTransform->worldPos();
		const float aspectRatio = DX11::GetAspectRatio();
		const float farPlane = myFarPlane;
		const float halfHeight = farPlane * tanf(Catbox::Deg2Rad(0.5f * myFoV));
		const float halfWidth = aspectRatio * halfHeight;
		const float nearHalfHeight = myNearPlane * tanf(Catbox::Deg2Rad(0.5f * myFoV));
		const float nearHalfWidth = aspectRatio * nearHalfHeight;

		auto& matrix = myTransform->GetWorldTransformMatrix();
		Vector4f farBottomLeftV4 = Vector4f(-halfWidth, -halfHeight, farPlane, 0.0f) * matrix;
		Vector4f farTopLeftV4 = Vector4f(-halfWidth, halfHeight, farPlane, 0.0f) * matrix;
		Vector4f farTopRightV4 = Vector4f(halfWidth, halfHeight, farPlane, 0.0f) * matrix;
		Vector4f farBottomRightV4 = Vector4f(halfWidth, -halfHeight, farPlane, 0.0f) * matrix;

		Vector4f nearBottomLeftV4 = Vector4f(-nearHalfWidth, -nearHalfHeight, myNearPlane, 0.0f) * matrix;
		Vector4f nearTopLeftV4 = Vector4f(-nearHalfWidth, nearHalfHeight, myNearPlane, 0.0f) * matrix;
		Vector4f nearTopRightV4 = Vector4f(nearHalfWidth, nearHalfHeight, myNearPlane, 0.0f) * matrix;
		Vector4f nearBottomRightV4 = Vector4f(nearHalfWidth, -nearHalfHeight, myNearPlane, 0.0f) * matrix;

		Vector3f farBottomLeftV3 = Vector3f(farBottomLeftV4.x, farBottomLeftV4.y, farBottomLeftV4.z);
		Vector3f farTopLeftV3 = Vector3f(farTopLeftV4.x, farTopLeftV4.y, farTopLeftV4.z);
		Vector3f farTopRightV3 = Vector3f(farTopRightV4.x, farTopRightV4.y, farTopRightV4.z);
		Vector3f farBottomRightV3 = Vector3f(farBottomRightV4.x, farBottomRightV4.y, farBottomRightV4.z);

		Vector3f nearBottomLeftV3 = Vector3f(nearBottomLeftV4.x, nearBottomLeftV4.y, nearBottomLeftV4.z);
		Vector3f nearTopLeftV3 = Vector3f(nearTopLeftV4.x, nearTopLeftV4.y, nearTopLeftV4.z);
		Vector3f nearTopRightV3 = Vector3f(nearTopRightV4.x, nearTopRightV4.y, nearTopRightV4.z);
		Vector3f nearBotttomRightV3 = Vector3f(nearBottomRightV4.x, nearBottomRightV4.y, nearBottomRightV4.z);



		frustum.topFace.InitWith3Points(nearTopLeftV3 + myPos, nearTopRightV3 + myPos, farTopLeftV3 + myPos);
		frustum.bottomFace.InitWith3Points(nearBotttomRightV3 + myPos, nearBottomLeftV3 + myPos, farBottomLeftV3 + myPos);
		frustum.leftFace.InitWith3Points(nearBottomLeftV3 + myPos, nearTopLeftV3 + myPos, farBottomLeftV3 + myPos);
		frustum.rightFace.InitWith3Points(nearTopRightV3 + myPos, nearBotttomRightV3 + myPos, farBottomRightV3 + myPos);
		frustum.nearFace.InitWith3Points(nearTopRightV3 + myPos, nearTopLeftV3 + myPos, nearBotttomRightV3 + myPos);
		frustum.farFace.InitWith3Points(farTopLeftV3 + myPos, farTopRightV3 + myPos, farBottomLeftV3 + myPos);
	}
	myFrustum = frustum;
	return frustum;
}

Vector3f Camera::MouseToWorldPos(Vector2i aMousePos, float anYPos)
{
	Catbox::Vector2<int> screenPos = Engine::GetInstance()->ViewportToScreenPos(static_cast<int>(aMousePos.x), static_cast<int>(aMousePos.y));

	int winWidth = DX11::GetResolution().x;
	int winHeight = DX11::GetResolution().y;

	float normalised_x = 2 * screenPos.x / static_cast<float>(winWidth) - 1;
	float normalised_y = 1 - 2 * screenPos.y / static_cast<float>(winHeight);

	Catbox::Vector4<float> clipCoordsNear = { normalised_x, normalised_y , 0, 1 }; //Nearplane

	Catbox::Matrix4x4<float> invereprojection = Catbox::Matrix4x4<float>::GetFastInverse(GetProjectionMatrix());
	Catbox::Matrix4x4<float> inverseViewMatrix = myTransform->GetWorldTransformMatrix();

	//convert from 2d to a 3d direction
	Catbox::Vector4<float> clipCoordsInCameraSpaceNear = clipCoordsNear * invereprojection;

	Catbox::Vector4<float> clipCoordsInCameraSpaceNearBackup = { clipCoordsInCameraSpaceNear.x , clipCoordsInCameraSpaceNear.y , 1, 0 };

	Catbox::Vector4<float> clipCoordsInWorldSpaceNear = clipCoordsInCameraSpaceNearBackup * inverseViewMatrix;

	Catbox::Vector3<float> RayDirectionNear = { clipCoordsInWorldSpaceNear.x, clipCoordsInWorldSpaceNear.y, clipCoordsInWorldSpaceNear.z };

	RayDirectionNear.Normalize();

	Catbox::Vector3<float> CameraPos = myTransform->worldPos();

	float multiplierNear = (anYPos - CameraPos.y) / RayDirectionNear.y;

	Catbox::Vector3<float> ClickPosNear = CameraPos + (RayDirectionNear * multiplierNear);
	ClickPosNear.y = 0;

	return ClickPosNear;
}

Vector3f Camera::MouseToWorldPos(Vector2i aMousePos)
{
	Vector2i mousePos = Engine::GetInstance()->ViewportToScreenPos(aMousePos);
	float mouseX = mousePos.x / static_cast<float>(DX11::GetResolution().x);
	float mouseY = mousePos.y / static_cast<float>(DX11::GetResolution().y);

	auto matInv = Catbox::Matrix4x4<float>::GetFastInverse(Catbox::Matrix4x4<float>::GetFastInverse(myTransform->GetWorldTransformMatrix()) * myProjectionMatrix);
	Vector4f rayTarget = Vector4f(mouseX * 2 - 1, 1 - mouseY * 2, 1, 1) * matInv;
	rayTarget /= rayTarget.w;

	Vector4f rayOrigin = Vector4f(mouseX * 2 - 1, 1 - mouseY * 2, 0, 1) * matInv;
	rayOrigin /= rayOrigin.w;

	Vector4f rayDir4 = (rayTarget - rayOrigin).GetNormalized();
	Vector3f rayDir = Vector3f(rayDir4.x, rayDir4.y, rayDir4.z);


	return myTransform->worldPos() + rayDir * 5.f;
}

Vector3f Camera::GetMouseClickDirection(Vector2i aMousePos)
{
	Catbox::Vector2<int> test0 = Engine::GetInstance()->ViewportToScreenPos(static_cast<int>(aMousePos.x), static_cast<int>(aMousePos.y));

	int winWidth = 1884;
	int winHeight = 1042;
	float normalised_x = 2 * test0.x / static_cast<float>(winWidth) - 1;
	float normalised_y = 1 - 2 * test0.y / static_cast<float>(winHeight);

	Catbox::Vector4<float> clipCoordsNear = { normalised_x, normalised_y , 0, 1 }; //Nearplane
	Catbox::Matrix4x4<float> invereprojection = Catbox::Matrix4x4<float>::GetFastInverse(myProjectionMatrix);
	Catbox::Matrix4x4<float> inverseViewMatrix = myTransform->GetWorldTransformMatrix();

	Catbox::Vector4<float> clipCoordsInCameraSpaceNear = clipCoordsNear * invereprojection;

	Catbox::Vector4<float> clipCoordsInCameraSpaceNearBackup = { clipCoordsInCameraSpaceNear.x , clipCoordsInCameraSpaceNear.y , 1, 0 };

	Catbox::Vector4<float> clipCoordsInWorldSpaceNear = clipCoordsInCameraSpaceNearBackup * inverseViewMatrix;

	Catbox::Vector3<float> RayDirectionNear = { clipCoordsInWorldSpaceNear.x, clipCoordsInWorldSpaceNear.y, clipCoordsInWorldSpaceNear.z };

	RayDirectionNear.Normalize();

	return RayDirectionNear;
}


bool Camera::IsInsideFrustum(Transform* aTransfrom)
{
	if (!aTransfrom->GetGameObject()->MarkedForDestroy())
	{
		const Vector3f globalScale = aTransfrom->worldScale();
		float boundsRadius = aTransfrom->GetBoundsRadius();
		if (boundsRadius < 2.0f)
		{
			boundsRadius = 2.0f;
		}
		boundsRadius *= 2.0f;
		const float maxScale = std::max<float>(std::max<float>(globalScale.x, globalScale.y), globalScale.z) * boundsRadius;
		Vector3f globalVector3 = aTransfrom->GetCenter();

		Catbox::Sphere globalSphere
		(globalVector3, maxScale);

		if (myFrustum.nearFace.IsSphereInFront(globalVector3, globalSphere.radius) &&
			myFrustum.farFace.IsSphereInFront(globalVector3, globalSphere.radius) &&
			myFrustum.topFace.IsSphereInFront(globalVector3, globalSphere.radius) &&
			myFrustum.bottomFace.IsSphereInFront(globalVector3, globalSphere.radius) &&
			myFrustum.rightFace.IsSphereInFront(globalVector3, globalSphere.radius) &&
			myFrustum.leftFace.IsSphereInFront(globalVector3, globalSphere.radius))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}