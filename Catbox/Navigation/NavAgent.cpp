#include "stdafx.h"
#include "NavAgent.h"
#include "Debugging\DebugDrawer.h"
#include "CrowdManager.h"
#include "DetourCrowd.h"
#include "Components/Physics/CharacterController.h"

NavAgent::~NavAgent()
{
	if (myParams)
	{
		delete myParams;
	}
	if (myIsInCrowd)
	{
		Engine::GetInstance()->GetCrowdManager()->RemoveAgent(myCrowdId, myAgentId);
	}
}

void NavAgent::Init()
{
	myDtFilter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
	myDtFilter.setExcludeFlags(0);
	// Change costs.
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_WATER, 10.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_ROAD, 1.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_DOOR, 1.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_GRASS, 2.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_JUMP, 1.5f);
}

void NavAgent::Awake()
{
	myDtFilter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
	myDtFilter.setExcludeFlags(0);
	// Change costs.
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_WATER, 10.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_ROAD, 1.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_DOOR, 1.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_GRASS, 2.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_JUMP, 1.5f);

	if (myIsInCrowd)
	{
		auto crowdManager = Engine::GetInstance()->GetCrowdManager();
		if (!crowdManager->CrowdExists(myCrowdId))
		{
			crowdManager->AddCrowd(myCrowdId, myNavMeshToUse);
		}
		myAgentId = crowdManager->AddAgentToCrowd(myCrowdId, myTransform->worldPos(), myTransform->worldScale(), &myDtFilter, mySpeed, myAvoidianceType, myRadius, myParams);
	}
	//FindPath(myTargetPosition, true);
}

void NavAgent::Update()
{
	if (!myTargetReached && !myPathFailed && !myIsInCrowd)
	{
		if (!myUsingCharacterController)
		{
			myTransform->Translate(myDir * mySpeed * deltaTime);
		}
		//myTransform->LookAtHorizontal(myPath[myPathIndex]);
		Vector3f aVector = myPath[myPathIndex];
		Vector3f anotherVector = myTransform->worldPos();
		myTargetPosition = myPath[myPathIndex];
		anotherVector.y = aVector.y;

		if ((aVector - anotherVector).LengthSqr() < 0.1f)
		{
			if (++myPathIndex >= myPath.size())
			{
				myTargetReached = true;
			}
			else
			{
				myDir = (myPath[myPathIndex] - myTransform->worldPos()).GetNormalized();
			}
		}
	}
	else if (!myTargetReached && !myPathFailed && myIsInCrowd)
	{
		const dtCrowdAgent* agent = Engine::GetInstance()->GetCrowdManager()->GetAgent(myCrowdId, myAgentId);

		Vector3f currentPos = { agent->npos[0], agent->npos[1], agent->npos[2] };
		Vector3f agentTar = { agent->targetPos[0], agent->targetPos[1], agent->targetPos[2] };

		if (myTransform->worldPos().Distance(currentPos) > 4.0f)
		{
			auto charController = myGameObject->GetComponent<CharacterController>();
			if (charController)
			{
				charController->SetFootPosition(currentPos);
			}
			else
			{
				myTransform->SetWorldPos(currentPos);
			}
		}

		myTargetPosition = agentTar;

		DebugDrawer::DrawSphere(currentPos, 0.1f, Color::Green(), 5);
		DebugDrawer::DrawSphere(agentTar, 0.1f, Color::Red(), 5);

		if (agentTar.Distance(myTransform->worldPos()) < 0.3f)
		{
			if (++myPathIndex >= myPath.size())
			{
				myTargetReached = true;
			}
			else
			{
				RequestMove(myPath[myPathIndex]);
			}
		}

	}
}

void NavAgent::RunInEditor()
{
	if (myGameObject->IsSelected(0))
	{
		DebugDrawer::DrawSphere(myTargetPosition, 0.5f, Color::Cyan());
	}
}

