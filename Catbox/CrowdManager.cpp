#include "stdafx.h"
#include "CrowdManager.h"
#include "Navigation/NavMesh.h"
#include "DetourCrowd.h"


void CrowdManager::AddCrowd(int aId, std::shared_ptr<NavMesh> aNavmesh)
{
	dtCrowd* newCrowd = dtAllocCrowd();
	newCrowd->init(100, 10, const_cast<dtNavMesh*>(aNavmesh->GetDtNavMesh()));
	// Setup local avoidance params to different qualities.
	dtObstacleAvoidanceParams params;
	// Use mostly default settings, copy from dtCrowd.
	memcpy(&params, newCrowd->getObstacleAvoidanceParams(0), sizeof(dtObstacleAvoidanceParams));
	// Low (11)
	params.velBias = 0.5f;
	params.adaptiveDivs = 5;
	params.adaptiveRings = 2;
	params.adaptiveDepth = 1;
	newCrowd->setObstacleAvoidanceParams(0, &params);

	// Medium (22)
	params.velBias = 0.5f;
	params.adaptiveDivs = 5;
	params.adaptiveRings = 2;
	params.adaptiveDepth = 2;
	newCrowd->setObstacleAvoidanceParams(1, &params);

	// Good (45)
	params.velBias = 0.5f;
	params.adaptiveDivs = 7;
	params.adaptiveRings = 2;
	params.adaptiveDepth = 3;
	newCrowd->setObstacleAvoidanceParams(2, &params);

	// High (66)
	params.velBias = 0.5f;
	params.adaptiveDivs = 7;
	params.adaptiveRings = 3;
	params.adaptiveDepth = 3;
	newCrowd->setObstacleAvoidanceParams(3, &params);
	myCrowds.insert({aId, newCrowd});
	myCrowdNavmeshes.insert({ aId, aNavmesh });
}

void CrowdManager::RemoveAgent(int aCrowdId, int aAgentId)
{
	if (myCrowds.find(aCrowdId) != myCrowds.end())
	{
		myCrowds[aCrowdId]->removeAgent(aAgentId);
	}
}

int CrowdManager::AddAgentToCrowd(int aCrowdId, const Vector3f& aPos, const Vector3f& aSize, dtQueryFilter* aFilter, float aMovementSpeed, int aAvodianceType, float aRadius, dtCrowdAgentParams* aParams)
{
	if (myCrowds.find(aCrowdId) != myCrowds.end()) 
	{
		auto settings = myCrowdNavmeshes[aCrowdId]->GetBakingSettings();
		dtCrowdAgentParams* ap = new dtCrowdAgentParams;
		ap->radius = aRadius;
		ap->height = settings.agentHeight;
		ap->maxAcceleration = aMovementSpeed;
		ap->maxSpeed = aMovementSpeed;
		ap->collisionQueryRange = ap->radius * 12.0f;
		ap->pathOptimizationRange = ap->radius * 1.0f;
		ap->updateFlags = 0;

		ap->queryFilterType = 

		//if (m_toolParams.m_anticipateTurns)
		ap->updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
		//if (m_toolParams.m_optimizeVis)
		ap->updateFlags |= DT_CROWD_OPTIMIZE_VIS;
		//if (m_toolParams.m_optimizeTopo)
		ap->updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
		//if (m_toolParams.m_obstacleAvoidance)
		ap->updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
		//if (m_toolParams.m_separation)
		ap->updateFlags |= DT_CROWD_SEPARATION;

		ap->obstacleAvoidanceType = (unsigned char)(aAvodianceType);
		ap->separationWeight = 1.0f;

		float pos[3];
		pos[0] = aPos.x;
		pos[1] = aPos.y;
		pos[2] = aPos.z;


		float tarPos[3];
		tarPos[0] = aPos.x;
		tarPos[1] = aPos.y;
		tarPos[2] = aPos.z;

		float size[3];
		size[0] = 1.0f;
		size[1] = 1.0f;
		size[2] = 1.0f;

		float nearestPos[3];

		dtPolyRef aRef = 0;
		int counter = 1;

		myCrowdNavmeshes[aCrowdId]->GetDtNavMeshQuery()->findNearestPoly(pos, size, aFilter, &aRef, tarPos);

		int idx = myCrowds[aCrowdId]->addAgent(tarPos, ap);
		if (idx != -1)
		{

			if (myCrowds[aCrowdId]->getAgent(idx)->state == DT_CROWDAGENT_STATE_INVALID) 
			{
				print("Invalid");
			}
			//float size[3];
			//size[0] = aSize.x;
			//size[1] = aSize.y;
			//size[2] = aSize.z;

			dtPolyRef nearest = 0;
			

			//auto targetRef = myCrowdNavmeshes[aCrowdId]->GetDtNavMeshQuery()->findNearestPoly(pos, size, aFilter, &nearest, tarPos);
			//if (targetRef)
			//{
			//	myCrowds[aCrowdId]->requestMoveTarget(idx, nearest, tarPos);
			//}	
			aParams = ap;
			return idx;
		}
	}
	return -1;
}

