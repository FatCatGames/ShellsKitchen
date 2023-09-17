#include "Game.pch.h"
#include "RayPickerComponent.h"
#include "..\..\Catbox\Components\3D\Camera.h" 
#include "Managers\GameManager.h"
#include "..\Catbox\GameObjects\GameObject.h"

void RayPickerComponent::Init()
{
	GameManager::GetInstance()->SetRayPicker(*this);
}

void RayPickerComponent::Awake()
{
	GameManager::GetInstance()->SetRayPicker(*this);
}

void RayPickerComponent::Update()
{
}


void RayPickerComponent::RunInEditor()
{
}

Catbox::Vector3<float> RayPickerComponent::GetMouseWorldPos(Vector2i aMousePos, float anYvalue)
{
	//https://stackoverflow.com/questions/71731722/correct-way-to-generate-3d-world-ray-from-2d-mouse-coordinates

	const Catbox::Vector2<int> test0 = Engine::GetInstance()->ViewportToScreenPos(static_cast<int>(aMousePos.x), static_cast<int>(aMousePos.y));

	int winWidth = DX11::GetResolution().x;
	int winHeight = DX11::GetResolution().y;
	const float normalised_x = 2 * test0.x / static_cast<float>(winWidth) - 1;
	const float normalised_y = 1 - 2 * test0.y / static_cast<float>(winHeight);

	const Catbox::Vector4<float> clipCoordsNear = { normalised_x, normalised_y , 0, 1 }; //Nearplane

	const Catbox::Matrix4x4<float> invereprojection = Catbox::Matrix4x4<float>::GetFastInverse(myGameObject->GetComponent<Camera>()->GetProjectionMatrix());
	const Catbox::Matrix4x4<float> inverseViewMatrix = myGameObject->GetTransform()->GetWorldTransformMatrix();

	//convert from 2d to a 3d direction
	const Catbox::Vector4<float> clipCoordsInCameraSpaceNear = clipCoordsNear * invereprojection;
	const Catbox::Vector4<float> clipCoordsInCameraSpaceNearBackup = { clipCoordsInCameraSpaceNear.x , clipCoordsInCameraSpaceNear.y , 1, 0 };
	const Catbox::Vector4<float> clipCoordsInWorldSpaceNear = clipCoordsInCameraSpaceNearBackup * inverseViewMatrix;
	Catbox::Vector3<float> RayDirectionNear = { clipCoordsInWorldSpaceNear.x, clipCoordsInWorldSpaceNear.y, clipCoordsInWorldSpaceNear.z };

	RayDirectionNear.Normalize();

	const Catbox::Vector3<float> CameraPos = myGameObject->GetTransform()->worldPos();

	const float multiplierNear = (anYvalue - CameraPos.y) / RayDirectionNear.y;

	const Catbox::Vector3<float> ClickPosNear = CameraPos + (RayDirectionNear * multiplierNear);

	return ClickPosNear;
}

Catbox::Vector3<float> RayPickerComponent::GetClickDirection(Vector2i aMousePos)
{
	Catbox::Vector2<int> test0 = Engine::GetInstance()->ViewportToScreenPos(static_cast<int>(aMousePos.x), static_cast<int>(aMousePos.y));

	int winWidth = DX11::GetResolution().x;
	int winHeight = DX11::GetResolution().y;
	if (EDITORMODE)
	{
		int winWidth = 1884;
		int winHeight = 1042;
	}

	const float normalised_x = 2 * test0.x / static_cast<float>(winWidth) - 1;
	const float normalised_y = 1 - 2 * test0.y / static_cast<float>(winHeight);

	const Catbox::Vector4<float> clipCoordsNear = { normalised_x, normalised_y , 0, 1 }; //Nearplane

	const Catbox::Matrix4x4<float> invereprojection = Catbox::Matrix4x4<float>::GetFastInverse(myGameObject->GetComponent<Camera>()->GetProjectionMatrix());
	const Catbox::Matrix4x4<float> inverseViewMatrix = myGameObject->GetTransform()->GetWorldTransformMatrix();

	const Catbox::Vector4<float> clipCoordsInCameraSpaceNear = clipCoordsNear * invereprojection;
	const Catbox::Vector4<float> clipCoordsInCameraSpaceNearBackup = { clipCoordsInCameraSpaceNear.x , clipCoordsInCameraSpaceNear.y , 1, 0 };
	const Catbox::Vector4<float> clipCoordsInWorldSpaceNear = clipCoordsInCameraSpaceNearBackup * inverseViewMatrix;
	Catbox::Vector3<float> RayDirectionNear = { clipCoordsInWorldSpaceNear.x, clipCoordsInWorldSpaceNear.y, clipCoordsInWorldSpaceNear.z };

	RayDirectionNear.Normalize();

	return RayDirectionNear;
}