bool NavAgent::FindPath(const Vector3f& aTarget, bool forcePathFinding)
{
	//if (aTarget == myTargetPosition && !forcePathFinding) { return false; };
	//if (myIsInCrowd) { RequestMove(aTarget); return true; }
	myPathIndex = 0;
	myPath.clear();
	myTargetReached = false;
	myTargetPosition = aTarget;

	auto navmesh = myNavMeshToUse->GetDtNavMeshQuery();
	dtPolyRef startPoly = 1;

	float posArray[3];
	Vector3f pos = myTransform->worldPos();
	posArray[0] = pos.x;
	posArray[1] = pos.y;
	posArray[2] = pos.z;

	float sizeArray[3];
	Vector3f size = myTransform->worldScale();
	sizeArray[0] = 10;
	sizeArray[1] = 10;
	sizeArray[2] = 10;

	float random = 0.1f;
	//float (*fpi)() = []() {return Catbox::GetRandom<float>(0, 1.0f); };
	//dtTileRef aRef;

	//float realPos[3];
	//realPos[0] = 0;
	//realPos[1] = 0;
	//realPos[2] = 0;
	//auto status = navmesh->findRandomPoint(&myDtFilter, fpi, &aRef, realPos);
	//if (dtStatusFailed(status))
	//{
	//	return false;
	//}
	float nearestPos[3];

	auto status = navmesh->findNearestPoly(posArray, sizeArray, &myDtFilter, &startPoly, nearestPos);
	if (dtStatusFailed(status))
	{
		myPathFailed = true;
		return false;
	}

	dtPolyRef endPoly = 1;

	float targetPosArray[3];
	Vector3f targetPos = aTarget;
	targetPosArray[0] = targetPos.x;
	targetPosArray[1] = targetPos.y;
	targetPosArray[2] = targetPos.z;

	float nearestPosTar[3];
	status = navmesh->findNearestPoly(targetPosArray, sizeArray, &myDtFilter, &endPoly, nearestPosTar);
	if (dtStatusFailed(status))
	{
		myPathFailed = true;
		return false;
	}
	dtPolyRef path[sizeof(dtPolyRef) * 25];

	int pathSize = 0;

	status = navmesh->findPath(startPoly, endPoly, nearestPos, nearestPosTar, &myDtFilter, path, &pathSize, myMaxPathSize);
	//if (dtStatusFailed(status))
	//{
	//	myPathFailed = true;
	//	return false;
	//}

	if (pathSize > 0)
	{
		float finalTargetPosArray[3];
		finalTargetPosArray[0] = targetPos.x;
		finalTargetPosArray[1] = targetPos.y;
		finalTargetPosArray[2] = targetPos.z;
		if (path[pathSize - 1] != endPoly)
		{
			navmesh->closestPointOnPoly(path[pathSize - 1], targetPosArray, finalTargetPosArray, 0);
		}

		float pathPos[sizeof(float) * 3 * 25];
		int straightPathSize = 0;

		status = navmesh->findStraightPath(nearestPos, nearestPosTar, path, pathSize, pathPos, nullptr, nullptr, &straightPathSize, myMaxPathSize);
		//if (dtStatusFailed(status))
		//{
		//	myPathFailed = true;
		//	return false;
		//}

		for (int i = 0; i < straightPathSize; i++)
		{
			Vector3f aPos;
			aPos.x = pathPos[i * 3];
			aPos.y = pathPos[i * 3 + 1];
			aPos.z = pathPos[i * 3 + 2];
			myPath.push_back(aPos);
		}
	}
	else
	{
		myPathFailed = true;
		return false;
	}
	myPathFailed = false;

	if (myIsInCrowd && !myPathFailed)
	{
		if (myPath.size() > 1) 
		{
			RequestMove(myPath[++myPathIndex]);
		}
		else 
		{
			RequestMove(myPath[myPathIndex]);
		}
	}
	return true;
}

void NavAgent::ChangeSpeed(float aMovmentSpeed)
{
	mySpeed = aMovmentSpeed;
	if (!myParams)
	{
		myParams = new dtCrowdAgentParams();
		myParams->radius = myRadius;
		myParams->height = myNavMeshToUse->GetBakingSettings().agentHeight;
		myParams->maxAcceleration = aMovmentSpeed * 2.0f;
		myParams->maxSpeed = aMovmentSpeed;
		myParams->collisionQueryRange = myParams->radius * 12.0f;
		myParams->pathOptimizationRange = myParams->radius * 1.0f;
		myParams->updateFlags = 0;

		myParams->queryFilterType =

			//if (m_toolParams.m_anticipateTurns)
			myParams->updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
		//if (m_toolParams.m_optimizeVis)
		myParams->updateFlags |= DT_CROWD_OPTIMIZE_VIS;
		//if (m_toolParams.m_optimizeTopo)
		myParams->updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
		//if (m_toolParams.m_obstacleAvoidance)
		myParams->updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
		//if (m_toolParams.m_separation)
		myParams->updateFlags |= DT_CROWD_SEPARATION;

		myParams->obstacleAvoidanceType = (unsigned char)(myAvoidianceType);
		myParams->separationWeight = 1.0f;

	}
	if (PLAYMODE && myParams && myAgentId != -1)
	{
		myParams->maxSpeed = aMovmentSpeed;
		myParams->maxAcceleration = aMovmentSpeed * 2.0f;
		Engine::GetInstance()->GetCrowdManager()->UpdateAgentParamaters(myCrowdId, myAgentId, myParams);
		Engine::GetInstance()->GetCrowdManager()->GetEditableAgent(myCrowdId, myAgentId)->desiredSpeed = aMovmentSpeed;
	}
}

