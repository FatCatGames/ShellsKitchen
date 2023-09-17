#include "Game.pch.h"
#include "MovingInteractable.h"
#include "Components/Physics/RigidBody.h"
#include "Components/Interactables/Holdables/Ingredient.h"
#include "Components/Environment/MovingInteractableItems.h"
#include "Editor/Windows/KitchenMakerWindow.h"
#include "Components/3D/VertexAnimator.h"
#include "Components/GamePlay/IngredientIcon.h"
#include <math.h>
void MovingInteractable::Awake()
{
	myIsGoingRight = maybe;
}

void MovingInteractable::setActiveVector()
{
	if (myMovingVectors.size() != 0)
	{
		int randomIndex = Catbox::GetRandom(0, static_cast<int>(myMovingVectors.size()));
		if (myMovingVectors[randomIndex].size() == 0)
		{
			print("ERROR ---  One of your Pos Vectors are empty");
		}
		else
		{
			myActiveVector = myMovingVectors[randomIndex];
			myActiveVectorIndex = randomIndex;
			myMovingPercent = 0;
			myActiveIndex = 0;
			myJumpPercent = 0;
			myMovingDistance = myActiveVector[0].Distance(myActiveVector[1]);
			myPrefab->GetComponent<RigidBody>()->SetActorPosition(myActiveVector[0]);
		}
	}

}

void MovingInteractable::FixedUpdate()
{
	if (start)
	{
		myStartingSpawnTimer = myStartingSpawnTimer + fixedDeltaTime;
		if (myStartingSpawnTimer > myStartingSpawnTime)
		{
			start = false;
			myJumpUpFlag = true;

			myPrefab = InstantiatePrefab("MovingInteractableItem");
			setActiveVector();
			if (VertexAnimator* vx = myPrefab->GetComponent<VertexAnimator>())
			{
				vx->StopVAT();
			}
		}
	}

	CheckIfHeld();
	if (myJumpUpFlag)
	{
		if (!myIsIdlee)
		{
			JumpUp();
		}
		else
		{
			Idlee();
		}

	}
	else if (myPatrollingFlag)
	{
		Patrolling();
	}
	else if (myJumpDownFlag)
	{

		if (!myIsIdlee)
		{
			JumpDown();
		}
		else
		{
			Idlee();
		}

	}
}



void MovingInteractable::JumpUp()
{
	//Need to optimize turning anim off, but it did not turn off when I did it only once before so added it likes this
	if (!myPrefab)
	{
		return;
	}
	if (VertexAnimator* vx = myPrefab->GetComponent<VertexAnimator>())
	{
		vx->StopVAT();
	}

	myJumpPercent = myJumpPercent + myJumpSpeed[myActiveVectorIndex] * fixedDeltaTime;
	float lerpY = Catbox::Lerp(-Catbox::Pi() / 2, (Catbox::Pi() / 2), myJumpPercent);
	lerpY = sin(lerpY) * 4;
	myMovingPercent = myMovingPercent + myJumpMovingSpeed[myActiveVectorIndex] / myMovingDistance * fixedDeltaTime;
	Vector3f lerpXY = Catbox::Lerp(myActiveVector[myActiveIndex], myActiveVector[myActiveIndex + 1], myMovingPercent);
	if (myMovingPercent > 1)
	{
		myMovingPercent = 1;
	}
	if (Ingredient* in = myPrefab->GetComponent<Ingredient>())
	{
		in->SetIconActive(true);
	}
	myPrefab->GetComponent<RigidBody>()->SetActorPosition({ lerpXY.x,lerpY + myJumpingY[myActiveVectorIndex], lerpXY.z });
	if (myMovingPercent == 1)
	{
		if (Ingredient* in = myPrefab->GetComponent<Ingredient>())
		{
			in->ResetSplash();
		}
		myJumpUpFlag = false;
		myPatrollingFlag = true;
		myMovingPercent = 0;
		myJumpPercent = 0;
		myActiveIndex++;

		myIsIdlee = true;
		myTimerTime = myIdleTime;
		myIdleeSetPos = true;
		myIdleePos = lerpXY;

		if (VertexAnimator* vx = myPrefab->GetComponent<VertexAnimator>())
		{
			vx->PlayVAT(0);
		}
	}
}

