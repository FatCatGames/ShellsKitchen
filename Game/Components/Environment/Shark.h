#pragma once

class Animator;
class Shark : public Component
{

public:
	void Awake() override;
	void Update() override;
	void OnCollisionEnter(Collider* aCollider);

private:
	void LiveSharkReaction(OrderResult* aResult);
	void OrderTimedOut();
	Animator* myAnimator;
	ListenerWithParameter myListener;
	Listener myThirtySecondsListener;
	ListenerWithParameter myOrderTimedOutListener;
	bool myShouldUpdateTriggers = false;
	LevelCounterComponent* myLevelCounterComponent = nullptr;
	bool myIsImpatiant = false;
	bool myIsClamsay = false;
};