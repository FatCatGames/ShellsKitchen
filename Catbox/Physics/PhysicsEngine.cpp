#include "stdafx.h"
#include "PhysicsEngine.h"
#include "Assets/Model.h"
#include "physX/cudamanager/PxCudaContext.h"
#include "Components/Physics/Collisions/BoxCollider.h"
#include "Components/Physics/Collisions/ConvexCollider.h"
#include "Components/Physics/Collisions/SphereCollider.h"
#include "Components/Physics/Collisions/MeshCollider.h"
#include "Components/Physics/Collisions/CapsuleCollider.h"
#include "Components/Physics/Collisions/CollisionManager.h"

void PhysicsEngine::Init()
{
	mySimCallback = new DefaultSimulationCallback();
	myCCTCallback = new DefaultCharacterControllerCallback();
	myCCFCallback = new DefaultCCFCallback();

	myPxfoundation = PxCreateFoundation(PX_PHYSICS_VERSION, myPxDefaultAllocatorCallback, myPxDefaultErrorCallback);
	assert(myPxfoundation != nullptr && "COULD NOT CREATE PHYSX FOUNDATION");
	myPxPvd = physx::PxCreatePvd(*myPxfoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	//myPxPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	physx::PxTolerancesScale tolerancesScale;
	myPxToleranceScale = tolerancesScale;
	myPxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *myPxfoundation, myPxToleranceScale, true, myPxPvd);
	physx::PxCookingParams aParams = physx::PxCookingParams(myPxToleranceScale);
	myCooker = PxCreateCooking(PX_PHYSICS_VERSION, *myPxfoundation, aParams);
	myPxDispatcher = physx::PxDefaultCpuDispatcherCreate(2);


	physx::PxCudaContextManagerDesc cudaContextManager;
	myCudaManager = PxCreateCudaContextManager(*myPxfoundation, cudaContextManager, PxGetProfilerCallback());
	myDefaultParticleMaterial = myPxPhysics->createPBDMaterial(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	myPxDefaultMaterial = myPxPhysics->createMaterial(1.0f, 1.0f, 0.0f);
	myPhysxMaterialMap.insert({ "Default", myPxDefaultMaterial });
	InitScene();
}

void PhysicsEngine::Update()
 {
	if (deltaTime > 0 && !myResetingScene && !myFirstFrame)
	{
		if (myPxScene)
		{
			for (int i = 0; i < mySteps; i++)
			{
				myPxScene->simulate(myStepSize / mySteps);

				if (myPxScene->fetchResults(true))
				{
					for (size_t i = 0; i < myActorsToRemove.size(); i++)
					{
						if (myActorsToRemove[i]->isReleasable())
						{
							myActorsToRemove[i]->release();
						}
					}
					myActorsToRemove.clear();
				}
			}
		}
	}

	myFirstFrame = false;
 }

void PhysicsEngine::DrawLines()
{
	if (myPxScene && EDITORMODE && Engine::GetInstance()->GetGraphicsEngine()->GetDebugDrawerToggle())
	{
		const physx::PxRenderBuffer& aBuffer = myPxScene->getRenderBuffer();

		
		for (physx::PxU32 i = 0; i < aBuffer.getNbPoints(); i++)
		{
			const physx::PxDebugPoint& debugPoint = aBuffer.getPoints()[i];
			const Vector3f& aPoint = { debugPoint.pos.x, debugPoint.pos.y, debugPoint.pos.z };
			Color color = GetColorFromIndex(debugPoint.color);
			DebugDrawer::DrawSphere(aPoint, 0.5f, color, 5.0f, 0.1f);
		}
		for (physx::PxU32 i = 0; i < aBuffer.getNbLines(); i++)
		{
			const physx::PxDebugLine& debugLine = aBuffer.getLines()[i];
			const Vector3f& aVector0 = { debugLine.pos0.x,debugLine.pos0.y, debugLine.pos0.z };
			const Vector3f& aVector1 = { debugLine.pos1.x,debugLine.pos1.y, debugLine.pos1.z };
			Color color = GetColorFromIndex(debugLine.color0);
			DebugDrawer::DrawLine(aVector0, aVector1, color);
		}
		for (physx::PxU32 i = 0; i < aBuffer.getNbTriangles(); i++)
		{
			const physx::PxDebugTriangle& debugTriangle = aBuffer.getTriangles()[i];
			const Vector3f& aVector0 = { debugTriangle.pos0.x,debugTriangle.pos0.y, debugTriangle.pos0.z };
			const Vector3f& aVector1 = { debugTriangle.pos1.x,debugTriangle.pos1.y, debugTriangle.pos1.z };
			const Vector3f& aVector2 = { debugTriangle.pos2.x,debugTriangle.pos2.y, debugTriangle.pos2.z };
			Color color0 = GetColorFromIndex(debugTriangle.color0);
			Color color1 = GetColorFromIndex(debugTriangle.color1);
			Color color2 = GetColorFromIndex(debugTriangle.color2);
			DebugDrawer::DrawLine(aVector0, aVector1, color0);
			DebugDrawer::DrawLine(aVector1, aVector2, color1);
			DebugDrawer::DrawLine(aVector0, aVector2, color2);
		}
	}
}

void PhysicsEngine::ToggleDebugDrawing(bool aFlag)
{
	if (myPxScene) 
	{
		if (!aFlag) 
		{
			myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 0.0f);
		}
		else 
		{
			myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
		}
	}
}

