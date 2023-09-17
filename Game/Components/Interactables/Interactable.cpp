#include "Game.pch.h"
#include "Components\3D\ModelInstance.h"
#include "Assets\Material.h"
#include "Shaders\Interaction\HighlightPS.h"
#include "Components\Physics\RigidBody.h"

Interactable::~Interactable()
{
	for (auto& player : GameManager::GetInstance()->GetPlayers())
	{
		player->RemoveInteractable(this);
	}
}

void Interactable::Awake()
{
	myModel = myGameObject->GetComponent<ModelInstance>();
	if (!myModel)
	{
		printerror(myGameObject->GetName() + " has no ModelInstance");
		return;
	}

	myModel->SetRenderToUse(Renderer::Forward);
	CreateInteractableShader();
}

void Interactable::Update()
{
	if (myIsLerpingIn && myHighlightTimer < myHighlightLerpTime)
	{
		myHighlightTimer += deltaTime;
		if (myHighlightTimer > myHighlightLerpTime) myHighlightTimer = myHighlightLerpTime;

		for (size_t i = 0; i < myHighlightPS.size(); i++)
		{
			if (!myHighlightPS[i]) continue;
			myHighlightPS[i]->myHighlightData.Highlight = (myHighlightTimer / myHighlightLerpTime) * myHighlightStrength;
		}
	}
	else if (!myIsLerpingIn && myHighlightTimer > 0)
	{
		myHighlightTimer -= deltaTime;
		if (myHighlightTimer < 0) myHighlightTimer = 0;

		for (size_t i = 0; i < myHighlightPS.size(); i++)
		{
			if (!myHighlightPS[i]) continue;
			myHighlightPS[i]->myHighlightData.Highlight = (myHighlightTimer / myHighlightLerpTime) * myHighlightStrength;
		}
	}
}

void Interactable::SetmyHeldItem(HoldableItem& aHoldableItem)
{
	myHeldItem = &aHoldableItem;
}

HoldableItem* Interactable::GetMyHeldItem()
{
	return myHeldItem;
}


void Interactable::CreateInteractableShader()
{
	myHighlightPS.clear();
	auto mats = myModel->GetMaterials();
	for (size_t i = 0; i < mats.size(); i++)
	{
		if (!mats[i]) continue;
		mats[i]->SetPixelShader(AssetRegistry::GetInstance()->GetAsset<PixelShader>("HighlightPS"));
		mats[i] = mats[i]->MakeInstanceOfMaterial();
		myModel->SetMaterial(mats[i], i, false);
		myHighlightPS.push_back(static_cast<HighlightPS*>(mats[i]->GetPixelShader().get()));
	}
}

void Interactable::Highlight()
{
	myIsLerpingIn = true;
	/*for (size_t i = 0; i < myHighlightPS.size(); i++)
	{
		if (!myHighlightPS[i]) continue;
		myHighlightPS[i]->myHighlightData.Highlight = 1;
	}*/

	if (myHeldItem)
	{
		myHeldItem->Highlight();
	}
}

void Interactable::UnHighlight()
{
	/*for (size_t i = 0; i < myHighlightPS.size(); i++)
	{
		if (!myHighlightPS[i]) continue;
		myHighlightPS[i]->myHighlightData.Highlight = 0;
	}*/

	myIsLerpingIn = false;

	if (myHeldItem)
	{
		myHeldItem->UnHighlight();
	}
}

void Interactable::UnHighlightInstant()
{
	myIsLerpingIn = false;
	myHighlightTimer = 0;
	for (size_t i = 0; i < myHighlightPS.size(); i++)
	{
		if (!myHighlightPS[i]) continue;
		myHighlightPS[i]->myHighlightData.Highlight = 0;
	}


	if (myHeldItem)
	{
		myHeldItem->UnHighlightInstant();
	}
}

void Interactable::SetMyCanInteract(const bool& aInteractFlag)
{
	myCanInteract = aInteractFlag;
}
