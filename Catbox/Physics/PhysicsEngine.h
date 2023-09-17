#pragma once
#include <physX\PxPhysicsAPI.h>
enum Shape
{
	PxS_Box,
	PxS_Circle,
	PxS_Capsule,
	PxS_Convex,
	PxS_Mesh,
	PxS_Count
};

enum ParticleType
{
	PxPS_Fluid,
	PxPS_Cloth
};

struct FilterGroup
{
	enum Filters
	{
		PxF_Default = (1 << 0),
		PxF_Enemy = (1 << 1),
		PxF_WhateverIDC = (1 << 2)
	};
};

class DefaultCCFCallback : public physx::PxControllerFilterCallback
{
	bool filter(const physx::PxController& a, const physx::PxController& b);
};

class DefaultSimulationCallback : public physx::PxSimulationEventCallback
{
public:
	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override;

	void onWake(physx::PxActor** actors, physx::PxU32 count) override;

	void onSleep(physx::PxActor** actors, physx::PxU32 count) override;

	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;

	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;

	void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override;
};

class DefaultCharacterControllerCallback : public physx::PxUserControllerHitReport
{
public:
	void onShapeHit(const physx::PxControllerShapeHit& hit);
	void onControllerHit(const physx::PxControllersHit& hit);
	void onObstacleHit(const physx::PxControllerObstacleHit& hit);
};

class PhysicsEngine
{
public:
	friend class DefaultSimulationCallback;
	friend class DefaultCharacterControllerCallback;
	friend class DefaultCCFCallback;
	void Init();
	void Update();
	void DrawLines();
	void ToggleDebugDrawing(bool aFlag);
	void AddActor(physx::PxRigidActor* aActor, const int& aId, const int aLayerID = 0);
	void AddParticleSystem(physx::PxParticleSystem* aParticleSystem, const int& aId);
	void ResetScene();
	void SetDebugSettings(const std::array<bool, 6>& debugSettings);

	inline double GetSimulationFrameTime() const { return mySimulationTime; }
	physx::PxScene* GetScene() { return myPxScene; }

	physx::PxRigidActor* CreateDynamicActor(Shape aShape = Shape::PxS_Box, const Vector3f& aScale = { 1,1,1 }, Model* aMeshdata = nullptr, const std::string& aMaterialName = "Default", float aMass = 1, bool aDebugMode = false, const Vector3f& aTransformSize = { 1,1,1 }, bool aIsTrigger = false);
	physx::PxRigidStatic* CreateStaticActor(Shape aShape = Shape::PxS_Box, const Vector3f& aScale = { 1,1,1 }, Model* aMeshdata = nullptr, const std::string& aMaterialName = "Default", bool aDebugMode = false, const Vector3f& aTransformSize = { 1,1,1 }, const Vector3f& aLocalRotation = { 0,0,0 }, bool aIsTrigger = false);
	physx::PxRigidStatic* CreateStaticNavmesh(MeshData aMeshData);
	physx::PxPBDParticleSystem* CreateParticleSystem(ParticleType = ParticleType::PxPS_Fluid);
	physx::PxController* CreateCharacterController(Shape aShape = Shape::PxS_Box, const Vector3f& aSize = { 1,1,1 }, float aDensity = 1.0f, const std::string& aMaterialName = "Default", int aId = -1);
	physx::PxMaterial* CreateMaterial(const std::string& aName, const Vector3f&aMaterial);
	physx::PxMaterial* GetMaterial(const std::string& aName);
	physx::PxMaterial* EditMaterial(const std::string& aName, const Vector3f& aMaterial);
	physx::PxControllerFilterCallback* GetCCFFilterCallback() { return myCCFCallback; };
	void RemoveActor(physx::PxRigidActor* anActor) { myActorsToRemove.push_back(anActor); };

	void RemoveActor(physx::PxRigidStatic* anActor) { myActorsToRemove.push_back(anActor); };

	void CreateJoint(physx::PxRigidActor* actor0, const physx::PxTransform& localFrame0, physx::PxRigidActor* actor1, const physx::PxTransform& localFrame1);

	void CreateAdditionalTriggerBox(Vector3f aSize, Vector3f anOffset, physx::PxRigidActor* anActor, const int aLayerID = 0);
	void RemoveShapeFromActor(physx::PxRigidActor* anActor, const int aShapeID);

	void SetShapeLayer(physx::PxRigidActor* anActor, const int aShapeID, const int aLayerID);

	Color GetColorFromIndex(uint16_t aIndex);

protected:
	std::vector<std::function<void(Collider*)>> myCallbacks;
	std::vector<std::function<void()>> myCCTCallbacks;
	std::vector<Collider*> myCallbacksParameters;
private:
	void InitScene();

	physx::PxConvexMesh* CookConvexMesh(MeshData& aMeshData, const Vector3f&aSize = { 1,1,1 });
	physx::PxConvexMesh* CookConvexMesh(std::vector<Vertex>& aMeshData, const Vector3f&aSize = { 1,1,1 });
	physx::PxTriangleMesh* CookTriangleMesh(const MeshData& aMeshData, const Vector3f&aSize = { 1,1,1 });

	void SendCallbacks();

	bool myResetingScene = false;
	bool myFirstFrame = true;
	float mySteppedTimer = 0.0f;
	float myStepSize = 1.0f / 60.0f;
	float mySteps = 1;

	physx::PxScene* myPxScene = NULL;
	physx::PxDefaultAllocator myPxDefaultAllocatorCallback;
	physx::PxDefaultErrorCallback myPxDefaultErrorCallback;
	physx::PxFoundation* myPxfoundation = NULL;
	physx::PxCooking* myCooker = NULL;
	physx::PxSimulationEventCallback* mySimCallback = NULL;
	physx::PxUserControllerHitReport* myCCTCallback = NULL;
	physx::PxControllerFilterCallback* myCCFCallback = NULL;
	physx::PxMaterial* myPxDefaultMaterial = NULL;
	physx::PxParticleMaterial* myDefaultParticleMaterial = NULL;
	physx::PxTolerancesScale myPxToleranceScale;
	physx::PxPvd* myPxPvd = NULL;
	physx::PxPhysics* myPxPhysics = NULL;
	physx::PxDefaultCpuDispatcher* myPxDispatcher = NULL;
	physx::PxControllerManager* myCharacterController = NULL;
	physx::PxCudaContextManager* myCudaManager = NULL;

	std::vector<physx::PxShape*> myShapes;
	std::vector<physx::PxActor*> myAddActorsInRuntime;
	std::vector<physx::PxActor*> myActorsToRemove;

	float myDebugRenderTimer = 0;
	std::vector<physx::PxActor*> myActors;
	std::vector<physx::PxActor*> myNavMeshStatics;
	std::unordered_map<std::string, physx::PxMaterial*> myPhysxMaterialMap;

	double mySimulationTime = 0;
};

inline physx::PxFilterFlags DefaultPxFilterFlags(
	physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	// let triggers through
	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags |= physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}
	if (physx::PxFilterObjectIsKinematic(attributes0) || physx::PxFilterObjectIsKinematic(attributes1))
	{
		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
			pairFlags |= physx::PxPairFlag::eCONTACT_DEFAULT;
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
			pairFlags |= physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
			return physx::PxFilterFlag::eDEFAULT;
		}
	}

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
		pairFlags |= physx::PxPairFlag::eCONTACT_DEFAULT;
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
		pairFlags |= physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
	}
	return physx::PxFilterFlag::eDEFAULT;
}