void PhysicsEngine::AddActor(physx::PxRigidActor* aActor, const int& aId, const int aLayerID)
{
	if (aActor && myPxScene)
	{
		physx::PxFilterData filterData;

		filterData.word0 = 1 << aLayerID;
		filterData.word1 = 0;

		CollisionManager* aCollisionManager = Engine::GetInstance()->GetCollisionManager().get(); //Let it dangle then, idc

		for (size_t i = 0; i < aCollisionManager->GetPhysicsCollisionLayers()[aLayerID].size(); i++)
		{
			if (aCollisionManager->GetPhysicsCollisionLayers()[aLayerID][i])
			{
				filterData.word1 |= 1 << i;
			}
		}

		const physx::PxU32 numShapes = aActor->getNbShapes();
		physx::PxShape** aShapeArray = (physx::PxShape**)malloc(sizeof(physx::PxShape*) * numShapes);
		aActor->getShapes(aShapeArray, numShapes);
		for (int i = 0; i < numShapes; i++)
		{
			physx::PxShape* shape = aShapeArray[i];
			shape->setSimulationFilterData(filterData);
			shape->setQueryFilterData(filterData);
			if (shape->getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE)	//Bro imagine actually reading what stuff does before writing it??
			{
				shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
			}
			else
			{
				shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
			}
		}
		free(aShapeArray);

		aActor->userData = (void*)aId;

		myPxScene->addActor(*aActor);
	}
	else 
	{
		printerror("Could not add Actor to PxScene, probaly Actor was nullptr");
	}
}

void PhysicsEngine::AddParticleSystem(physx::PxParticleSystem* aParticleSystem, const int& aId)
{
	aParticleSystem->userData = (void*)aId;
	myPxScene->addActor(*aParticleSystem);
	physx::PxParticleBuffer* particleBuffer = myPxPhysics->createParticleBuffer(1000, 1000, myCudaManager);
	physx::PxVec4* bufferPos = particleBuffer->getPositionInvMasses();
	myCudaManager->getCudaContext()->memcpyDtoHAsync(bufferPos, myCudaManager->getDevice(), 1000 * sizeof(physx::PxVec4), 0);
	particleBuffer->raiseFlags(physx::PxParticleBufferFlag::eUPDATE_POSITION);

	particleBuffer->setNbActiveParticles(1000);
	aParticleSystem->addParticleBuffer(particleBuffer);
}


void PhysicsEngine::ResetScene()
{
	myResetingScene = true;
	myFirstFrame = true;
	for (int i = 0; i < myActors.size(); i++)
	{
		if (myActors[i]->isReleasable()) 
		{
			myActors[i]->release();
		}
	}
	myActors.clear();
	for (size_t i = 0; i < myActorsToRemove.size(); i++)
	{
		if (myActorsToRemove[i]->isReleasable())
		{
			myActorsToRemove[i]->release();
		}
	}
	myActorsToRemove.clear();
	myPxScene = nullptr;
	InitScene();
}

void PhysicsEngine::SetDebugSettings(const std::array<bool, 6>& debugSettings)
{
	myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, debugSettings[0]);
	myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eCONTACT_POINT, debugSettings[1]);
	myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eCONTACT_FORCE, debugSettings[2]);
	myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eCONTACT_NORMAL, debugSettings[3]);
	myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_ANG_VELOCITY, debugSettings[4]);
	myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_LIN_VELOCITY, debugSettings[5]);
}

physx::PxRigidActor* PhysicsEngine::CreateDynamicActor(Shape aShape, const Vector3f& aScale, Model* aModel, const std::string& aMaterialName, float aMass, bool aDebugMode, const Vector3f& aTransformSize, bool aIsTrigger)
{
	physx::PxMaterial* mat = nullptr;

	if (myPhysxMaterialMap.find(aMaterialName) == myPhysxMaterialMap.end())
	{
		mat = myPhysxMaterialMap.find("Default")->second;
	}
	else
	{
		mat = myPhysxMaterialMap.find(aMaterialName)->second;
	}

	const Vector3f& aSize = (aScale /*(aModel->GetPivot()) * aScale*/) * 0.5f;
	physx::PxShape* actorShape = nullptr;
	std::vector<physx::PxConvexMesh*> aMesh;

	physx::PxFilterData filterData;
	filterData.word0 = 0; // word0 = own ID
	filterData.word1 = 1;
	if (aShape == Shape::PxS_Box)
	{
		if (aIsTrigger)
		{
			actorShape = myPxPhysics->createShape(physx::PxBoxGeometry(aSize.x * aTransformSize.x, aSize.y * aTransformSize.y, aSize.z * aTransformSize.z), *mat, false, physx::PxShapeFlag::eTRIGGER_SHAPE);
		}
		else
		{
			actorShape = myPxPhysics->createShape(physx::PxBoxGeometry(aSize.x * aTransformSize.x, aSize.y * aTransformSize.y, aSize.z * aTransformSize.z), *mat);
		}
	}
	else if (aShape == Shape::PxS_Circle)
	{
		if (aIsTrigger)
		{
			actorShape = myPxPhysics->createShape(physx::PxSphereGeometry(std::max(std::max(aSize.x, aSize.y), aSize.z)), *mat, false, physx::PxShapeFlag::eTRIGGER_SHAPE);
		}
		else
		{
			actorShape = myPxPhysics->createShape(physx::PxSphereGeometry(std::max(std::max(aSize.x, aSize.y), aSize.z)), *mat);
		}
	}
	else if (aShape == Shape::PxS_Capsule)
	{
		if (aIsTrigger) 
		{
			actorShape = myPxPhysics->createShape(physx::PxCapsuleGeometry(aScale.x, aScale.y * 0.5f), *mat, false, physx::PxShapeFlag::eTRIGGER_SHAPE);
		}
		else 
		{
			actorShape = myPxPhysics->createShape(physx::PxCapsuleGeometry(aScale.x, aScale.y * 0.5f), *mat);
		}
	}
	else if (aShape == Shape::PxS_Convex)
	{
		for (int i = 0; i < aModel->GetModelParts().size(); i++)
		{
			for (int j = 0; j < std::ceil(aModel->GetModelPart(i)->myVertices.size() / 255); j++)
			{
				int aIndex = 255;
				if (255 * j > aModel->GetModelPart(i)->myVertices.size())
				{
					aIndex = aModel->GetModelPart(i)->myVertices.size();
				}
				std::vector<Vertex> aVertexVector = std::vector<Vertex>(aModel->GetModelPart(i)->myVertices.begin(), aModel->GetModelPart(i)->myVertices.begin() + aIndex);
				aMesh.push_back(CookConvexMesh(aVertexVector, aSize * 2.0f));

			}
		}
	}
	if (!actorShape)
	{
		printerror("PxShape could not be created!");
	}
	physx::PxTransform globalTransfrom = { 0,0,0 };
	physx::PxTransform aLocalTransform = { 0,0,0 };
	if (aModel)
	{
		aLocalTransform = { (aModel->GetCenter().x * aTransformSize.x),(aModel->GetCenter().y * aTransformSize.y),(aModel->GetCenter().z * aTransformSize.z)};
	}
	if (aShape == PxS_Capsule) 
	{
		aLocalTransform.q = physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1));
	}

	physx::PxRigidActor* returnActor = myPxPhysics->createRigidDynamic(globalTransfrom);

	if (aShape != Shape::PxS_Convex)
	{
		actorShape->setLocalPose(aLocalTransform);
		actorShape->setSimulationFilterData(filterData);
		actorShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, aDebugMode);
	}
	else
	{
		for (int i = 0; i < aMesh.size(); i++)
		{
			if (actorShape)
			{
				returnActor->attachShape(*actorShape);
				actorShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, aDebugMode);
			}
			actorShape = physx::PxRigidActorExt::createExclusiveShape(*returnActor, physx::PxConvexMeshGeometry(aMesh[i]), *mat);
		}
	}
	if (returnActor)
	{
		//actorShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
		returnActor->attachShape(*actorShape);

	}

	if (actorShape) 
	{
		actorShape->release();
	}

	//if(returnActor) 
	//{
	//	dynamic_cast<physx::PxRigidDynamic*>(returnActor)->setMass(aMass);
	//}
	return returnActor;
}