void NavAgent::RequestMove(Vector3f aTarget)
{
	if (myIsInCrowd)
	{
		Engine::GetInstance()->GetCrowdManager()->RequestMove(myCrowdId, myAgentId, aTarget, Vector3f(10.0f, 10.0f, 10.0f), &myDtFilter);
	}
}

void NavAgent::ResumePathfinding()
{
	myTargetReached = myPathIndex >= myPath.size();
}

ActorState NavAgent::GetAgentState()
{
	const dtCrowdAgent* agent = Engine::GetInstance()->GetCrowdManager()->GetAgent(myCrowdId, myAgentId);
	if (agent && agent->state == DT_CROWDAGENT_STATE_OFFMESH)
	{
		return dtActorState_OffMesh;
	}
	return dtActorState_NotActive;
}

Vector3f NavAgent::GetTarget() const
{
	if (myPath.size() > 0 && myPathIndex < myPath.size())
	{
		return myPath[myPathIndex];
	}
	return Vector3f::zero();
}

Vector3f NavAgent::GetEndTarget() const
{
	if (myPath.size() > 0)
	{
		return myPath[myPath.size() - 1];
	}
	return myTransform->worldPos();
}

void NavAgent::ResetAgent()
{
	if (myIsInCrowd)
	{
		Vector3f pos = myTransform->worldPos();
		if (myGameObject->GetComponent<CharacterController>())
		{
			pos = myGameObject->GetComponent<CharacterController>()->GetActorPos();
		}
		if (myAgentId != -1) 
		{
			Engine::GetInstance()->GetCrowdManager()->RemoveAgent(myCrowdId, myAgentId);
		}

		auto crowdManager = Engine::GetInstance()->GetCrowdManager();
		if (!crowdManager->CrowdExists(myCrowdId))
		{
			crowdManager->AddCrowd(myCrowdId, myNavMeshToUse);
		}
		myAgentId = crowdManager->AddAgentToCrowd(myCrowdId, pos, myTransform->worldScale(), &myDtFilter, mySpeed, myAvoidianceType, myRadius, myParams);
	}
}


void NavAgent::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto agents = ComponentVectorCast<NavAgent>(aComponentList);

	bool edit;
	auto newNavMesh = AssetRegistry::GetInstance()->AssetDropdownMenu<NavMesh>(myNavMeshToUse.get(), "NavMesh", edit);
	if (edit)
	{
		for (auto& agent : agents)
		{
			agent->myNavMeshToUse = newNavMesh;
		}
	}

	if (Catbox::DragFloat("Speed", &mySpeed, 0.1f, 0, 100))
	{
		for (auto& agent : agents)
		{
			agent->mySpeed = mySpeed;
		}
	}

	if (ImGui::Button("Stop navigation")) myTargetReached = true;

	float target[3] = { myTargetPosition.x, myTargetPosition.y, myTargetPosition.z };
	if (ImGui::DragFloat3("Target", target, 0.05f))
	{

		myTargetPosition.x = target[0];
		myTargetPosition.y = target[1];
		myTargetPosition.z = target[2];
		for (size_t i = 0; i < aComponentList.size(); i++)
		{
			agents[i]->myTargetPosition = myTargetPosition;
		}
	}

	if (ImGui::Checkbox("Is In Crowd", &myIsInCrowd))
	{
		for (int i = 0; i < agents.size(); i++)
		{
			agents[i]->myIsInCrowd = myIsInCrowd;
		}
	}

	if (myIsInCrowd)
	{
		ImGui::InputInt("Crowd Id", &myCrowdId);
		ImGui::Text("Agent Id", &myAgentId);

		if (Catbox::DragInt("Avoidance Type", &myAvoidianceType))
		{
			for (int i = 0; i < agents.size(); i++)
			{
				agents[i]->myAvoidianceType = myAvoidianceType;
			}
		}
		if (Catbox::DragFloat("Radius", &myRadius, 0.2f, 0.1f, 5.0f))
		{
			for (int i = 0; i < agents.size(); i++)
			{
				agents[i]->myRadius = myRadius;
			}
		}

	}

	if (ImGui::Button("FindPath"))
	{
		for (int i = 0; i < agents.size(); i++)
		{
			agents[i]->TestTarget();
		}
	}
	if (ImGui::Button("RequestMove"))
	{
		Engine::GetInstance()->GetCrowdManager()->RequestMove(myCrowdId, myAgentId, myPath[myPathIndex], Vector3f(10.0f, 10.0f, 10.0f), &myDtFilter);
		++myPathIndex;

	}
}