void MovingInteractable::JumpDown()
{
	if (!myPrefab)
	{
		return;
	}

	//Need to optimize turning anim off, but it did not turn off when I did it only once before so added it likes this
	if (myPrefab != nullptr)
	{
		if (VertexAnimator* vx = myPrefab->GetComponent<VertexAnimator>())
		{
			vx->StopVAT();
		}
	}

	myJumpPercent = myJumpPercent + myJumpSpeed[myActiveVectorIndex] * 0.75f * fixedDeltaTime;
	float lerpY = Catbox::Lerp((0.f), (3 * Catbox::Pi()) / 2, myJumpPercent);
	lerpY = (sin(lerpY + 0.5f) - 0.5) * 4;
	if (myJumpPercent > 1)
	{
		myJumpPercent = 1;
	}

	myMovingPercent = myMovingPercent + myJumpMovingSpeed[myActiveVectorIndex] / myMovingDistance * fixedDeltaTime;
	Vector3f lerpXY = Catbox::Lerp(myActiveVector[myActiveIndex], myActiveVector[myActiveIndex + 1], myMovingPercent);
	if (myMovingPercent > 1)
	{
		myMovingPercent = 1;
	}
	if (myPrefab != nullptr)
	{
		myPrefab->GetComponent<RigidBody>()->SetActorPosition({ lerpXY.x, myActiveVector[myActiveVector.size() - 1].y + lerpY, lerpXY.z });
	}
	if (myMovingPercent == 1)
	{
		if (myPrefab != nullptr)
		{
			myPrefab->Destroy();
		}
		myPrefab = nullptr;
		myIsRespawning = true;
		myIsIdlee = true;
		myJumpPercent = 0;
		myTimerTime = Catbox::GetRandom(myRandomSecondsLOW, myRandomSecondsMAX);
	}
}

void MovingInteractable::Patrolling()
{
	if (myActiveVector.size() != 0 && myIsIdlee == false)
	{
		if (myActiveVector.size() - 2 > myActiveIndex)
		{
			myMovingPercent = myMovingPercent + mySpeed / myMovingDistance * fixedDeltaTime;
			if (myMovingPercent > 1)
			{
				myMovingPercent = 1;
			}
			Vector3f lerp = Catbox::Lerp(myActiveVector[myActiveIndex], myActiveVector[myActiveIndex + 1], myMovingPercent);

			//Random Noice (do only once per line!)

			Vector2f rotation = { lerp.GetNormalized().x,lerp.GetNormalized().z };
			rotation.x = rotation.x * (-1);
			rotation = { rotation.y,rotation.x };
			//Right Or Left
			myTimeUntilTurning = myTimeUntilTurning + fixedDeltaTime;
			if (myTimeUntilTurning > myTurnTimer)
			{
				if (Catbox::GetRandom(0, 100) <= myProcentChangeDirection)
				{
					if (myIsGoingRight)
					{
						myIsGoingRight = false;
					}
					else
					{
						myIsGoingRight = true;
					}
				}
			}

			//
			if (myIsGoingRight)
			{
				myNintyDegreeNoice = myNintyDegreeNoice + abs(Catbox::GetRandom(0.f, myNoiceSpeed));
			}
			else
			{
				myNintyDegreeNoice = myNintyDegreeNoice - abs(Catbox::GetRandom(0.f, myNoiceSpeed));
			}

			if (myNintyDegreeNoice > myMaxNoice)
			{
				myNintyDegreeNoice = myMaxNoice;
			}
			else if (myNintyDegreeNoice < -myMaxNoice)
			{
				myNintyDegreeNoice = -myMaxNoice;
			}
			myRotationAdd = { rotation.x,0,rotation.y };
			myRotationAdd = myRotationAdd * myNintyDegreeNoice;

			//

			myPrefab->GetComponent<RigidBody>()->SetActorPosition(lerp + myRotationAdd);

			if ((lerp.z) == (myActiveVector[myActiveIndex + 1].z) && (lerp.x) == (myActiveVector[myActiveIndex + 1].x) && (lerp.y) == (myActiveVector[myActiveIndex + 1].y))
			{
				myIsIdlee = true;
				myTimerTime = myIdleTime;
				myMovingPercent = 0;
				myActiveIndex++;
				if (myActiveVector.size() > myActiveIndex + 1)
				{
					myMovingDistance = myActiveVector[myActiveIndex].Distance(myActiveVector[myActiveIndex + 1]);
				}

				myIdleeSetPos = true;
				myIdleePos = lerp + myRotationAdd;
			}
		}
		else
		{
			myIsIdlee = true;
			myTimerTime = myIdleTime;
			myIdleeSetPos = true;
			myIdleePos = myPrefab->GetComponent<RigidBody>()->GetActorPosition();
			if (Ingredient* in = myPrefab->GetComponent<Ingredient>())
			{
				in->ResetSplash();
			}
			myMovingPercent = 0;

			myJumpDownFlag = true;
			myPatrollingFlag = false;
		}
	}
	else if (myIsIdlee)
	{
		Idlee();
	}
}