physx::PxRigidStatic* PhysicsEngine::CreateStaticActor(Shape aShape, const Vector3f& aScale, Model* aModel, const std::string& aMaterialName, bool aDebugMode, const Vector3f& aTransformSize, const Vector3f& aLocalRotation, bool aIsTrigger)
{
	physx::PxFilterData filterData;
	filterData.word0 = 0; // word0 = own ID
	filterData.word1 = 1;
	physx::PxMaterial* mat = nullptr;

	if (myPhysxMaterialMap.find(aMaterialName) == myPhysxMaterialMap.end())
	{
		mat = myPhysxMaterialMap.find("Default")->second;
	}
	else
	{
		mat = myPhysxMaterialMap.find(aMaterialName)->second;
	}

	const Vector3f& aSize = (aScale /*(aModel->GetPivot()) * aScale*/) * 0.5f;
	physx::PxShape* actorShape = nullptr;
	std::vector<physx::PxConvexMesh*> aMesh;
	std::vector<physx::PxTriangleMesh*> triangleMeshList;
	if (aShape == Shape::PxS_Box)
	{
		if (aIsTrigger)
		{
			actorShape = myPxPhysics->createShape(physx::PxBoxGeometry(aSize.x * aTransformSize.x, aSize.y * aTransformSize.y, aSize.z * aTransformSize.z), *mat, false, physx::PxShapeFlag::eTRIGGER_SHAPE);
		}
		else
		{
			actorShape = myPxPhysics->createShape(physx::PxBoxGeometry(aSize.x * aTransformSize.x, aSize.y * aTransformSize.y, aSize.z * aTransformSize.z), *mat);
		}
	}
	else if (aShape == Shape::PxS_Circle)
	{
		if (aIsTrigger)
		{
			actorShape = myPxPhysics->createShape(physx::PxSphereGeometry(std::max(std::max(aSize.x, aSize.y), aSize.z)), *mat, false, physx::PxShapeFlag::eTRIGGER_SHAPE);
		}
		else
		{
			actorShape = myPxPhysics->createShape(physx::PxSphereGeometry(std::max(std::max(aSize.x, aSize.y), aSize.z)), *mat);
		}
	}
	else if (aShape == Shape::PxS_Capsule)
	{
		actorShape = myPxPhysics->createShape(physx::PxCapsuleGeometry(aScale.x, aScale.y * 0.5f), *mat);
	}
	else if (aShape == Shape::PxS_Convex)
	{
		for (int i = 0; i < aModel->GetModelParts().size(); i++)
		{
			int aMax = std::ceil(aModel->GetModelPart(i)->myVertices.size() / 255);

			for (int j = 0; j <= aMax; j++)
			{
				int aIndex = 255 * (j + 1);
				if (aIndex > aModel->GetModelPart(i)->myVertices.size())
				{
					aIndex = aModel->GetModelPart(i)->myVertices.size();
				}
				std::vector<Vertex> aVertexVector = std::vector<Vertex>(aModel->GetModelPart(i)->myVertices.begin() + (255 * j), aModel->GetModelPart(i)->myVertices.begin() + aIndex);
				aMesh.push_back(CookConvexMesh(aVertexVector, aSize * 2.0f));

			}
		}
	}
	else if (aShape == Shape::PxS_Mesh)
	{
		for (int i = 0; i < aModel->GetModelParts().size(); i++)
		{
			triangleMeshList.push_back(CookTriangleMesh(*aModel->GetModelPart(i), aScale));
		}
	}
	physx::PxTransform globalTransfrom = { 0,0,0 };
	physx::PxTransform aLocalTransform = { 0,0,0 };
	Catbox::Quaternion tempQuat;

	tempQuat = Catbox::Quaternion::ToQuaternion(aLocalRotation);
	globalTransfrom.q.x = tempQuat.x;
	globalTransfrom.q.y = tempQuat.y;
	globalTransfrom.q.z = tempQuat.z;
	globalTransfrom.q.w = tempQuat.w;

	if (aModel && aShape != Shape::PxS_Mesh)
	{
		aLocalTransform.p = { (aModel->GetCenter().x) * aTransformSize.x,(aModel->GetCenter().y) * aTransformSize.y,(aModel->GetCenter().z) * aTransformSize.z };
	}


	physx::PxRigidStatic* returnActor = myPxPhysics->createRigidStatic(globalTransfrom);

	if (aShape == Shape::PxS_Convex)
	{
		for (int i = 0; i < aMesh.size(); i++)
		{
			actorShape = physx::PxRigidActorExt::createExclusiveShape(*returnActor, physx::PxConvexMeshGeometry(aMesh[i]), *mat);
			actorShape->setLocalPose(aLocalTransform);
			returnActor->attachShape(*actorShape);
			actorShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, aDebugMode);
		}
	}
	else if (aShape == Shape::PxS_Mesh)
	{
		for (int i = 0; i < triangleMeshList.size(); i++)
		{
			actorShape = physx::PxRigidActorExt::createExclusiveShape(*returnActor, physx::PxTriangleMeshGeometry(triangleMeshList[i]), *mat);

			returnActor->attachShape(*actorShape);
			actorShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, aDebugMode);
			triangleMeshList[i]->release();
		}
	}
	else
	{
		actorShape->setLocalPose(aLocalTransform);
		actorShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, aDebugMode);
		actorShape->setSimulationFilterData(filterData);
		actorShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		returnActor->attachShape(*actorShape);
	}

	if (!actorShape)
	{
		printerror("PxShape could not be created!");
	}

	if (actorShape)
	{
		actorShape->release();
	}

	return returnActor;
}

