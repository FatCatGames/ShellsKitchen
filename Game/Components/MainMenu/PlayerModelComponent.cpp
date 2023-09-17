#include "Game.pch.h"
#include "PlayerModelComponent.h"
#include "Components/3D/ModelInstance.h"

PlayerModelComponent::PlayerModelComponent()
{
}

PlayerModelComponent::~PlayerModelComponent()
{
}

void PlayerModelComponent::Awake()
{
	if (myTransform->GetChildren().size() > 0)
	{
		myTransform->GetChildren()[0]->GetGameObject()->GetComponent<ModelInstance>()->SetRenderToUse(Renderer::Forward);
	}
}

void PlayerModelComponent::Update()
{

}

void PlayerModelComponent::RenderInProperties(std::vector<Component*>& aComponentList)
{
	//ImGui::Checkbox("Affected by Character Controllers", &myIsAffectedByCharacterControllers);
	//ImGui::Checkbox("Bob", &myShouldBob);
	//ImGui::DragFloat3("Bob Speed", myBobSpeeds);
	//ImGui::DragFloat3("Bob Height", myBobHeights);
	//ImGui::Checkbox("Spin", &myShouldSpin);
	//ImGui::DragFloat3("Spin Speed", mySpinSpeeds);
}

void PlayerModelComponent::RunInEditor()
{
}


void PlayerModelComponent::Save(rapidjson::Value& aComponentData)
{

}

void PlayerModelComponent::Load(rapidjson::Value& aComponentData)
{

}
