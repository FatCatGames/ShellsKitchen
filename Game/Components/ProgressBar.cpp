#include "Game.pch.h"
#include "ProgressBar.h"
#include "Components\3D\ModelInstance.h"
#include "Assets\Material.h"
//#include "Shaders\Interaction\ProgressBarPS.h"
#include "Shaders\UI\CookingProgressBar\UICookingProgressBarPS.h"

void ProgressBar::Awake()
{
	//auto* model = myGameObject->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>();
	//if (!model)
	//{
	//	printerror(myGameObject->GetName() + " has no ModelInstance");
	//	return;
	//}

	//model->SetRenderToUse(Renderer::Forward);

	//auto mat = model->GetMaterial(0);
	//if (!mat)
	//{
	//	printerror(myGameObject->GetName() + " has no material");
	//	return;
	//}

	//myProgressBarPS = static_cast<ProgressBarPS*>(mat->GetPixelShader().get());


	myCamera = GameManager::GetInstance()->GetRayPicker()->GetGameObject().GetComponent<Camera>();

	if(myGameObject->GetTransform()->GetChildren().size() > 0)
	{
		myFirstChild = myGameObject->GetTransform()->GetChildren()[0];
	}
	if(myGameObject->GetTransform()->GetChildren().size() > 1)
	{
		mySecondChild = myGameObject->GetTransform()->GetChildren()[1];
	}

	myParentPos = myGameObject->GetTransform()->worldPos();
	myRayPickerComponent = GameManager::GetInstance()->GetRayPicker();
}

void ProgressBar::Update()
{
	if(myGameObject->GetTransform()->GetChildren().size() > 0)
	{
		if(myGameObject->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>())
		{
			Catbox::Matrix4x4<float> projection = myCamera->GetProjectionMatrix();
			Catbox::Matrix4x4<float> viewMatrix = Catbox::Matrix4x4<float>::GetFastInverse(myRayPickerComponent->GetTransform()->GetWorldTransformMatrix());

			Vector3f parentPos = myGameObject->GetTransform()->worldPos();

			Catbox::Vector4<float> parentPosWorld = { parentPos.x, parentPos.y, parentPos.z , 1 };
			parentPosWorld = parentPosWorld * viewMatrix;
			parentPosWorld = parentPosWorld * projection;
			parentPosWorld = parentPosWorld / parentPosWorld.w;

			parentPos = { parentPosWorld.x, parentPosWorld.y, parentPosWorld.z };
			parentPos += myOffset;
			myFirstChild->SetWorldPos(parentPos);
			mySecondChild->SetWorldPos(parentPos);
		}
	}
}

void ProgressBar::OnEnable()
{
	//myTransform->SetWorldRot(0, 0, 0);
}

void ProgressBar::SetProgress(float aPercent)
{
	//myProgressBarPS->myProgressData.Resource = aPercent;

	myProgress = aPercent;

	// Här borde det bli. 
	auto timeLeftShader = dynamic_cast<UICookingProgressBarPS*>(myTransform->GetChildren()[0]->GetGameObject()->GetComponent<UISprite>()->GetPixelShader().get());
	if(timeLeftShader)
	{
		timeLeftShader->myUICookingProgressData.Resource = myProgress;
		timeLeftShader->SetData();
	}
}
