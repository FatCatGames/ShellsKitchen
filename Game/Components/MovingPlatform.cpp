#include "Game.pch.h"
#include "MovingPlatform.h"
#include "Components/Physics/Collisions/Collider.h"
#include "Components/Physics/RigidBody.h"
#include "Components/Interactables/Surfaces/IngredientContainer.h"
#include "Components/BobSpinComponent.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Model.h"
#include "Assets/Material.h"
#include "Components/Animator.h"

#include "Editor/Windows/KitchenMakerWindow.h"

MovingPlatform::MovingPlatform()
{
}

MovingPlatform::~MovingPlatform()
{
}

void MovingPlatform::Awake()
{
	if (myHasHermitCrab)
	{
		if (!myCrabContainer)
		{	//Top 10 ways to get blacklisted from the industry, Number 7:
			if (Engine::GetInstance()->GetGameObjectFactory()->GetObjectById(77777))
			{
				myCrabContainer = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById(77777);
			}
			else
			{
				myCrabContainer = Engine::GetInstance()->GetGameObjectFactory()->CreateGameObjectWithId(77777);
				myCrabContainer->SetName("Crabs");
			}
		}

		myCrab = Engine::GetInstance()->GetGameObjectFactory()->CreateGameObjectWithPrefab("HermitCrab");
		myCrab->SetActive(false);

		if (myCrab->GetComponent<Animator>())
		{
			myAnimator = myCrab->GetComponent<Animator>();
		}
	}

	if (myGameObject->GetComponent<RigidBody>())
	{
		myRigidBody = myGameObject->GetComponent<RigidBody>();
		myRigidBody->SetKinematic();
		myCastedActor = static_cast<physx::PxRigidDynamic*>(myRigidBody->GetActor());
		myRigidBody->SetActorPosition(myWaypoints[myCurrentWaypoint].position);
	}

	for (size_t i = 0; i < myTransform->GetChildren().size(); i++)
	{
		Transform* aChild = myTransform->GetChildren()[i];
		if (aChild->GetGameObject()->GetComponent<RigidBody>())
		{
			Engine::GetInstance()->GetPhysicsEngine()->CreateJoint(
				myRigidBody->GetActor(),
				{ aChild->worldPos().x, aChild->worldPos().y, aChild->worldPos().z },
				aChild->GetGameObject()->GetComponent<RigidBody>()->GetActor(),
				{ myTransform->worldPos().x, myTransform->worldPos().y, myTransform->worldPos().z });
		}
		else if (aChild->GetGameObject()->GetComponent<BoxCollider>())
		{
			Engine::GetInstance()->GetPhysicsEngine()->CreateJoint(
				myRigidBody->GetActor(),
				{ aChild->worldPos().x, aChild->worldPos().y, aChild->worldPos().z },
				aChild->GetGameObject()->GetComponent<BoxCollider>()->GetStatic(),
				{ myTransform->worldPos().x, myTransform->worldPos().y, myTransform->worldPos().z });
			myStaticChildren.push_back(aChild->GetGameObject()->GetComponent<BoxCollider>());
		}
	}

	myInitialPosition = myTransform->worldPos();
	myCurrentWaypoint = 0;
	if (myWaypoints.size() != 0)
	{
		myFinalPosition = myWaypoints[myCurrentWaypoint].position;
	}
	myTotalTime = 0;
	myElapsedDelayTime = 0;
	myTravelPrecent = 0;
	myAngle = Catbox::Rad2Deg(atan2f(myFinalPosition.x - myInitialPosition.x, myFinalPosition.z - myInitialPosition.z));
	myOriginalAngle = myTransform->worldRot();

	//myWaypoints[0].delay -= ((myID) * (1.0f/6.0f));
	//myWaypoints[1].delay += ((6 - myID) * (1.0f/6.0f)) * 2;

	//myWaypoints[0].delay += (6 - myID) * (1.0f/6.0f);
	//myWaypoints[1].delay -= (6 - myID) * (1.0f/6.0f);
	//myBasicBitchUniversalDelay = (myID) * (1.0f / 6.0f);
	myBasicBitchUniversalDelay = (myID) * (1.0f / 6.0f);
}