void MovingInteractable::Idlee()
{
	myIdleeTimer += fixedDeltaTime;
	if (myIdleeSetPos)
	{
		if (myPrefab != nullptr)
		{
			myPrefab->GetComponent<RigidBody>()->SetActorPosition(myIdleePos);
		}
	}
	if (myIdleeTimer > myTimerTime)
	{
		myIdleeTimer = 0;
		myIsIdlee = false;
		if (myIsRespawning)
		{
			myPrefab = InstantiatePrefab("MovingInteractableItem");
			myPrefab->GetComponent<RigidBody>()->ChangeGravityScale(true);
			setActiveVector();
			myIsRespawning = false;
			myIdleePos = { 0,0,0 };
			myIdleeSetPos = false;

			myJumpUpFlag = true;
			myJumpDownFlag = false;

		}
	}
}

void MovingInteractable::CheckIfHeld()
{
	if (myPrefab != nullptr)
	{
		if (myPrefab->GetComponent<MovingInteractableItems>()->GetIsHeld())
		{
			if (myPrefab->GetTransform()->GetChildren().size() > 0)
			{
				myPrefab->GetTransform()->GetChildren()[0]->GetGameObject()->SetActive(false);
			}
			if (RigidBody* rb = myPrefab->GetComponent<RigidBody>())
			{
				rb->SetRotationAxisLock(false, false, false);
			}
			if (VertexAnimator* vx = myPrefab->GetComponent<VertexAnimator>())
			{
				vx->StopVAT();
				//myPrefab->GetComponent<VertexAnimator>()->SetEnabled(false);
			}
			myIsRespawning = true;
			myIsIdlee = true;
			myIdleeSetPos = false;
			myTimerTime = Catbox::GetRandom(myRandomSecondsLOW, myRandomSecondsMAX);
			myPrefab = nullptr;
		}
	}
}



