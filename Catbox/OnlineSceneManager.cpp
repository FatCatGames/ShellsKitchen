#include "stdafx.h"
#include "OnlineSceneManager.h"
#include "..\CatNet\NetworkGameCommands.h"

void OnlineSceneManager::Render()
{
	//Debug draw frustum of all users
	for (int i = 0; i < myUsers.size(); i++)
	{
		auto camera = Editor::GetInstance()->GetEditorCamera().GetCamera();

		const float myNearPlane = camera->GetNearPlane();
		const float myFarPlane = camera->GetFarPlane();
		
		const float aspectRatio = Engine::GetInstance()->GetWindowRatio();
		const float farPlane = myFarPlane;
		const float halfHeight = farPlane * tanf(Catbox::Deg2Rad(0.5f * camera->GetFoV()));
		const float halfWidth = aspectRatio * halfHeight;
		const float nearHalfHeight = myNearPlane * tanf(Catbox::Deg2Rad(0.5f * camera->GetFoV()));
		const float nearHalfWidth = aspectRatio * nearHalfHeight;

		auto& matrix = myUsers[i].transform.GetWorldTransformMatrix();
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
		auto& worldPos = myUsers[i].transform.worldPos();

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
}

void OnlineSceneManager::SendMovmentMessages()
{
	Vector3f position = Editor::GetInstance()->GetEditorCamera().GetTransform().worldPos();
	Vector3f rotation = Editor::GetInstance()->GetEditorCamera().GetTransform().worldRot();
	auto movmentMessage = new NetMoveCommand();
}
