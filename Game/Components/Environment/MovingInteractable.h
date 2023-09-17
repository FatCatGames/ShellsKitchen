#pragma once

class MovingInteractable : public Component
{
public:
	virtual void FixedUpdate() override;
	virtual void Awake() override;
private:
	void setActiveVector();

	virtual void RenderInProperties(std::vector<Component*>& aComponentList);
	virtual void Save(rapidjson::Value& aComponentData);
	virtual void Load(rapidjson::Value& aComponentData);

	void JumpUp();
	void JumpDown();
	void Patrolling();

	bool myJumpUpFlag = false;
	bool myJumpDownFlag = false;
	bool myPatrollingFlag = false;
	float myMargin = 2.f;
	void Idlee();
	void CheckIfHeld();

	std::vector <std::vector<Vector3f>> myMovingVectors = {};
	std::vector<Vector3f> myActiveVector;
	std::vector<bool> myDebugVector;
	GameObject* myPrefab = nullptr;

	float mySpeed = 1;

	float myMovingDistance = 0;
	float myMovingPercent = 0;
	int myActiveIndex = 0;

	float myIdleeTimer = 0;
	bool myIsIdlee = false;
	float myIdleTime = 0; // myTimerTime is set to this value during idlee
	float myTimerTime = 0; //The value I use to check if the time is done
	int myRandomSecondsLOW = 5;
	int myRandomSecondsMAX = 10;
	bool myIsRespawning = false;

	bool myIdleeSetPos = false;
	Vector3f myIdleePos ={0,0,0};

	float myNintyDegreeNoice = 0;
	float myMaxNoice = 1.0f;
	float myNoiceSpeed = 0.05f;
	Vector3f myRotationAdd = {0,0,0};

	int myProcentChangeDirection = 10;
	bool myIsGoingRight = true;
	float myTurnTimer = 0.5f;
	float myTimeUntilTurning = 0;

	bool myWaitingForClick = false;
	bool myWaitingForClickDelay = false;
	int myWaitingForClickTargetVector = -1;
	int myWaitingForClickTargetID = -1;

	//Jumping

	std::vector<float> myJumpSpeed = {};  //
	std::vector<float> myJumpMovingSpeed = {};  //
	std::vector<float> myJumpingY = {};  //
	int myActiveVectorIndex = 0;

	float myJumpPercent = 0;
	
	bool start = true;
	float myStartingSpawnTimer = 0;
	float myStartingSpawnTime = 3;
};