void MovingInteractable::RenderInProperties(std::vector<Component*>& aComponentList)
{
	bool disabled = myWaitingForClick;
	auto kitchenEditor = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<KitchenMakerWindow>(WindowType::KitchenEditor);

	if (myWaitingForClick)
	{
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			if (kitchenEditor)
			{
				myMovingVectors[myWaitingForClickTargetVector][myWaitingForClickTargetID] = kitchenEditor->GetClickPosCentered();
			}
			myWaitingForClick = false;
			myWaitingForClickDelay = true;
		}
		else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			myWaitingForClick = false;
			myWaitingForClickDelay = true;
		}
	}

	if ((!disabled && myWaitingForClickDelay) || !kitchenEditor)
	{
		if (!Input::GetKeyHeld(KeyCode::MOUSELEFT) && !ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			Editor::GetInstance()->SetPrefabBrushMode(false);
			Editor::GetInstance()->GetGizmos().SetGizmoActive(true);
			myWaitingForClickDelay = false;
		}
	}

	if (disabled)
	{
		ImGui::BeginDisabled();
	}

	ImGui::InputInt("randomRepsawnTimeLowValue", &myRandomSecondsLOW);
	ImGui::InputInt("randomRepsawnTimeHighValue", &myRandomSecondsMAX);
	ImGui::InputFloat("IdleTime", &myIdleTime);
	ImGui::InputFloat("mySpeed", &mySpeed);

	ImGui::InputFloat("TimebeforeCheckingIfTurning", &myTurnTimer);
	ImGui::InputInt("myProcentChanseOfDirectionChange", &myProcentChangeDirection);
	ImGui::InputFloat("myMaxCurveDistance", &myMaxNoice);
	ImGui::InputFloat("myCurvingSpeed", &myNoiceSpeed);

	std::string treeName = "vector";
	int vectorSize = myMovingVectors.size();
	ImGui::PushItemWidth(203);
	ImGui::InputInt("Vectors##123", &vectorSize);
	if (vectorSize != myMovingVectors.size() && vectorSize >= 0)
	{
		myMovingVectors.resize(vectorSize);
	}
	if (vectorSize != myDebugVector.size() && vectorSize >= 0)
	{
		myDebugVector.resize(vectorSize);
	}
	ImGui::PopItemWidth();


	for (size_t i = 0; i < myMovingVectors.size(); i++)
	{
		std::string InstanceTreeName = treeName + std::to_string(i);
		if (ImGui::TreeNode(InstanceTreeName.c_str()))
		{
			ImGui::InputFloat(("myJumpSpeed##" + std::to_string(i)).c_str(), &myJumpSpeed[i]);
			ImGui::InputFloat(("myJumpMovingSpeed##" + std::to_string(i)).c_str(), &myJumpMovingSpeed[i]);
			ImGui::InputFloat(("myJumpingY##" + std::to_string(i)).c_str(), &myJumpingY[i]);

			bool temp = myDebugVector[i];
			ImGui::Checkbox("ShowDebug(RequiredDebugDraw)", &temp);
			ImGui::TextWrapped("Setting positions by click required kitchen editor window open and uses kitchen editors height for clicking");
			myDebugVector[i] = temp;
			int amountOfPos = myMovingVectors[i].size();
			ImGui::PushItemWidth(203);
			ImGui::InputInt(("Positions##" + std::to_string(i)).c_str(), &amountOfPos);
			if (amountOfPos != myMovingVectors[i].size() && amountOfPos >= 0)
			{
				myMovingVectors[i].resize(amountOfPos);
			}
			ImGui::PopItemWidth();

			for (size_t posIndex = 0; posIndex < myMovingVectors[i].size(); posIndex++)
			{
				ImGui::PushItemWidth(70);
				ImGui::InputFloat((" x ##" + std::to_string(posIndex)).c_str(), &myMovingVectors[i][posIndex].x);
				ImGui::SameLine();
				ImGui::InputFloat((" y ##" + std::to_string(posIndex)).c_str(), &myMovingVectors[i][posIndex].y);
				ImGui::SameLine();
				ImGui::InputFloat((" z ##" + std::to_string(posIndex)).c_str(), &myMovingVectors[i][posIndex].z);
				if (kitchenEditor)
				{
					ImGui::SameLine();
					if (ImGui::Button(("PosClick##" + std::to_string(posIndex)).c_str()))
					{
						myWaitingForClickTargetVector = i;
						myWaitingForClickTargetID = posIndex;
						myWaitingForClick = true;
						Editor::GetInstance()->SetPrefabBrushMode(true);
						Editor::GetInstance()->GetGizmos().SetGizmoActive(false);
					}
				}
				ImGui::PopItemWidth();
			}
			ImGui::TreePop();
		}

	}

	for (size_t vec = 0; vec < myMovingVectors.size(); vec++)
	{
		if (myDebugVector[vec])
		{
			for (size_t wp = 0; wp < myMovingVectors[vec].size(); wp++)
			{
				auto& point = myMovingVectors[vec][wp];

				DebugDrawer::DrawSphere(point, 0.5f);
				Vector3f heightOffset = { 0.f, 0.0f, 0.f };

				if (wp < myMovingVectors[vec].size() - 1)
				{
					auto& point2 = myMovingVectors[vec][wp + 1];
					Vector3f direction = (point2 - point).GetNormalized();
					DebugDrawer::DrawLine(point + heightOffset + direction * 0.5f, point2 + heightOffset - direction * 0.5f);


					float arrowheadLength = 0.5f;
					Vector3f arrowhead1 = point2 + heightOffset - (direction * arrowheadLength) - (direction.Cross(Vector3f::up()) * arrowheadLength);
					Vector3f arrowhead2 = point2 + heightOffset - (direction * arrowheadLength) + (direction.Cross(Vector3f::up()) * arrowheadLength);

					DebugDrawer::DrawLine(point2 + heightOffset - direction * 0.5f, arrowhead1 - direction * 0.5f);
					DebugDrawer::DrawLine(point2 + heightOffset - direction * 0.5f, arrowhead2 - direction * 0.5f);
				}
			}
		}
	}

	if (disabled)
	{
		ImGui::EndDisabled();
	}
}