const dtCrowdAgent* CrowdManager::GetAgent(int aCrowdId, int aAgentId)
{
	return myCrowds[aCrowdId]->getAgent(aAgentId);
}
dtCrowdAgent* CrowdManager::GetEditableAgent(int aCrowdId, int aAgentId)
{
	return myCrowds[aCrowdId]->getEditableAgent(aAgentId);
}

void CrowdManager::UpdateAgentParamaters(int aCrowdId, int aAgentId, dtCrowdAgentParams* params)
{
	if (aCrowdId <= myCrowds.size())
	{
		myCrowds[aCrowdId]->updateAgentParameters(aAgentId, params);
	}
}

bool CrowdManager::RequestMove(int aCrowdId, int aAgentId, const Vector3f& aPos, const Vector3f& aSize, dtQueryFilter* aFilter)
{
	if (aCrowdId <= myCrowds.size())
	{
		//switch (myCrowds[aCrowdId]->getAgent(aAgentId)->targetState)
		//{
		//case DT_CROWDAGENT_TARGET_NONE:
		//{
		//	print("DT_CROWDAGENT_TARGET_NONE");
		//	break;
		//}
		//case DT_CROWDAGENT_TARGET_FAILED:
		//{
		//	print("DT_CROWDAGENT_TARGET_FAILED");
		//	break;
		//}
		//case DT_CROWDAGENT_TARGET_VALID:
		//{
		//	print("DT_CROWDAGENT_TARGET_VALID");
		//	break;
		//}
		//case DT_CROWDAGENT_TARGET_REQUESTING:
		//{
		//	print("DT_CROWDAGENT_TARGET_REQUESTING");
		//	break;
		//}
		//case DT_CROWDAGENT_TARGET_WAITING_FOR_QUEUE:
		//{
		//	print("DT_CROWDAGENT_TARGET_WAITING_FOR_QUEUE");
		//	break;
		//}
		//case DT_CROWDAGENT_TARGET_WAITING_FOR_PATH:
		//{
		//	print("DT_CROWDAGENT_TARGET_WAITING_FOR_PATH");
		//	break;
		//}
		//case DT_CROWDAGENT_TARGET_VELOCITY:
		//{
		//	print("DT_CROWDAGENT_TARGET_VELOCITY");
		//	break;
		//}
		//default:
		//	break;
		//}
		float pos[3]{ aPos.x, aPos.y, aPos.z };
		float size[3]{ aSize.x, aSize.y, aSize.z };
		dtPolyRef aRef = 0;
		float tarPos[3];
		int counter = 1;

		while (aRef == 0)
		{
			size[0] += counter;
			size[1] += counter;
			size[2] += counter;
			myCrowdNavmeshes[aCrowdId]->GetDtNavMeshQuery()->findNearestPoly(pos, size, aFilter, &aRef, tarPos);
			counter++;
		}

		bool returnValue = myCrowds[aCrowdId]->requestMoveTarget(aAgentId, aRef, tarPos);
		//if (!returnValue)
		//{
		//	myCrowds[aCrowdId]->resetMoveTarget(aAgentId);
		//}
		return returnValue;
	}
	return false;
}

bool CrowdManager::ResetMove(int aCrowdId, int aAgentId)
{
	if (aCrowdId <= myCrowds.size())
	{
		myCrowds[aCrowdId]->resetMoveTarget(aAgentId);
		auto agent = myCrowds[aCrowdId]->getEditableAgent(aAgentId);
		agent->dvel[0] = 0;
		agent->dvel[1] = 0;
		agent->dvel[2] = 0;
		agent->nvel[0] = agent->vel[0] * 0.2f;
		agent->nvel[1] = agent->vel[0] * 0.2f;
		agent->nvel[2] = agent->vel[0] * 0.2f;
		agent->vel[0] = agent->vel[0] * 0.2f;
		agent->vel[1] = agent->vel[0] * 0.2f;
		agent->vel[2] = agent->vel[0] * 0.2f;

		return true;
	}
	return false;
}

void CrowdManager::Update()
{
	PerformanceManager::BeginTimer("Crowd Manager", "Engine");
	for (auto& crowd : myCrowds)
	{
		if (crowd.second) 
		{
			crowd.second->update(deltaTime, nullptr);
			auto path = crowd.second->getPathQueue();
		}
	}
	PerformanceManager::StopTimer("Crowd Manager");
}

void CrowdManager::ClearCrowds()
{
	myCrowds.clear();
	myCrowdNavmeshes.clear();
	myCrowdIdCounter = 0;
}