physx::PxRigidStatic* PhysicsEngine::CreateStaticNavmesh(MeshData aMeshData)
{
	physx::PxFilterData filterData;
	filterData.word0 = 0; // word0 = own ID
	filterData.word1 = 1;
	std::vector<physx::PxTriangleMesh*> triangleMeshList;
	physx::PxShape* actorShape = nullptr;

	triangleMeshList.push_back(CookTriangleMesh(aMeshData, {100.0f, 100.0f ,100.0f}));

	physx::PxTransform globalTransfrom = { 0,0,0 };
	physx::PxTransform aLocalTransform = { 0,0,0 };
	Catbox::Quaternion tempQuat;

	physx::PxRigidStatic* returnActor = myPxPhysics->createRigidStatic(globalTransfrom);

	for (int i = 0; i < triangleMeshList.size(); i++)
	{
		actorShape = physx::PxRigidActorExt::createExclusiveShape(*returnActor, physx::PxTriangleMeshGeometry(triangleMeshList[i]), *myPhysxMaterialMap["Default"]);

		returnActor->attachShape(*actorShape);
		actorShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
	}

	if (!actorShape)
	{
		printerror("PxShape could not be created!");
	}

	if (actorShape)
	{
		actorShape->release();
	}

	return returnActor;
}

physx::PxPBDParticleSystem* PhysicsEngine::CreateParticleSystem(ParticleType aType)
{
	physx::PxPBDParticleSystem* returnParticleSystem = NULL;
	returnParticleSystem = myPxPhysics->createPBDParticleSystem(*myCudaManager);
	const physx::PxU32 particlePhase = returnParticleSystem->createPhase(myDefaultParticleMaterial, physx::PxParticlePhaseFlags(physx::PxParticlePhaseFlag::eParticlePhaseFluid | physx::PxParticlePhaseFlag::eParticlePhaseSelfCollide));

	const physx::PxReal particleSpacing = 0.2f;
	const physx::PxReal fluidDensity = 1000.f;
	const physx::PxReal restOffset = 0.5f * particleSpacing / 0.6f;
	const physx::PxReal solidRestOffset = restOffset;
	const physx::PxReal fluidRestOffset = restOffset * 0.6f;
	const physx::PxReal renderRadius = fluidRestOffset;
	const physx::PxReal particleMass = fluidDensity * 1.333f * 3.14159f * renderRadius * renderRadius * renderRadius;
	returnParticleSystem->setRestOffset(restOffset);
	returnParticleSystem->setContactOffset(restOffset + 0.01f);
	returnParticleSystem->setParticleContactOffset(physx::PxMax(solidRestOffset + 0.01f, fluidRestOffset / 0.6f));
	returnParticleSystem->setSolidRestOffset(solidRestOffset);
	returnParticleSystem->setFluidRestOffset(fluidRestOffset);

	return returnParticleSystem;
}

physx::PxController* PhysicsEngine::CreateCharacterController(Shape aShape, const Vector3f& aSize, float aDensity, const std::string& aMaterialName, int aId)
{
	physx::PxBoxControllerDesc desc;
	desc.halfHeight = aSize.y * 0.5f;
	desc.halfForwardExtent = aSize.z * 0.5f;
	desc.halfSideExtent = aSize.x * 0.5f;
	desc.density = aDensity;
	desc.material = myPhysxMaterialMap[aMaterialName];
	desc.reportCallback = myCCTCallback;

	desc.userData = (void*)aId;

	physx::PxController* returnController = myCharacterController->createController(desc);

	returnController->getActor()->userData = (void*)aId;
	physx::PxShape* tempShape;
	returnController->getActor()->getShapes(&tempShape, 1);
	tempShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
	//tempShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);

	return returnController;
}

