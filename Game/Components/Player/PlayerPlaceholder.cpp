#include "Game.pch.h"
#include "PlayerPlaceholder.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Material.h"

PlayerPlaceholder::~PlayerPlaceholder()
{
}

void PlayerPlaceholder::Awake()
{
	GameObject* gameObject = nullptr;

	int playerId = GameManager::GetInstance()->GetNumberOfConnectedPlayers() + 1;
	CommonUtilities::XboxControllerInput* controller = GameManager::GetInstance()->GetPlayerController(playerId);

	switch (playerId)
	{
	case 1:
		gameObject = InstantiatePrefab("Player1");
		break;
	case 2:
		gameObject = InstantiatePrefab("Player2");
		break;
	case 3:
		controller->Refresh();
		if (controller->IsConnected())
		{
			gameObject = InstantiatePrefab("Player1");
			gameObject->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>("customer2_mat"), 0);
		}
		break;
	case 4:
		controller->Refresh();
		if (controller->IsConnected())
		{
			gameObject = InstantiatePrefab("Player2");
			gameObject->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>("customer4_mat"), 0);
		}
		break;
	default:
		gameObject = InstantiatePrefab("Player1");
		break;
	}

	if (gameObject != nullptr)
	{
		RigidBody* rigidbody = gameObject->GetComponent<RigidBody>();

		if (rigidbody != nullptr)
		{
			rigidbody->SetActorPosition(myGameObject->GetTransform()->worldPos());
		}

		Player* player = gameObject->GetComponent<Player>();

		if (player != nullptr)
		{
			player->SetPlayerID(playerId);
			GameManager::GetInstance()->AddPlayer(player);
		}
	}

	myGameObject->Destroy();
}