void MovingInteractable::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();

	wrapper.SaveValue<DataType::Int>("randomRepsawnTimeLowValue", myRandomSecondsLOW);
	wrapper.SaveValue<DataType::Int>("randomRepsawnTimeHighValue", myRandomSecondsMAX);
	wrapper.SaveValue<DataType::Float>("IdleTime", myIdleTime);
	wrapper.SaveValue<DataType::Float>("mySpeed", mySpeed);

	wrapper.SaveValue<DataType::Int>("myProcentChanseOfDirectionChange", myProcentChangeDirection);
	wrapper.SaveValue<DataType::Float>("TimebeforeCheckingIfTurning", myTurnTimer);
	wrapper.SaveValue<DataType::Float>("myMaxCurveDistance", myMaxNoice);
	wrapper.SaveValue<DataType::Float>("myCurvingSpeed", myNoiceSpeed);


	auto tempArray = wrapper.CreateArray();
	for (size_t movingVectorIndex = 0; movingVectorIndex < myMovingVectors.size(); movingVectorIndex++)
	{

		rapidjson::Value point;
		point.SetObject();
		auto tempArray2 = wrapper.CreateArray();
		point.AddMember("myJumpSpeed", myJumpSpeed[movingVectorIndex], alloc);
		point.AddMember("myJumpMovingSpeed", myJumpMovingSpeed[movingVectorIndex], alloc);
		point.AddMember("myJumpingY", myJumpingY[movingVectorIndex], alloc);
		for (size_t posVectorIndex = 0; posVectorIndex < myMovingVectors[movingVectorIndex].size(); posVectorIndex++)
		{
			rapidjson::Value pos;
			pos.SetObject();
			////enemyType.SetInt(static_cast<int>(mySpawnList[spawnIndex][enemyIndex].enemyType));
			pos.AddMember("x", myMovingVectors[movingVectorIndex][posVectorIndex].x, alloc);
			pos.AddMember("y", myMovingVectors[movingVectorIndex][posVectorIndex].y, alloc);
			pos.AddMember("z", myMovingVectors[movingVectorIndex][posVectorIndex].z, alloc);
			tempArray2.PushBack(pos, alloc);
		}
		point.AddMember("myPosVectors", tempArray2, alloc);
		tempArray.PushBack(point, alloc);
	}
	wrapper.SaveObject("myMovingVectors", tempArray);
}