physx::PxMaterial* PhysicsEngine::CreateMaterial(const std::string& aName, const Vector3f& aMaterial)
{
	if (myPhysxMaterialMap.find(aName) == myPhysxMaterialMap.end())
	{
		physx::PxMaterial* createdMaterial = myPxPhysics->createMaterial(aMaterial.x, aMaterial.y, aMaterial.z);
		myPhysxMaterialMap.insert({ aName, createdMaterial });
		return createdMaterial;
	}
	return myPhysxMaterialMap.find(aName)->second;
}

physx::PxMaterial* PhysicsEngine::GetMaterial(const std::string& aName)
{
	if (myPhysxMaterialMap.find(aName) != myPhysxMaterialMap.end())
	{
		return myPhysxMaterialMap.find(aName)->second;
	}
	return nullptr;
}

physx::PxMaterial* PhysicsEngine::EditMaterial(const std::string& aName, const Vector3f& aMaterial)
{
	if (myPhysxMaterialMap.find(aName) != myPhysxMaterialMap.end())
	{
		myPhysxMaterialMap.find(aName)->second->release();
		physx::PxMaterial* createdMaterial = myPxPhysics->createMaterial(aMaterial.x, aMaterial.y, aMaterial.z);
		myPhysxMaterialMap.find(aName)->second = createdMaterial;
		return createdMaterial;
	}
	return CreateMaterial(aName, aMaterial);
}

void PhysicsEngine::CreateJoint(physx::PxRigidActor* actor0, const physx::PxTransform& localFrame0, physx::PxRigidActor* actor1, const physx::PxTransform& localFrame1)
{
	physx::PxFixedJoint* aJoint;
	aJoint = physx::PxFixedJointCreate(*myPxPhysics, actor0, localFrame0, actor1, localFrame1);
	aJoint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, true);
}

void PhysicsEngine::CreateAdditionalTriggerBox(Vector3f aSize, Vector3f anOffset, physx::PxRigidActor* anActor, const int aLayerID)
{
	physx::PxMaterial* mat = nullptr;
	mat = myPhysxMaterialMap.find("Default")->second;

	physx::PxShape* actorShape = nullptr;

	actorShape = myPxPhysics->createShape(physx::PxBoxGeometry(aSize.x * 0.5f, aSize.y * 0.5f, aSize.z * 0.5f), *mat, false, physx::PxShapeFlag::eTRIGGER_SHAPE);

	if (!actorShape)
	{
		printerror("PxShape could not be created!");
	}

	physx::PxFilterData filterData;

	filterData.word0 = 1 << aLayerID;
	filterData.word1 = 0;

	CollisionManager* aCollisionManager = Engine::GetInstance()->GetCollisionManager().get(); //Let it dangle then, idc

	for (size_t i = 0; i < aCollisionManager->GetPhysicsCollisionLayers()[aLayerID].size(); i++)
	{
		if (aCollisionManager->GetPhysicsCollisionLayers()[aLayerID][i])
		{
			filterData.word1 |= 1 << i;
		}
	}

	actorShape->setSimulationFilterData(filterData);
	actorShape->setQueryFilterData(filterData);
	actorShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
	actorShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);

	physx::PxTransform aLocalTransform = { anOffset.x, anOffset.y, anOffset.z };
	actorShape->setLocalPose(aLocalTransform);

	if (anActor)
	{
		anActor->attachShape(*actorShape);
	}

	/* I'm not going to sugarcoat it, You are 100% not using the Shape for your own use regardless
	return actorShape;*/
}

void PhysicsEngine::RemoveShapeFromActor(physx::PxRigidActor* anActor, const int aShapeID)
{
	physx::PxShape* actorShape = nullptr;

	const physx::PxU32 numShapes = anActor->getNbShapes();
	physx::PxShape** aShapeArray = (physx::PxShape**)malloc(sizeof(physx::PxShape*) * numShapes);
	anActor->getShapes(aShapeArray, numShapes);
	actorShape = aShapeArray[aShapeID];

	anActor->detachShape(*actorShape);
}

void PhysicsEngine::SetShapeLayer(physx::PxRigidActor* anActor, const int aShapeID, const int aLayerID)
{
	CollisionManager* aCollisionManager = Engine::GetInstance()->GetCollisionManager().get(); //Let it dangle then, idc
	physx::PxShape* actorShape = nullptr;
	physx::PxFilterData filterData;

	const physx::PxU32 numShapes = anActor->getNbShapes();
	physx::PxShape** aShapeArray = (physx::PxShape**)malloc(sizeof(physx::PxShape*) * numShapes);
	anActor->getShapes(aShapeArray, numShapes);
	actorShape = aShapeArray[aShapeID];

	filterData.word0 = 1 << aLayerID;
	filterData.word1 = 0;

	for (size_t i = 0; i < aCollisionManager->GetPhysicsCollisionLayers()[aLayerID].size(); i++)
	{
		if (aCollisionManager->GetPhysicsCollisionLayers()[aLayerID][i])
		{
			filterData.word1 |= 1 << i;
		}
	}

	actorShape->setSimulationFilterData(filterData);
	actorShape->setQueryFilterData(filterData);
}