Vector3f NavAgent::GetCurrentDirection() const
{
	if (myIsInCrowd && !myTargetReached && !myPathFailed)
	{
		auto agent = Engine::GetInstance()->GetCrowdManager()->GetAgent(myCrowdId, myAgentId);
		if (agent)
		{
			return Vector3f(agent->targetPos[0], agent->targetPos[1], agent->targetPos[2]);
		}
	}
	else if (!myTargetReached && !myPathFailed)
	{
		return myDir;
	}
	return Vector3f::zero();
}

Vector3f NavAgent::GetActorDirection() const
{
	if (IsInCrowd())
	{
		auto agent = Engine::GetInstance()->GetCrowdManager()->GetAgent(myCrowdId, myAgentId);
		if (agent)
		{
			return Vector3f(agent->nvel[0], agent->nvel[1], agent->nvel[2]);
		}
		return myTransform->worldPos().GetNormalized();
	}
	else
	{
		return myDir;
	}
}

Vector3f NavAgent::GetAgentTarget() const
{
	if (myIsInCrowd)
	{
		const dtCrowdAgent* agent = Engine::GetInstance()->GetCrowdManager()->GetAgent(myCrowdId, myAgentId);
		if (agent)
		{
			return Vector3f(agent->targetPos[0], agent->targetPos[1], agent->targetPos[2]);
		}
	}
	return Vector3f::zero();
}

std::shared_ptr<NavMesh> NavAgent::GetNavMesh() const
{
	return myNavMeshToUse;
}

Vector3f NavAgent::GetAgentPos() const
{
	if (IsInCrowd())
	{
		auto agent = Engine::GetInstance()->GetCrowdManager()->GetAgent(myCrowdId, myAgentId);
		return Vector3f(agent->npos[0], agent->npos[1] - myNavMeshToUse->GetBakingSettings().agentHeight * 0.5f, agent->npos[2]);
	}
	return Vector3f::zero();
}

void NavAgent::TestTarget()
{
	FindPath(myTargetPosition, true);
	if (myIsInCrowd && !myPathFailed)
	{
		//for (int i = 0; i < myPath.size(); ++i)
		//{
		Engine::GetInstance()->GetCrowdManager()->RequestMove(myCrowdId, myAgentId, myPath[myPathIndex], Vector3f(10.0f, 10.0f, 10.0f), &myDtFilter);
		//}
	}
}

void NavAgent::Save(rapidjson::Value& /*aComponentData*/)
{
	int version = 1;
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Int>("Version", version);

	const char* navName = myNavMeshToUse ? myNavMeshToUse->GetName().c_str() : "";
	wrapper.SaveString("NavMesh", navName);
	wrapper.SaveValue<DataType::Float>("Speed", mySpeed);
	wrapper.SaveValue<DataType::Bool>("Is In Crowd", myIsInCrowd);
	if (myIsInCrowd)
	{
		wrapper.SaveValue<DataType::Int>("CrowdId", myCrowdId);
	}

}

void NavAgent::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Version"))
	{
		//int version = aComponentData["Version"].GetInt();
		mySpeed = aComponentData["Speed"].GetFloat();
		myNavMeshToUse = AssetRegistry::GetInstance()->GetAsset<NavMesh>(aComponentData["NavMesh"].GetString());
		if (aComponentData["Version"].GetInt() > 0)
		{
			myIsInCrowd = aComponentData["Is In Crowd"].GetBool();
			if (myIsInCrowd)
			{
				myCrowdId = aComponentData["CrowdId"].GetInt();
			}
		}
	}
}

void NavAgent::PausePathfinding()
{
	myTargetReached = true;
}

void NavAgent::StopPathFinding()
{
	myTargetReached = true;
	myPath.clear();
	if (myIsInCrowd)
	{
		const dtCrowdAgent* agent = Engine::GetInstance()->GetCrowdManager()->GetAgent(myCrowdId, myAgentId);
		Engine::GetInstance()->GetCrowdManager()->ResetMove(myCrowdId, myAgentId);
	}
}