void MovingInteractable::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("randomRepsawnTimeLowValue"))
	{
		myRandomSecondsLOW = aComponentData["randomRepsawnTimeLowValue"].GetInt();
	}
	if (aComponentData.HasMember("randomRepsawnTimeHighValue"))
	{
		myRandomSecondsMAX = aComponentData["randomRepsawnTimeHighValue"].GetInt();
	}
	if (aComponentData.HasMember("IdleTime"))
	{
		myIdleTime = aComponentData["IdleTime"].GetFloat();
	}
	if (aComponentData.HasMember("mySpeed"))
	{
		mySpeed = aComponentData["mySpeed"].GetFloat();
	}
	if (aComponentData.HasMember("myProcentChanseOfDirectionChange"))
	{
		myProcentChangeDirection = aComponentData["myProcentChanseOfDirectionChange"].GetInt();
	}
	if (aComponentData.HasMember("TimebeforeCheckingIfTurning"))
	{
		myTurnTimer = aComponentData["TimebeforeCheckingIfTurning"].GetFloat();
	}
	if (aComponentData.HasMember("myMaxCurveDistance"))
	{
		myMaxNoice = aComponentData["myMaxCurveDistance"].GetFloat();
	}
	if (aComponentData.HasMember("myCurvingSpeed"))
	{
		myNoiceSpeed = aComponentData["myCurvingSpeed"].GetFloat();
	}
	if (aComponentData.HasMember("myMovingVectors"))
	{
		int movingVectorSize = aComponentData["myMovingVectors"].GetArray().Size();
		myMovingVectors.resize(movingVectorSize);
		myJumpSpeed.resize(movingVectorSize);
		myJumpMovingSpeed.resize(movingVectorSize);
		myJumpingY.resize(movingVectorSize);
		for (size_t movingVectorIndex = 0; movingVectorIndex < movingVectorSize; movingVectorIndex++)
		{
			if (aComponentData["myMovingVectors"][movingVectorIndex].HasMember("myJumpSpeed"))
			{
				myJumpSpeed[movingVectorIndex] = aComponentData["myMovingVectors"][movingVectorIndex]["myJumpSpeed"].GetFloat();
			}
			if (aComponentData["myMovingVectors"][movingVectorIndex].HasMember("myJumpMovingSpeed"))
			{
				myJumpMovingSpeed[movingVectorIndex] = aComponentData["myMovingVectors"][movingVectorIndex]["myJumpMovingSpeed"].GetFloat();
			}
			if (aComponentData["myMovingVectors"][movingVectorIndex].HasMember("myJumpingY"))
			{
				myJumpingY[movingVectorIndex] = aComponentData["myMovingVectors"][movingVectorIndex]["myJumpingY"].GetFloat();
			}

			if (aComponentData["myMovingVectors"][movingVectorIndex].HasMember("myPosVectors"))
			{
				int posVectorSize = aComponentData["myMovingVectors"][movingVectorIndex]["myPosVectors"].GetArray().Size();
				myMovingVectors[movingVectorIndex].resize(posVectorSize);
				for (size_t posVectorIndex = 0; posVectorIndex < posVectorSize; posVectorIndex++)
				{
					if (aComponentData["myMovingVectors"][movingVectorIndex]["myPosVectors"][posVectorIndex].HasMember("x"))
					{
						myMovingVectors[movingVectorIndex][posVectorIndex].x = aComponentData["myMovingVectors"][movingVectorIndex]["myPosVectors"][posVectorIndex]["x"].GetFloat();
					}
					if (aComponentData["myMovingVectors"][movingVectorIndex]["myPosVectors"][posVectorIndex].HasMember("y"))
					{
						myMovingVectors[movingVectorIndex][posVectorIndex].y = aComponentData["myMovingVectors"][movingVectorIndex]["myPosVectors"][posVectorIndex]["y"].GetFloat();
					}
					if (aComponentData["myMovingVectors"][movingVectorIndex]["myPosVectors"][posVectorIndex].HasMember("z"))
					{
						myMovingVectors[movingVectorIndex][posVectorIndex].z = aComponentData["myMovingVectors"][movingVectorIndex]["myPosVectors"][posVectorIndex]["z"].GetFloat();
					}
				}
			}
		}
	}
}


