#pragma once
#include <stdafx.h>
#include "Debugging\DebugDrawer.h"
enum ContactDirection
{
	CD_None = 0,
	CD_Right = (1 << 0),
	CD_Left = (1 << 1),
	CD_Up = (1 << 2),
	CD_Down = (1 << 3),
	CD_Forward = (1 << 4),
	CD_Back = (1 << 5)
};


struct ContactReport
{
	int ContactPointsNum = 0;
	std::vector<Vector3f> ContactPoints;
	std::vector<Vector3f> ContactNormals;
	ContactDirection CollisionDirection;
};

class RigidBody;
class Collider : public Component
{
public:
	enum class ColliderType
	{
		undefined,
		Box,
		Sphere,
		Circle2D,
		Box2D
	};

	Collider() = default;
	void Awake() override;
	~Collider();
	void RegisterCollision(Collider* anotherCollider);
	void UnregisterCollision(Collider* anotherCollider);
	inline std::vector<Collider*>& GetCollisions() { return myCurrentCollisions; }
	virtual void DebugDraw() = 0;
	inline const ColliderType& GetColliderType() const { return myColliderType; }
	inline const int GetCollisionLayer() const { return myLayer; }
	const std::string GetCollisionLayerName() const;
	inline void SetIsTrigger(bool aValue) { myIsTrigger = aValue; }
	inline bool GetIsTrigger() { return myIsTrigger; }
	void OnDisable() override;
	void SetLayer(int aLayer);
	void SetLayer(const std::string& aLayer);
	const int GetLayer() { return myLayer; };
	virtual bool IsInside(const Vector3f& aPoint) const = 0;
	bool GetDebugMode() const { return myDebugMode; }
	void SetContactReport(const ContactReport& aCollisionData) { myCollisionData = aCollisionData; }
	const ContactReport& GetContactReport() { return myCollisionData; }

protected:
	bool myHasLoaded = false;
	int myLayer = 0;
	RigidBody* myRb;
	std::vector<Collider*> myCurrentCollisions;
	ColliderType myColliderType;
	ContactReport myCollisionData;
	bool myIsTrigger = false;
	bool hasRunAwake = false;
	bool myDebugMode = true;
};