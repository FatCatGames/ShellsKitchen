#include "Game.pch.h"
#include "BurnIndicator.h"
#include "Components\Physics\RigidBody.h"

void BurnIndicator::Awake()
{
	myCamera = GameManager::GetInstance()->GetRayPicker()->GetGameObject().GetComponent<Camera>();

	for (auto child : myGameObject->GetTransform()->GetChildren())
	{
		if (child->GetGameObject()->GetName() == "BurnIndicator")
		{
			myBurnSprite = child;
			myBurnSprite->GetGameObject()->SetActive(false);
		}
	}

	myRayPickerComponent = GameManager::GetInstance()->GetRayPicker();
}

void BurnIndicator::Update()
{
	if (myBurnSprite)
	{
		if (auto sprite = myBurnSprite->GetGameObject()->GetComponent<UISprite>())
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

			sprite->UnlockTransform();
			myBurnSprite->SetWorldPos(parentPos);
			sprite->LockTransform();
		}
	}
}

void BurnIndicator::SetOffset(Vector3f anOffset)
{
	myOffset = anOffset;
}

void BurnIndicator::Start()
{
	myIsActive = true;
	myIndicatorIsVisible = true;
	if (myBurnSprite) myBurnSprite->GetGameObject()->SetActive(true);

}

void BurnIndicator::Switch()
{
	myIndicatorIsVisible = !myIndicatorIsVisible;
	if (myBurnSprite) myBurnSprite->GetGameObject()->SetActive(myIndicatorIsVisible);
}

void BurnIndicator::Stop()
{
	myIsActive = false;
	myIndicatorIsVisible = false;
	if (myBurnSprite) myBurnSprite->GetGameObject()->SetActive(myIndicatorIsVisible);
}
