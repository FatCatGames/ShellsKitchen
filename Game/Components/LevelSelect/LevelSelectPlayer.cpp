#include "Game.pch.h"
#include "LevelSelectPlayer.h"
#include "Components\Physics\Collisions\Collider.h"

LevelSelectPlayer::~LevelSelectPlayer()
{
	delete myFirstController;
	myFirstController = nullptr;
}

void LevelSelectPlayer::Awake()
{
	GameManager::GetInstance()->SetLevelSelectPlayer(this);
	myFirstController = new CommonUtilities::XboxControllerInput(CommonUtilities::XboxPlayer::One);
	myRigidBody = myGameObject->GetComponent<RigidBody>();

	for (auto child : myTransform->GetChildren())
	{
		if (child)
		{
			if (child->GetGameObject()->GetName() == "BoatWave")
			{
				myWaveLines = child;
			}
		}
	}
	myAudioComponent = myGameObject->GetComponent<AudioMonoComponent>();
	myRigidBody->SetActorPosition(GameManager::GetInstance()->GetBoatPos());
	myRigidBody->PutToSleep();
}

void LevelSelectPlayer::HandleWaveLines()
{
	if (myWaveLines)
	{

		//if (myRigidBody->GetVelocity().Length() > 0.f)
		//{
		//	myWaveShaderSpeed += fixedDeltaTime;
		//	mySpawnRippleTimer += fixedDeltaTime;
		//}
		//else
		//{
		//	myWaveShaderSpeed -= fixedDeltaTime;
		//	mySpawnRippleTimer -= fixedDeltaTime;
		//}
		//mySpawnRippleTimer = max(mySpawnRippleTimer, 0.f);
		//
		//if (mySpawnRippleTimer > 0.05f)
		//{
		//	mySpawnRippleTimer = 0.f;
		//	auto meme = InstantiatePrefab("BoatRipplesVFX");
		//	meme->GetTransform()->SetWorldPos(myTransform->worldPos() + myTransform->back() * 3.0f);
		//}
		//myWaveShaderSpeed = std::clamp(myWaveShaderSpeed, 0.f, 1.f);
		//myWaveLines->GetGameObject()->SetCustomFloat(myWaveShaderSpeed);
		myRigidBody->SetMaximumLinearVelocity(15);
	}
}

void LevelSelectPlayer::FixedUpdate()
{
	//Vector3f vel = myRigidBody->GetVelocity();
	//if (vel.y > 0)
	//{
	//	vel.y = 0;
	//	myRigidBody->SetVelocity(vel);
	//}

	if (myFirstController) myFirstController->Refresh();
	HandleMovement();
	HandleWaveLines();
	GameManager::GetInstance()->SetBoatPos(myTransform->worldPos());
}

