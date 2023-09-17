#pragma once

class dtCrowd;
class NavMesh;
class dtQueryFilter;
class dtCrowdAgent;
struct dtCrowdAgentParams;

class CrowdManager
{
public:
	void AddCrowd(int aId, std::shared_ptr<NavMesh> aNavmesh);
	void RemoveAgent(int aCrowdId, int aAgentId);
	int AddAgentToCrowd(int aCrowdId, const Vector3f& aPos, const Vector3f& aSize, dtQueryFilter* aFilter, float aMovementSpeed, int aAvodianceType, float aRadius, dtCrowdAgentParams* aParams = nullptr);
	const dtCrowdAgent* GetAgent(int aCrowdId, int aAgentId);
	dtCrowdAgent* GetEditableAgent(int aCrowdId, int aAgentId);
	void UpdateAgentParamaters(int aCrowdId, int aAgentId, dtCrowdAgentParams* params);
	bool RequestMove(int aCrowdId, int aAgentId, const Vector3f& aPos, const Vector3f& aSize, dtQueryFilter* aFilter);
	bool ResetMove(int aCrowdId, int aAgentId);

	bool CrowdExists(int aCrowdId) { return (myCrowds.find(aCrowdId) != myCrowds.end()); }

	void Update();

	void ClearCrowds();
private:
	std::unordered_map<short, dtCrowd*> myCrowds;
	std::unordered_map<short, std::shared_ptr<NavMesh>> myCrowdNavmeshes;
	int myCrowdIdCounter = 0;
};

