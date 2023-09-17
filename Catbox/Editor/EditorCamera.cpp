#include "stdafx.h"
#include "EditorCamera.h"
#include "CommonUtilities/InputHandler.h"
#include "Components/3D/Camera.h"
#include "GameObjects/Transform.h"
#include <UtilityFunctions.hpp>
#include "Graphics\Rendering\DX11\DX11.h"
#include "Windows\PropertiesWindow.h"
#include "Windows\SceneHierarchyWindow.h"
#include <ImGuizmo.h>
#include "Windows/SceneWindow.h"

EditorCamera::EditorCamera()
{
	myCamera.OverrideTransform(myTransform);
	myTransform.SetWorldPos({ 0, 0.7f, 1.5f });
	myTransform.SetLocalRot({ 20, 180, 0 });
	myCamera.myPostProcessingVolume = std::make_shared<PostProcessingVolume>();
	Engine::GetInstance()->GetGraphicsEngine()->SetCamera(&myCamera);

	if (Engine::GetInstance()->GetMainCamera() == &myCamera || Engine::GetInstance()->GetMainCamera() == &myUICamera)
	{
		Engine::GetInstance()->SetMainCamera(nullptr);
	}

	myUICamera.OverrideTransform(myUICameraTransform);
	myUICameraTransform.SetLocalPos(0, 0, -1);
}

void EditorCamera::ScrollZoom()
{
	myMoveSpeed += myMoveSpeed * Input::GetMouseScroll() * 0.1f;
	myMoveSpeed = Catbox::Clamp(myMoveSpeed, 0.1f, 100.f);
	Editor::GetInstance()->GetUserPrefs().myCamMoveSpeed = myMoveSpeed;
}

void EditorCamera::Movement()
{
	if (Input::GetKeyHeld(KeyCode::MOUSERIGHT) && !ImGui::GetIO().WantCaptureKeyboard)
	{
		float forward = 0;
		float right = 0;
		float up = 0;
		forward += (Input::GetKeyHeld(KeyCode::W));
		forward -= (Input::GetKeyHeld(KeyCode::S));
		right += (Input::GetKeyHeld(KeyCode::D));
		right -= (Input::GetKeyHeld(KeyCode::A));
		up -= (Input::GetKeyHeld(KeyCode::Q));
		up += (Input::GetKeyHeld(KeyCode::E));

		if (forward != 0 || right != 0 || up != 0) 
		{
			Vector3f rightVector = myCamera.GetTransform()->right() * right * myMoveSpeed * realDeltaTime;
			Vector3f forwardVector = myCamera.GetTransform()->forward() * forward * myMoveSpeed * realDeltaTime;
			Vector3f upVector = Vector3f::up() * up * myMoveSpeed * realDeltaTime;
			myCamera.GetTransform()->Translate(rightVector + forwardVector + upVector);
		}
	}
}

void EditorCamera::Rotate()
{
	if (Input::GetKeyHeld(KeyCode::MOUSERIGHT) && Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneWindow>(WindowType::Scene)->IsHovered())
	{
		Vector2i delta = Input::GetMouseDelta();
		if (delta.x != 0 || delta.y != 0) 
		{
			myCamera.OnTransformChanged();
			Vector3f rot = myTransform.localRot();
			rot.x += delta.y * myRotateSpeed * realDeltaTime;
			rot.y += delta.x * myRotateSpeed * realDeltaTime;
			rot.x = Catbox::Clamp(rot.x, -90.f, 90.f);
			rot.z = 0;
			myTransform.SetLocalRot(rot);
		}
	}
}


Transform& EditorCamera::GetTransform()
{
	return myTransform;
}
