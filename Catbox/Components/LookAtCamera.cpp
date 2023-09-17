#include "stdafx.h"
#include "LookAtCamera.h"

void LookAtCamera::Update()
{
	myTransform->LookAt(GraphicsEngine::GetInstance()->GetMainCamera()->GetTransform()->worldPos());
	//myTransform->SetWorldRot({ 45.0f, 180.0f, 0 });
}

void LookAtCamera::RunInEditor()
{
	myTransform->LookAt(Editor::GetInstance()->GetEditorCamera().GetTransform().worldPos());
	//myTransform->SetWorldRot({ 45.0f, 180.0f, 0 });
}
