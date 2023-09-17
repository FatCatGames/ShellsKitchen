#pragma once
#include "NavMesh.h"
#include "DetourNavMeshQuery.h"

struct dtCrowdAgentParams;

enum ActorState
{
	dtActorState_OffMesh,
	dtActorState_NotActive
};

class NavAgent : public Component
{
public:
	~NavAgent();
	void Init() override;
	void Awake() override;
	void Update() override;
	void RunInEditor() override;
	void RequestMove(Vector3f aTarget);
	bool FindPath(const Vector3f& aTarget, bool forcePathFinding = false);
	void ChangeSpeed(float aMovmentSpeed);
	float GetSpeed() { return mySpeed; }
	void PausePathfinding();
	void StopPathFinding();
	void ResumePathfinding();
	ActorState GetAgentState();
	//float GetAgentSpeed() const;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	bool ReachedTarget() const { return myTargetReached; }
	Vector3f GetCurrentDirection() const;
	Vector3f GetActorDirection() const;
	Vector3f GetAgentTarget() const;
	std::shared_ptr<NavMesh> GetNavMesh() const;
	bool HasTarget() const { if (myPathIndex > myPath.size()) { return false; } else { return true; } }
	Vector3f GetAgentPos() const;
	Vector3f GetTarget() const;
	Vector3f GetEndTarget() const;
	void ResetAgent();
	bool IsInCrowd() const { return myIsInCrowd; }
	void SetNavMesh(std::shared_ptr<NavMesh> aNavMesh) { myNavMeshToUse = aNavMesh; }
	void SetUsingCharacterController(bool aFlag) { myUsingCharacterController = aFlag; }
	dtQueryFilter GetDtFilter() const { return myDtFilter; }

	void TestTarget();
	
private:

	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	float mySpeed = 5;

	std::shared_ptr<NavMesh> myNavMeshToUse = nullptr;
	std::vector<Vector3f> myPath;
	Vector3f myCrowdTarget;
	//std::vector<NavMesh::Connection*> path;
	//NavMesh::Connection end;
	Vector3f myStartPoint;
	std::vector<Vector3f> myLeftFunnelPoints;
	std::vector<Vector3f> myRightFunnelPoints;
	dtCrowdAgentParams* myParams = nullptr;

	Vector3f portalLeft;
	Vector3f portalApex;
	Vector3f portalRight;
	bool drawRightNext = true;
	int myFunnelPointIndex = 0;

	int myCrowdId = 1;
	int myAgentId = -1;
	float myRadius = 0.5f;
	int myAvoidianceType = 1;

	dtQueryFilter myDtFilter;

	const int myMaxPathSize = 255;
	bool myUsingCharacterController = false;
	bool myIsInCrowd = false;
	int myPathIndex = 0;
	Vector3f myDir;
	Vector3f myTargetPosition = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
	bool myTargetReached = true;
	bool myPathFailed = false;
	float myAngleY = 0;
};