Color PhysicsEngine::GetColorFromIndex(uint16_t aIndex)
{
	Color aColor = Color::White();
	if (aIndex & physx::PxDebugColor::eARGB_BLACK)
	{
		aColor = Color::Black();
	}
	else if (aIndex & physx::PxDebugColor::eARGB_BLUE)
	{
		aColor = Color::Blue();
	}
	else if (aIndex & physx::PxDebugColor::eARGB_CYAN)
	{
		aColor = Color::Cyan();
	}
	else if (aIndex & physx::PxDebugColor::eARGB_DARKBLUE)
	{
		aColor = Color(0, 0, 0.52f, 1);
	}
	else if (aIndex & physx::PxDebugColor::eARGB_DARKGREEN)
	{
		aColor = Color(0, 0.52f, 0, 1);
	}
	else if (aIndex & physx::PxDebugColor::eARGB_DARKRED)
	{
		aColor = Color(0.52f, 0, 0, 1);
	}
	else if (aIndex & physx::PxDebugColor::eARGB_GREEN)
	{
		aColor = Color::Green();
	}
	else if (aIndex & physx::PxDebugColor::eARGB_GREY)
	{
		aColor = Color(0.52f, 0.52f, 0.52f, 1);
	}
	else if (aIndex & physx::PxDebugColor::eARGB_MAGENTA)
	{
		aColor = Color::Magenta();
	}
	else if (aIndex & physx::PxDebugColor::eARGB_RED)
	{
		aColor = Color::Red();
	}
	else if (aIndex & physx::PxDebugColor::eARGB_YELLOW)
	{
		aColor = Color::Yellow();
	}

	return aColor;
}

void PhysicsEngine::InitScene()
{
	physx::PxSceneDesc sceneDesc(myPxToleranceScale);
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.simulationEventCallback = mySimCallback;
	sceneDesc.cpuDispatcher = myPxDispatcher;
	sceneDesc.filterShader = DefaultPxFilterFlags;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_CCD;
	//sceneDesc.cudaContextManager = myCudaManager;
	//sceneDesc.flags |= physx::PxSceneFlag::eENABLE_GPU_DYNAMICS | physx::PxSceneFlag::eENABLE_PCM;
	//sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eGPU;
	myPxScene = myPxPhysics->createScene(sceneDesc);
	myCharacterController = PxCreateControllerManager(*myPxScene);


	if (EDITORMODE) 
	{
		myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);

		auto& userPrefs = Editor::GetInstance()->GetUserPrefs();
		SetDebugSettings(userPrefs.myDebugSettings);
		//Joint Visualization
		//myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
		//myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
		//myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);

		physx::PxPvdSceneClient* pvdClient = myPxScene->getScenePvdClient();


		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
	}
	myResetingScene = false;
}

physx::PxConvexMesh* PhysicsEngine::CookConvexMesh(MeshData& aMeshData, const Vector3f& aSize)
{
	physx::PxConvexMeshDesc desc;
	physx::PxVec3 aData[255];
	for (int i = 0; i < aMeshData.myVertices.size(); i++)
	{
		physx::PxVec3 aVector3;
		aVector3.x = (aMeshData.myVertices[i].position.x * aSize.x) * 0.01f;
		aVector3.y = (aMeshData.myVertices[i].position.y * aSize.y) * 0.01f;
		aVector3.z = (aMeshData.myVertices[i].position.z * aSize.z) * 0.01f;
		aData[i] = aVector3;
	}

	desc.points.data = aData;
	desc.points.count = physx::PxU32(aMeshData.myVertices.size());
	desc.points.stride = sizeof(physx::PxVec3);
	desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	myCooker->cookConvexMesh(desc, buf, &result);
	switch (result)
	{
	case physx::PxConvexMeshCookingResult::eSUCCESS:
		print("Mesh cooked!");
		break;
	case physx::PxConvexMeshCookingResult::eZERO_AREA_TEST_FAILED:
		print("Mesh has no area! Failed!");
		break;
	case physx::PxConvexMeshCookingResult::ePOLYGONS_LIMIT_REACHED:
		break;
	case physx::PxConvexMeshCookingResult::eFAILURE:
		print("Mesh failed!");
		break;
	default:
		break;
	}

	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	physx::PxConvexMesh* convexMesh = myPxPhysics->createConvexMesh(input);
	return convexMesh;
}
physx::PxConvexMesh* PhysicsEngine::CookConvexMesh(std::vector<Vertex>& aMeshData, const Vector3f& aSize)
{
	physx::PxConvexMeshDesc desc;
	physx::PxVec3 aData[255];
	for (int i = 0; i < aMeshData.size(); i++)
	{
		physx::PxVec3 aVector3;
		aVector3.x = (aMeshData[i].position.x * aSize.x) * 0.01f;
		aVector3.y = (aMeshData[i].position.y * aSize.y) * 0.01f;
		aVector3.z = (aMeshData[i].position.z * aSize.z) * 0.01f;
		aData[i] = aVector3;
	}

	desc.points.data = aData;
	desc.points.count = physx::PxU32(aMeshData.size());
	desc.points.stride = sizeof(physx::PxVec3);
	desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	myCooker->cookConvexMesh(desc, buf, &result);
	switch (result)
	{
	case physx::PxConvexMeshCookingResult::eSUCCESS:
		print("Mesh cooked!");
		break;
	case physx::PxConvexMeshCookingResult::eZERO_AREA_TEST_FAILED:
		print("Mesh has no area! Failed!");
		break;
	case physx::PxConvexMeshCookingResult::ePOLYGONS_LIMIT_REACHED:
		break;
	case physx::PxConvexMeshCookingResult::eFAILURE:
		print("Mesh failed!");
		break;
	default:
		break;
	}

	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	physx::PxConvexMesh* convexMesh = myPxPhysics->createConvexMesh(input);
	return convexMesh;
}
physx::PxTriangleMesh* PhysicsEngine::CookTriangleMesh(const MeshData& aMeshData, const Vector3f& aSize)
{
	physx::PxTriangleMeshDesc desc;

	std::vector<Vertex> aData;
	for (int i = 0; i < aMeshData.myNumberOfVertices; i++)
	{
		Vertex aVector3 = aMeshData.myVertices[i];
		aVector3.position.x = (aVector3.position.x * aSize.x);
		aVector3.position.y = (aVector3.position.y * aSize.y);
		aVector3.position.z = (aVector3.position.z * aSize.z);
		aData.push_back(aVector3);
	}

	desc.points.count = aMeshData.myNumberOfVertices;
	desc.points.data = aData.data();
	desc.points.stride = sizeof(Vertex);

	desc.triangles.count = aMeshData.myNumberOfIndices / 3;
	desc.triangles.data = aMeshData.myIndices.data();
	desc.triangles.stride = sizeof(unsigned int) * 3;

#ifdef _DEBUG
	//bool res = PxValidateTriangleMesh(myPxToleranceScale, desc);
#endif // _DEBUG
	physx::PxDefaultMemoryOutputStream buf;
	physx::PxTriangleMeshCookingResult::Enum result;
	myCooker->cookTriangleMesh(desc, buf, &result);
	switch (result)
	{
	case physx::PxTriangleMeshCookingResult::eSUCCESS:
		print("Mesh cooked!");
		break;
	case physx::PxTriangleMeshCookingResult::eFAILURE:
		print("Mesh failed!");
		break;
	default:
		break;
	}

	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());

	physx::PxTriangleMesh* aTriangleMesh = myPxPhysics->createTriangleMesh(input);

	return aTriangleMesh;
}
void PhysicsEngine::SendCallbacks()
{
	for (int i = 0; i < myCallbacksParameters.size(); i++)
	{
		myCallbacks[i](myCallbacksParameters[i]);
	}
	myCallbacksParameters.clear();
	myCallbacks.clear();
}

