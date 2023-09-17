#include "stdafx.h"
#include "PhysXUtilities.h"
#include "GameObjects/GameObjectFactory.h"
//#include "PhysicsEngine.h"

HitReport PhysXUtilities::RayCast(const Vector3f& aOrigin, const Vector3f& aDirection, float aMaxDistance, float aAliveTime, const Color& aColor, int layersToCheck)
{
	physx::PxRaycastBuffer aBuffer;
	physx::PxQueryFilterData aFilterData = physx::PxQueryFilterData();
	aFilterData.data.word0 = layersToCheck;

	if (aAliveTime > 0 && EDITORMODE) 
	{
		DebugDrawer::DrawLine(aOrigin, aOrigin + aDirection * aMaxDistance, aColor, aAliveTime);
	}

	Engine::GetInstance()->GetPhysicsEngine()->GetScene()->raycast(physx::PxVec3(aOrigin.x, aOrigin.y, aOrigin.z), physx::PxVec3(aDirection.x, aDirection.y, aDirection.z).getNormalized(), physx::PxReal(aMaxDistance), aBuffer, physx::PxHitFlag::eDEFAULT, aFilterData);

	HitReport returnHitReport;
	returnHitReport.Hit = aBuffer.hasAnyHits();
	if(returnHitReport.Hit)
	{
		returnHitReport.GameObjectHit = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)(aBuffer.getAnyHit(0).actor->userData));
		returnHitReport.NormalHit = Vector3f(aBuffer.getAnyHit(0).normal.x, aBuffer.getAnyHit(0).normal.y, aBuffer.getAnyHit(0).normal.z);
		returnHitReport.PositionHit = Vector3f(aBuffer.getAnyHit(0).position.x, aBuffer.getAnyHit(0).position.y, aBuffer.getAnyHit(0).position.z);
	}
	return returnHitReport;
}

bool PhysXUtilities::SweepSingle(const Vector3f& aOrigin, const float aRadius, const Vector3f& aDirection, float aMaxDistance, int layersToCheck)
{
	physx::PxSweepHit aHitBuffer[256];
	physx::PxSweepBuffer aBuffer(aHitBuffer, 256);
	physx::PxSphereGeometry aSweepShape = physx::PxSphereGeometry(aRadius);
	physx::PxTransform aTransform(physx::PxVec3(aOrigin.x, aOrigin.y, aOrigin.z));
	std::vector<GameObject*> objectsHit;
	physx::PxQueryFilterData aFilterData = physx::PxQueryFilterData();
	aFilterData.data.word0 = layersToCheck;

	Engine::GetInstance()->GetPhysicsEngine()->GetScene()->sweep(aSweepShape, aTransform, physx::PxVec3(aDirection.x, aDirection.y, aDirection.z), aMaxDistance, aBuffer, physx::PxHitFlag::eDEFAULT, aFilterData);

	if (aBuffer.nbTouches > 0)
	{
		return true;
	}

	return false;
}

std::vector<GameObject*> PhysXUtilities::SweepMultiple(const Vector3f& aOrigin, const float aRadius, const Vector3f& aDirection, float aMaxDistance, int layersToCheck)
{
	physx::PxSweepHit aHitBuffer[256];
	physx::PxSweepBuffer aBuffer(aHitBuffer, 256);
	physx::PxSphereGeometry aSweepShape = physx::PxSphereGeometry(aRadius);
	physx::PxTransform aTransform(physx::PxVec3(aOrigin.x, aOrigin.y, aOrigin.z));
	std::vector<GameObject*> objectsHit;
	physx::PxQueryFilterData aFilterData = physx::PxQueryFilterData();
	aFilterData.data.word0 = layersToCheck;

	Engine::GetInstance()->GetPhysicsEngine()->GetScene()->sweep(aSweepShape, aTransform, physx::PxVec3(aDirection.x, aDirection.y, aDirection.z), aMaxDistance, aBuffer, physx::PxHitFlag::eDEFAULT, aFilterData);

	for (physx::PxU32 i = 0; i < aBuffer.nbTouches; i++)
	{
		objectsHit.push_back(Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)(aBuffer.touches[i].actor->userData)));
	}

	return std::vector<GameObject*>();
}

bool PhysXUtilities::OverlapAny(const Vector3f& aOrigin, const float aRadius, int layersToCheck)
{
	physx::PxOverlapHit aHitBuffer[256];
	physx::PxOverlapBuffer aBuffer(aHitBuffer, 256);
	physx::PxSphereGeometry anOverlapShape = physx::PxSphereGeometry(aRadius);
	physx::PxTransform aTransform(physx::PxVec3(aOrigin.x, aOrigin.y, aOrigin.z));
	std::vector<GameObject*> objectsHit;
	physx::PxQueryFilterData aFilterData = physx::PxQueryFilterData();
	aFilterData.data.word0 = layersToCheck;

	Engine::GetInstance()->GetPhysicsEngine()->GetScene()->overlap(anOverlapShape, aTransform, aBuffer, aFilterData);

	if (aBuffer.nbTouches > 0)
	{
		return true;
	}

	return false;
}

std::vector<GameObject*> PhysXUtilities::OverlapMultiple(const Vector3f& aOrigin, const float aRadius, int layersToCheck)
{
	physx::PxOverlapHit aHitBuffer[256];
	physx::PxOverlapBuffer aBuffer(aHitBuffer, 256);
	physx::PxSphereGeometry anOverlapShape = physx::PxSphereGeometry(aRadius);
	physx::PxTransform aTransform(physx::PxVec3(aOrigin.x, aOrigin.y, aOrigin.z));
	std::vector<GameObject*> objectsHit;
	physx::PxQueryFilterData aFilterData = physx::PxQueryFilterData();
	aFilterData.data.word0 = layersToCheck;
	
	Engine::GetInstance()->GetPhysicsEngine()->GetScene()->overlap(anOverlapShape, aTransform, aBuffer, aFilterData);

	for (physx::PxU32 i = 0; i < aBuffer.nbTouches; i++)
	{
		//Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)(aBuffer.touches[i].actor->userData))->SetActive(false);
		//Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)(aBuffer.touches[i].actor->userData))->Destroy(); //Teehee
		auto temp = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)(aBuffer.touches[i].actor->userData));
		if (temp) objectsHit.push_back(temp);
	}

	return objectsHit;
}

Vector3f& PhysXUtilities::quaternion2Euler(const physx::PxQuat& q)
{
	float r11 = 2 * (q.x * q.y + q.w * q.z);
	float r12 = q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z;
	float r21 = -2 * (q.x * q.z - q.w * q.y);
	float r31 = 2 * (q.y * q.z + q.w * q.x);
	float r32 = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;

	if (r21 > 1.0f) {
		r21 = 1.0f;
	}
	else if (r21 < -1.0f) {
		r21 = -1.0f;
	}
	float asin_arg = r21;
	float asin_result = Catbox::Rad2Deg(asin(asin_arg));
	Vector3f returnRotation;
	returnRotation.x = Catbox::Rad2Deg(atan2(r31, r32));
	returnRotation.y = asin_result;
	returnRotation.z = Catbox::Rad2Deg(atan2(r11, r12));

	return returnRotation;
}