void MovingPlatform::FixedUpdate()
{
	if (myHasHermitCrab && !myCrab->IsActive())
	{
		myRigidBody->SetActorRotation(myOriginalAngle);
	}

	if (myCastedActor && myBasicBitchUniversalDelay < myElapsedDelayTime)
	{
		Vector3f currentPos = myRigidBody->GetActorPosition();
		Vector3f endPos = currentPos;
		Vector3f vel = Vector3f::zero();

		if (myCurrentWaypoint < myWaypoints.size() && myWaypoints[myCurrentWaypoint].delay < myTotalTime)
		{
			if (!(currentPos == myFinalPosition)) //That there != Operator don't work :Baffled:
			{
				if (myHasHermitCrab)
				{
					if (!myCrab->IsActive())
					{
						myCrab->SetActive(true);
						myAnimator->RandomizeFramze();
						/*
						Vector3f myInverseTiltDirection = myFinalPosition - myInitialPosition;
						myInverseTiltDirection.Normalize();
						myRigidBody->SetActorRotation({ -25 * myInverseTiltDirection.z, 0, 25 * myInverseTiltDirection.x });
						*/
					}
					myCrab->GetTransform()->SetWorldRot(0, myAngle, 0);
					myCrab->GetTransform()->SetWorldPos(myTransform->worldPos().x, myTransform->worldPos().y - 0.5f, myTransform->worldPos().z);

					Vector3f myInverseTiltDirection = myFinalPosition - myInitialPosition;
					myInverseTiltDirection.Normalize();

					if (myTravelPrecent > 0.96f)
					{
						myShellTravelPrecent -= 5.5f * fixedDeltaTime;
						if (myShellTravelPrecent < 0)
						{
							myShellTravelPrecent = 0;
						}
					}
					else
					{
						myShellTravelPrecent += 5.5f * fixedDeltaTime;
						if (myShellTravelPrecent > 1)
						{
							myShellTravelPrecent = 1;
						}
					}

					myCrab->GetTransform()->SetWorldScale({myShellTravelPrecent, myShellTravelPrecent, myShellTravelPrecent});
					myRigidBody->SetActorRotation({ myOriginalAngle.x + (-25 * myInverseTiltDirection.z * myShellTravelPrecent), myOriginalAngle.y, myOriginalAngle.z + (25 * myInverseTiltDirection.x * myShellTravelPrecent)});
				}

				float myMovingDistance = myInitialPosition.Distance(myFinalPosition);
				myTravelPrecent += myWaypoints[myCurrentWaypoint].speed / myMovingDistance * fixedDeltaTime;
				if (myTravelPrecent > 1)
				{
					myTravelPrecent = 1;
				}
				Vector3f newPosition = Catbox::Lerp(myInitialPosition, myFinalPosition, myTravelPrecent);
				physx::PxTransform newTransform;
				newTransform.p = { newPosition.x, newPosition.y, newPosition.z };
				newTransform.q = myCastedActor->getGlobalPose().q;
				myCastedActor->setKinematicTarget(newTransform);

				endPos = newPosition;

				//if (myTravelPrecent == 1) //Potential Fix
				if (newPosition == myFinalPosition)
				{
					endPos = myFinalPosition;
					myRigidBody->SetActorPosition(myFinalPosition);
					myTravelPrecent = 0;
					myShellTravelPrecent = 0;
					myTotalTime = 0;
					myInitialPosition = myWaypoints[myCurrentWaypoint].position;
					myCurrentWaypoint += 1;
					if (myCurrentWaypoint == myWaypoints.size())
					{
						if (myShouldLoop)
						{
							myCurrentWaypoint = 0;
							myFinalPosition = myWaypoints[myCurrentWaypoint].position;
							myAngle = Catbox::Rad2Deg(atan2f(myFinalPosition.x - myInitialPosition.x, myFinalPosition.z - myInitialPosition.z));
						}
					}
					else
					{
						myFinalPosition = myWaypoints[myCurrentWaypoint].position;
						myAngle = Catbox::Rad2Deg(atan2f(myFinalPosition.x - myInitialPosition.x, myFinalPosition.z - myInitialPosition.z));
					}

					if (myHasHermitCrab)
					{
						myRigidBody->SetActorRotation({ 0, 0, 0 });
						myCrab->SetActive(false);

						if (!myIsMovingBack)
						{
							//myBasicBitchUniversalDelay = (6 - myID) * (1.0f / 6.0f) * 2;
							myBasicBitchUniversalDelay = (6 - myID) * (2.0f / 6.0f);
							myIsMovingBack = true;
							myElapsedDelayTime = 0;
						}
						else
						{
							//myBasicBitchUniversalDelay = (myID) * (1.0f / 6.0f);
							myBasicBitchUniversalDelay = (myID) * (2.0f / 6.0f);
							myIsMovingBack = false;
							myElapsedDelayTime = 0;
						}
					}
				}
				else if (myHasHermitCrab)
				{
					myRigidBody->SetActorPosition({ newTransform.p.x, myFinalPosition.y + 0.5f, newTransform.p.z });
				}

				for (size_t i = 0; i < myStaticChildren.size(); i++)
				{
					myStaticChildren[i]->SetActorPosition(myStaticChildren[i]->GetTransform()->worldPos());
				}
			}
			else
			{
				myRigidBody->SetActorPosition(myFinalPosition);
				myTravelPrecent = 0;
				myShellTravelPrecent = 0;
				myTotalTime = 0;
				myInitialPosition = myWaypoints[myCurrentWaypoint].position;
				myCurrentWaypoint += 1;
				if (myCurrentWaypoint == myWaypoints.size())
				{
					if (myShouldLoop)
					{
						myCurrentWaypoint = 0;
						myFinalPosition = myWaypoints[myCurrentWaypoint].position;
						myAngle = Catbox::Rad2Deg(atan2f(myFinalPosition.x - myInitialPosition.x, myFinalPosition.z - myInitialPosition.z));
					}
				}
				else
				{
					myFinalPosition = myWaypoints[myCurrentWaypoint].position;
					myAngle = Catbox::Rad2Deg(atan2f(myFinalPosition.x - myInitialPosition.x, myFinalPosition.z - myInitialPosition.z));
				}
			}
		}
		else if (myHasHermitCrab && !(currentPos == myFinalPosition) && myCurrentWaypoint < myWaypoints.size() && myWaypoints[myCurrentWaypoint].delay - 1.0f < myTotalTime)
		{
			Vector3f dir = { Catbox::GetRandom(-1.0f, 1.0f), 0, Catbox::GetRandom(-1.0f, 1.0f) };
			Vector3f shakeDistance = 0.5f * dir * fixedDeltaTime;
			myRigidBody->SetActorPosition(myTransform->worldPos() + shakeDistance);
		}

		if (myShouldCarryRigidbodies)
		{
			for (size_t i = 0; i < myObjectsToCarry.size(); i++)
			{
				//if (abitch[i]->GetGameObject().GetComponent<RigidBody>() && (myTransform->worldPos().y + (myTransform->worldScale().y) / 2) - 0.05f < abitch[i]->GetTransform()->worldPos().y)
				if (myObjectsToCarry[i]->GetGameObject().GetComponent<RigidBody>())
				{
					Vector3f displacement;
					displacement.x = endPos.x - currentPos.x;
					displacement.y = endPos.y - currentPos.y;
					displacement.z = endPos.z - currentPos.z;

					vel.x = displacement.x / fixedDeltaTime;
					vel.y = displacement.y / fixedDeltaTime;
					vel.z = displacement.z / fixedDeltaTime;


					if (myObjectsToCarry[i]->GetGameObject().GetComponent<Player>() /*&& myObjectsToCarry[i]->GetTransform()->worldPos().y > myTransform->worldPos().y*/)
					{
						Vector3f velocity = { vel };
						myObjectsToCarry[i]->GetGameObject().GetComponent<Player>()->SetAdditionalVelocity(velocity);
					}
					else
					{
						myObjectsToCarry[i]->GetGameObject().GetComponent<RigidBody>()->SetVelocity({ vel.x, vel.y, vel.z });
						//myObjectsToCarry[i]->GetGameObject().GetComponent<RigidBody>()->SetVelocity({ vel.x, myObjectsToCarry[i]->GetGameObject().GetComponent<RigidBody>()->GetVelocity().y + vel.y, vel.z });
					}
					//abitch[i]->GetGameObject().GetComponent<RigidBody>()->AddForce({ castedActor->getLinearVelocity().x * 7.5f, castedActor->getLinearVelocity().y * 7.5f, castedActor->getLinearVelocity().z * 7.5f }, physx::PxForceMode::eIMPULSE);

					//abitch[i]->GetGameObject().GetComponent<RigidBody>()->SetVelocity(myGameObject->GetComponent<BobSpinComponent>()->GetNextFrameAcceleration());

					/*
					Vector3f test = { castedActor->getLinearVelocity().x, castedActor->getLinearVelocity().y, castedActor->getLinearVelocity().z };
					test = myGameObject->GetComponent<BobSpinComponent>()->GetNextFrameAcceleration();
					test = Vector3f::zero();*/

				}
				else
				{
					myObjectsToCarry[i] = nullptr;
					myObjectsToCarry.erase(myObjectsToCarry.begin() + i);
					i -= 1;
				}
			}
		}

		myTotalTime += fixedDeltaTime;
	}
	else
	{
		myElapsedDelayTime += fixedDeltaTime;
	}
}