void DefaultSimulationCallback::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
{
}

void DefaultSimulationCallback::onWake(physx::PxActor** actors, physx::PxU32 count)
{
	
}

void DefaultSimulationCallback::onSleep(physx::PxActor** actors, physx::PxU32 count)
{

}

void DefaultSimulationCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	for (physx::PxU32 i = 0; i < nbPairs; i++)
	{
		//Shape is removed but does not get deleted. Gotta Check for this
		if (pairs[i].flags & (physx::PxContactPairFlag::eREMOVED_SHAPE_0 | physx::PxContactPairFlag::eREMOVED_SHAPE_1))
		{
			continue;
		}
		else
		{
			GameObject* gameObject0 = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)pairHeader.actors[0]->userData);
			GameObject* gameObject1 = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)pairHeader.actors[1]->userData);
			if (gameObject0 && gameObject1)
			{
				Collider* tempCollider0 = nullptr;
				Collider* tempCollider1 = nullptr;
				if (gameObject1->GetComponent<BoxCollider>())
				{
					tempCollider1 = gameObject1->GetComponent<BoxCollider>();
				}
				else if (gameObject1->GetComponent<SphereCollider>())
				{
					tempCollider1 = gameObject1->GetComponent<SphereCollider>();
				}
				else if (gameObject1->GetComponent<ConvexCollider>())
				{
					tempCollider1 = gameObject1->GetComponent<ConvexCollider>();
				}
				else if (gameObject1->GetComponent<MeshCollider>())
				{
					tempCollider1 = gameObject1->GetComponent<MeshCollider>();
				}
				else if (gameObject1->GetComponent<CapsuleCollider>())
				{
					tempCollider1 = gameObject1->GetComponent<CapsuleCollider>();
				}

				if (gameObject0->GetComponent<BoxCollider>())
				{
					tempCollider0 = gameObject0->GetComponent<BoxCollider>();
				}
				else if (gameObject0->GetComponent<SphereCollider>())
				{
					tempCollider0 = gameObject0->GetComponent<SphereCollider>();
				}
				else if (gameObject0->GetComponent<ConvexCollider>())
				{
					tempCollider0 = gameObject0->GetComponent<ConvexCollider>();
				}
				else if (gameObject0->GetComponent<MeshCollider>())
				{
					tempCollider0 = gameObject0->GetComponent<MeshCollider>();
				}
				else if (gameObject0->GetComponent<CapsuleCollider>())
				{
					tempCollider0 = gameObject0->GetComponent<CapsuleCollider>();
				}

				if (tempCollider0 && tempCollider1) 
				{
					ContactReport collisionData0;
					ContactReport collisionData1;
					collisionData0.ContactPointsNum = pairs[i].contactCount;
					collisionData1.ContactPointsNum = pairs[i].contactCount;

					collisionData0.CollisionDirection = CD_None;
					collisionData1.CollisionDirection = CD_None;

					physx::PxContactPairPoint* contactsArray = (physx::PxContactPairPoint*)malloc(sizeof(physx::PxContactPairPoint) * pairs[i].contactCount);
					pairs[i].extractContacts(contactsArray, pairs[i].contactCount);
					for (int j = 0; j < pairs[i].contactCount; j++)
					{
						if (contactsArray) 
						{
							Vector3f point = { contactsArray[j].position.x,contactsArray[j].position.y, contactsArray[j].position.z };
							Vector3f normal = { contactsArray[j].normal.x,contactsArray[j].normal.y, contactsArray[j].normal.z };
							collisionData0.ContactPoints.push_back(point);
							collisionData1.ContactPoints.push_back(point);
							collisionData0.ContactNormals.push_back(normal);
							collisionData1.ContactNormals.push_back(normal);

							Vector3f gameObject0Pos = gameObject0->GetTransform()->worldPos();
						
							if (normal.y >= 0.1f)
							{
								collisionData0.CollisionDirection = static_cast<ContactDirection>(collisionData0.CollisionDirection | CD_Down);
								collisionData1.CollisionDirection = static_cast<ContactDirection>(collisionData1.CollisionDirection | CD_Up);
							}
							else if (point.y <= 0.1f)
							{
								collisionData0.CollisionDirection = static_cast<ContactDirection>(collisionData0.CollisionDirection | CD_Up);
								collisionData1.CollisionDirection = static_cast<ContactDirection>(collisionData1.CollisionDirection | CD_Down);
							}
							if (normal.x >= 0.1f)
							{
								collisionData0.CollisionDirection = static_cast<ContactDirection>(collisionData0.CollisionDirection | CD_Left);
								collisionData1.CollisionDirection = static_cast<ContactDirection>(collisionData1.CollisionDirection | CD_Right);
							}
							else if (normal.x <= -0.1f)
							{
								collisionData0.CollisionDirection = static_cast<ContactDirection>(collisionData0.CollisionDirection | CD_Right);
								collisionData1.CollisionDirection = static_cast<ContactDirection>(collisionData1.CollisionDirection | CD_Left);
							}
							if (normal.z >= 0.1f)
							{
								collisionData0.CollisionDirection = static_cast<ContactDirection>(collisionData0.CollisionDirection | CD_Back);
								collisionData1.CollisionDirection = static_cast<ContactDirection>(collisionData1.CollisionDirection | CD_Forward);
							}
							else if (normal.z <= -0.1f)
							{
								collisionData0.CollisionDirection = static_cast<ContactDirection>(collisionData0.CollisionDirection | CD_Forward);
								collisionData1.CollisionDirection = static_cast<ContactDirection>(collisionData1.CollisionDirection | CD_Back);
							}
						}
					}

					tempCollider1->SetContactReport(collisionData0);
					tempCollider0->SetContactReport(collisionData1);


					if (pairs[i].events & (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND))
					{
						gameObject1->OnCollisionEnter(tempCollider0);
						gameObject0->OnCollisionEnter(tempCollider1);
					}
					else if (pairs[i].events & (physx::PxPairFlag::eNOTIFY_TOUCH_LOST))
					{
						gameObject1->OnCollisionExit(tempCollider0);
						gameObject0->OnCollisionExit(tempCollider1);
					}
					else if (pairs[i].events & (physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)) 
					{
						gameObject1->OnCollisionStay(tempCollider0);
						gameObject0->OnCollisionStay(tempCollider1);
					}
					free(contactsArray);
				}
			}
		}
	}
}

void DefaultSimulationCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	for (int i = 0; i < count; i++)
	{ 
		if (pairs[i].flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER | physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER))
		{
			continue;
		}
		else
		{
			GameObject* triggerObject = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)pairs[i].triggerActor->userData);
			GameObject* otherObject = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)pairs[i].otherActor->userData);
			if (triggerObject && otherObject)
			{
				Collider* tempCollider = nullptr;
				if (otherObject->GetComponent<BoxCollider>())
				{
					tempCollider = otherObject->GetComponent<BoxCollider>();
				}
				else if (otherObject->GetComponent<SphereCollider>())
				{
					tempCollider = otherObject->GetComponent<SphereCollider>();
				}
				else if (otherObject->GetComponent<ConvexCollider>())
				{
					tempCollider = otherObject->GetComponent<ConvexCollider>();
				}
				else if (otherObject->GetComponent<MeshCollider>())
				{
					tempCollider = otherObject->GetComponent<MeshCollider>();
				}
				else if (otherObject->GetComponent<CapsuleCollider>())
				{
					tempCollider = otherObject->GetComponent<CapsuleCollider>();
				}

				if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) 
				{
					triggerObject->OnTriggerExit(tempCollider);
				}
				else if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) 
				{
					triggerObject->OnTriggerEnter(tempCollider);
				}
			}
		}
	}
}

void DefaultSimulationCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
{

}


//Character Controller Callback
void DefaultCharacterControllerCallback::onShapeHit(const physx::PxControllerShapeHit& hit)
{
	GameObject* controller = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)hit.controller->getUserData());
	if ((int)hit.actor->userData != -1)
	{
		Collider* tempCollider = nullptr;

		GameObject* shape = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)hit.actor->userData);
		if (shape)
		{
			if (shape->GetComponent<BoxCollider>())
			{
				tempCollider = shape->GetComponent<BoxCollider>();
			}
			else if (shape->GetComponent<SphereCollider>())
			{
				tempCollider = shape->GetComponent<SphereCollider>();
			}
			else if (shape->GetComponent<ConvexCollider>())
			{
				tempCollider = shape->GetComponent<ConvexCollider>();
			}
			else if (shape->GetComponent<MeshCollider>())
			{
				tempCollider = shape->GetComponent<MeshCollider>();
			}
			if (tempCollider)
			{
				tempCollider->GetGameObject().OnCollisionStay(controller->GetComponent<BoxCollider>());
				controller->OnCollisionStay(tempCollider);
			}
		}
	}
}

void DefaultCharacterControllerCallback::onControllerHit(const physx::PxControllersHit& hit)
{
	//print("Controller hit");
}

void DefaultCharacterControllerCallback::onObstacleHit(const physx::PxControllerObstacleHit& hit)
{
}

bool DefaultCCFCallback::filter(const physx::PxController& a, const physx::PxController& b)
{
	//Fuck them Rats
	if (Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)a.getUserData())->GetComponent<BoxCollider>() &&
		Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)b.getUserData())->GetComponent<BoxCollider>())
	{
		if (Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)a.getUserData())->GetComponent<BoxCollider>()->GetLayer() == 4 &&
			Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)b.getUserData())->GetComponent<BoxCollider>()->GetLayer() == 4)
		{
			return false;
		}
	}

	return true;
}