void LevelSelectPlayer::HandleMovement()
{
	myGivingLeftStickInput = false;

	const Vector2f leftStick = Input::GetController().GetLeftStickAxis();
	const float deadZone = 0.05f;

	if ((leftStick.x * leftStick.x) > deadZone || (leftStick.y * leftStick.y) > deadZone)
	{
		myGivingLeftStickInput = true;
	}

	int directionToRotate = 1;

	if (myGivingLeftStickInput == true)
	{
		if (myAudioComponent)
		{
			Engine::GetInstance()->GetAudioManager()->SetParameterByIndex(myAudioComponent->GetSoundEventByIndex(0), 0, 0.4f);
		}
		Vector3f velocit = { 0.0f, myRigidBody->GetVelocity().y, 0.0f };
		myRigidBody->AddForce(myTransform->forward(), myMoveSpeed * fixedDeltaTime, physx::PxForceMode::eIMPULSE);

		if (velocit.x > myMoveSpeed)
		{
			velocit.x = myMoveSpeed;
		}
		else if (velocit.x < -myMoveSpeed)
		{
			velocit.x = -myMoveSpeed;
		}

		if (velocit.z > myMoveSpeed)
		{
			velocit.z = myMoveSpeed;
		}
		else if (velocit.z < -myMoveSpeed)
		{
			velocit.z = -myMoveSpeed;
		}

		myRigidBody->SetVelocity(velocit);

		const Vector2f rotatedTestVector = { -leftStick.y, leftStick.x };
		const Vector2f forward = { myTransform->forward().x, myTransform->forward().z };

		float dot = leftStick.GetNormalized().Dot(forward.GetNormalized());
		if (dot > 1.f) dot = 1.f;
		if (dot < -1.f) dot = -1.f;
		float rotationValue = Catbox::Rad2Deg(acos(dot));

		if (myRotSpeed * fixedDeltaTime < rotationValue)
		{
			rotationValue = myRotSpeed * fixedDeltaTime;
		}

		if (leftStick.Dot(forward) < 0)
		{
			// Vinkel större än 90. Fall (I).
			if (rotatedTestVector.Dot(forward) > 0)
			{
				directionToRotate = -1;
			}
			else
			{
				directionToRotate = 1;
			}
		}
		else
		{
			// Vinkel lika med eller mindre än 90. Fall (II).
			if (rotatedTestVector.Dot(forward) < 0)
			{
				directionToRotate = 1;
			}
			else
			{
				directionToRotate = -1;
			}
		}

		myRigidBody->Rotate({ 0, directionToRotate * rotationValue, 0 });
	}
	else
	{
		if (myAudioComponent)
		{
			Engine::GetInstance()->GetAudioManager()->SetParameterByIndex(myAudioComponent->GetSoundEventByIndex(0), 0, 0.0f);
		}
	}

	// Keyboard movement. Will NOT run if controller left stick WAS used this frame. 
	if (myGivingLeftStickInput == false)
	{
		Vector2f directionToMove = { 0.0f, 0.0f };

		bool gaveInput = false;
		bool gaveUpInput = false;
		bool gaveLeftInput = false;

		if (Input::GetKeyHeld(KeyCode::W) || Input::GetKeyHeld(KeyCode::UP))
		{
			directionToMove += {0.0f, 1.0f};
			gaveInput = true;
			gaveUpInput = true;
		}
		if (gaveUpInput == false)
		{
			if (Input::GetKeyHeld(KeyCode::S) || Input::GetKeyHeld(KeyCode::DOWN))
			{
				directionToMove += {0.0f, -1.0f};
				gaveInput = true;
			}
		}

		if (Input::GetKeyHeld(KeyCode::A) || Input::GetKeyHeld(KeyCode::LEFT))
		{
			directionToMove += {-1.0f, 0.0f};
			gaveInput = true;
			gaveLeftInput = true;
		}

		if (gaveLeftInput == false)
		{
			if (Input::GetKeyHeld(KeyCode::D) || Input::GetKeyHeld(KeyCode::RIGHT))
			{
				directionToMove += {1.0f, 0.0f};
				gaveInput = true;
			}
		}

		//Vector3f velocit = { 0.0f, 0.0f, 0.0f };
		Vector3f velocit = { 0.0f, myRigidBody->GetVelocity().y, 0.0f };

		if (gaveInput == true)
		{
			if (myAudioComponent)
			{
				Engine::GetInstance()->GetAudioManager()->SetParameterByIndex(myAudioComponent->GetSoundEventByIndex(0), 0, 0.4f);
			}
			if (directionToMove.x != 0 && directionToMove.y != 0)
			{
				directionToMove.Normalize();
			}
			myRigidBody->AddForce(myTransform->forward(), myMoveSpeed * fixedDeltaTime, physx::PxForceMode::eIMPULSE);

			if (velocit.x > myMoveSpeed)
			{
				velocit.x = myMoveSpeed;
			}
			else if (velocit.x < -myMoveSpeed)
			{
				velocit.x = -myMoveSpeed;
			}

			if (velocit.z > myMoveSpeed)
			{
				velocit.z = myMoveSpeed;
			}
			else if (velocit.z < -myMoveSpeed)
			{
				velocit.z = -myMoveSpeed;
			}

			//myRigidBody->SetVelocity(velocit);
			//print(std::to_string(velocit.x) + "," + std::to_string(velocit.y) + "," + std::to_string(velocit.z));

			int directionToRotate = 1;

			const Vector2f rotatedTestVector = { -directionToMove.y, directionToMove.x };
			const Vector2f forward = { myTransform->forward().x, myTransform->forward().z };

			float dot = directionToMove.GetNormalized().Dot(forward.GetNormalized());
			if (dot > 1.f) dot = 1.f;
			if (dot < -1.f) dot = -1.f;
			float rotationValue = Catbox::Rad2Deg(acos(dot));

			if (myRotSpeed * fixedDeltaTime < rotationValue)
			{
				rotationValue = myRotSpeed * fixedDeltaTime;
			}

			if (directionToMove.Dot(forward) < 0)
			{
				// Vinkel större än 90. Fall (I).
				if (rotatedTestVector.Dot(forward) > 0)
				{
					directionToRotate = -1;
				}
				else
				{
					directionToRotate = 1;
				}
			}
			else
			{
				// Vinkel lika med eller mindre än 90. Fall (II).
				if (rotatedTestVector.Dot(forward) < 0)
				{
					directionToRotate = 1;
				}
				else
				{
					directionToRotate = -1;
				}
			}

			myRigidBody->Rotate({ 0, directionToRotate * rotationValue, 0 });
		}
		else
		{
			myRigidBody->SetVelocity({ 0,0,0 });

			if (myAudioComponent)
			{
				Engine::GetInstance()->GetAudioManager()->SetParameterByIndex(myAudioComponent->GetSoundEventByIndex(0), 0, 0.0f);
			}
		}
	}
}