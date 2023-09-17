#pragma once
#include "Graphics/Animations/SkeletonData.h"

enum class AnimationEventType
{
	Prefab,
	PlaySound
};

struct PrefabData
{
	std::shared_ptr<Prefab> prefab;
	bool worldSpace = false;
	bool useBoneRotation = false;
	Vector3f position;
	Vector3f rotation;
	Skeleton::Bone* bone = nullptr;
};

struct SoundData
{
	int sfxIndex = 0;
};

class AnimationController;
class AnimationEvent
{
public:
	PrefabData myPrefabData;
	SoundData mySoundData;
	unsigned int myFrame = 0;
	AnimationEventType myType = AnimationEventType::Prefab;

	AnimationEvent() = default;
	~AnimationEvent() = default;
	inline void SetIndex(size_t anIndex) { myID = anIndex; }
	void Execute(Animator* anActor);
	void RenderInProperties(AnimationController* aController);

private:
	size_t myID;
	std::string mySoundEffectName = "";
};