void MovingPlatform::RenderInProperties(std::vector<Component*>& aComponentList)
{
	bool disabled = myWaitingForClick;
	auto kitchenEditor = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<KitchenMakerWindow>(WindowType::KitchenEditor);

	if (myWaitingForClick)
	{
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			if (kitchenEditor)
			{
				if (myShouldCarryRigidbodies) //moving platform bool
				{
					myWaypoints[myWaitingForClickTargetID].position = kitchenEditor->GetClickPosCentered() + Vector3f{ -0.5f, 0, -0.5f };
				}
				else
				{
					myWaypoints[myWaitingForClickTargetID].position = kitchenEditor->GetClickPosCentered();
				}
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

	if (myHasHermitCrab)
	{
		ImGui::InputInt("Crab ID", &myID);
	}

	if (ImGui::Checkbox("Is Platform", &myShouldCarryRigidbodies))
	{
		if (myShouldCarryRigidbodies)
		{
			myHasHermitCrab = false;
		}
	}

	if (!myShouldCarryRigidbodies)
	{
		if (!PLAYMODE)
		{
			ImGui::Checkbox("Has Hermit Crab", &myHasHermitCrab);
		}
		else
		{
			ImGui::Text(":Baffled:");
		}
	}

	//ImGui::InputFloat("Startup Delay", &myBasicBitchUniversalDelay);
	ImGui::Checkbox("Loop", &myShouldLoop);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
	ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, 320), false, window_flags);

	if (myWaypoints.size() != 0)
	{
		for (int i = 0; i < myWaypoints.size(); i++)
		{
			std::string name = " Waypoint " + std::to_string(i);
			ImGui::Text(name.c_str());
			ImGui::Indent();
			ImGui::Text("Destination");
			name = "##" + name;
			Catbox::DragFloat3(name.c_str(), &myWaypoints[i].position, 1.0f, -100);
			name = "##Speed" + name;
			ImGui::Text("Speed");
			ImGui::InputFloat(name.c_str(), &myWaypoints[i].speed);
			name = "##Delay" + name;
			ImGui::Text("Delay");
			ImGui::InputFloat(name.c_str(), &myWaypoints[i].delay);
			ImGui::Unindent();

			if (ImGui::Button(("Set Position by clicking##waypoint" + std::to_string(i)).c_str()))
			{
				myWaitingForClick = true;
				myWaitingForClickTargetID = i;
				//auto selectedObjects = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy)->GetSelectedObjects();
				//for (size_t i = 0; i < selectedObjects.size(); i++)
				//{
				//	selectedObjects[i]->Deselect();
				//}
				//Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy)->ClearSelectedObjects();
				Editor::GetInstance()->SetPrefabBrushMode(true);
				Editor::GetInstance()->GetGizmos().SetGizmoActive(false);
			}
		}
	}

	ImGui::EndChild();

	if (ImGui::Button("Create Waypoint"))
	{
		Waypoint aWaypoint;
		myWaypoints.push_back(aWaypoint);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Latest"))
	{
		if (myWaypoints.size() != 0)
		{
			myWaypoints.erase(myWaypoints.end() - 1);
		}
	}

	if (!myWaypoints.empty())
	{
		auto& point = myTransform->worldPos();
		DebugDrawer::DrawSphere(point, 0.5f);
		Vector3f heightOffset = { 0.f, 0.5f, 0.f };

		Vector3f direction = (myWaypoints[0].position - point).GetNormalized();
		DebugDrawer::DrawLine(point + heightOffset + direction * 0.5f, myWaypoints[0].position + heightOffset - direction * 0.5f);


		float arrowheadLength = 0.5f;
		Vector3f arrowhead1 = myWaypoints[0].position + heightOffset - (direction * arrowheadLength) - (direction.Cross(Vector3f::up()) * arrowheadLength);
		Vector3f arrowhead2 = myWaypoints[0].position + heightOffset - (direction * arrowheadLength) + (direction.Cross(Vector3f::up()) * arrowheadLength);

		DebugDrawer::DrawLine(myWaypoints[0].position + heightOffset - direction * 0.5f, arrowhead1 - direction * 0.5f);
		DebugDrawer::DrawLine(myWaypoints[0].position + heightOffset - direction * 0.5f, arrowhead2 - direction * 0.5f);
	}


	for (size_t wp = 0; wp < myWaypoints.size(); wp++)
	{
		auto& point = myWaypoints[wp];
		if (myShouldCarryRigidbodies)
		{
			//This is a moving platform
			Vector3f pos = point.position + Vector3f{ 0.5f * myTransform->worldScale().x * 100.f , 0.5f , 0.5f * myTransform->worldScale().z * 100.f };
			DebugDrawer::DrawCube(pos, myTransform->worldScale() * 100.f, myTransform->worldRot());
		}
		else
		{
			DebugDrawer::DrawSphere(point.position, 0.5f);
		}
		Vector3f heightOffset = { 0.f, 0.5f, 0.f };

		if (wp < myWaypoints.size() - 1)
		{
			Vector3f direction = (myWaypoints[wp + 1].position - point.position).GetNormalized();
			DebugDrawer::DrawLine(point.position + heightOffset + direction * 0.5f, myWaypoints[wp + 1].position + heightOffset - direction * 0.5f);


			float arrowheadLength = 0.5f;
			Vector3f arrowhead1 = myWaypoints[wp + 1].position + heightOffset - (direction * arrowheadLength) - (direction.Cross(Vector3f::up()) * arrowheadLength);
			Vector3f arrowhead2 = myWaypoints[wp + 1].position + heightOffset - (direction * arrowheadLength) + (direction.Cross(Vector3f::up()) * arrowheadLength);

			DebugDrawer::DrawLine(myWaypoints[wp + 1].position + heightOffset - direction * 0.5f, arrowhead1 - direction * 0.5f);
			DebugDrawer::DrawLine(myWaypoints[wp + 1].position + heightOffset - direction * 0.5f, arrowhead2 - direction * 0.5f);
		}
	}

	if (disabled)
	{
		ImGui::EndDisabled();
	}
	//ImGui::ShowDemoWindow();
}

void MovingPlatform::OnCollisionEnter(Collider* aCollider)
{
	if (myShouldCarryRigidbodies && aCollider->GetGameObject().GetComponent<RigidBody>() && !aCollider->GetGameObject().GetComponent<IngredientContainer>())
	{
		//physx::PxRigidDynamic* castedActor = static_cast<physx::PxRigidDynamic*>(myGameObject->GetComponent<RigidBody>()->GetActor());
		//aCollider->GetGameObject().GetComponent<Player>()->AddAdditionalVelocity({ castedActor->getLinearVelocity().x, castedActor->getLinearVelocity().y, castedActor->getLinearVelocity().z });

		myObjectsToCarry.push_back(aCollider);
	}
}

void MovingPlatform::OnCollisionExit(Collider* aCollider)
{
	if (myShouldCarryRigidbodies && aCollider->GetGameObject().GetComponent<RigidBody>())
	{
		for (size_t i = 0; i < myObjectsToCarry.size(); i++)
		{
			if (myObjectsToCarry[i] == aCollider)
			{
				//abitch[i]->GetGameObject().GetComponent<Player>()->AddAdditionalVelocity({ 0, 0, 0 });
				if (myObjectsToCarry[i]->GetGameObject().GetComponent<Player>())
				{
					myObjectsToCarry[i]->GetGameObject().GetComponent<Player>()->SetAdditionalVelocity(Vector3f::zero());
				}
				myObjectsToCarry[i] = nullptr;
				myObjectsToCarry.erase(myObjectsToCarry.begin() + i);
			}
		}
	}
}

void MovingPlatform::Save(rapidjson::Value& aComponentData)
{
	int version = 3;
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();

	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveValue<DataType::Bool>("ShouldLoop", myShouldLoop);
	wrapper.SaveValue<DataType::Bool>("ShouldCarry", myShouldCarryRigidbodies);
	wrapper.SaveValue<DataType::Bool>("HasCrab", myHasHermitCrab);
	wrapper.SaveValue<DataType::Float>("InitialDelay", myBasicBitchUniversalDelay);
	wrapper.SaveValue<DataType::Int>("ID", myID);

	auto finalPosition = wrapper.CreateArray();
	for (size_t i = 0; i < myWaypoints.size(); i++)
	{
		rapidjson::Value typeID;
		typeID.SetObject();
		typeID.AddMember("X", myWaypoints[i].position.x, alloc);
		typeID.AddMember("Y", myWaypoints[i].position.y, alloc);
		typeID.AddMember("Z", myWaypoints[i].position.z, alloc);
		typeID.AddMember("Speed", myWaypoints[i].speed, alloc);
		typeID.AddMember("Delay", myWaypoints[i].delay, alloc);
		finalPosition.PushBack(typeID, alloc);
	}
	wrapper.SaveObject("Waypoints", finalPosition);
}

void MovingPlatform::Load(rapidjson::Value& aComponentData)
{
	int version = aComponentData["Version"].GetInt();

	if (version > 0)
	{
		myShouldLoop = aComponentData["ShouldLoop"].GetBool();
		myShouldCarryRigidbodies = aComponentData["ShouldCarry"].GetBool();
		myHasHermitCrab = aComponentData["HasCrab"].GetBool();
		for (size_t i = 0; i < aComponentData["Waypoints"].Size(); i++)
		{
			Waypoint aWaypoint;
			aWaypoint.position = {
				aComponentData["Waypoints"][i]["X"].GetFloat(),
				aComponentData["Waypoints"][i]["Y"].GetFloat(),
				aComponentData["Waypoints"][i]["Z"].GetFloat() };
			aWaypoint.speed = aComponentData["Waypoints"][i]["Speed"].GetFloat();
			aWaypoint.delay = aComponentData["Waypoints"][i]["Delay"].GetFloat();
			myWaypoints.push_back(aWaypoint);
		}

		if (version > 1)
		{
			myBasicBitchUniversalDelay = aComponentData["InitialDelay"].GetFloat();
		}

		if (version > 2)
		{
			myID = aComponentData["ID"].GetInt();
		}
	}
}
