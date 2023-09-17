#pragma once

struct PlayerBounce
{
	bool shouldBounce = false;
	Vector3f bounceDirection = { 0.0f, 0.0f, 0.0f };
	Vector3f bounceVelocity = { 0.0f, 0.0f, 0.0f };
	float bounceStrength = 10.0f;
	float bounceDuration = 0.3f;
	float bounceTimer = 0.0f;
};

struct PlayerInputState
{
	bool pickup = false;
	bool interact = false;
	bool interactHeld = false;
	bool interactReleased = false;
	bool dash = false;
	bool emote = false;
};

class Animator;
class Interactable;
class HoldableItem;
class RigidBody;
class Player : public Component
{
public:
	~Player();
	void OnTriggerEnter(Collider* aCollider) override;
	void OnTriggerExit(Collider* aCollider) override;
	void OnCollisionEnter(Collider* aCollider) override;
	void OnCollisionStay(Collider* aCollider) override;
	void Awake() override;
	void Update() override;
	void FixedUpdate() override;
	const int GetPlayerId() const { return myPlayerId; }

	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	void SetCanMove(bool aCanMove);
	void SetCanRotate(bool aCanRotate);
	void SetFreezeInput(bool aFreezeInput);
	void SetInputFrozenTimer(float aValue) { myInputFrozenTimer = aValue; }
	void SetAdditionalVelocity(Vector3f aVelocity);
	void SetPlayerID(int anID) { myPlayerId = anID; }

	void SetHeldItem(HoldableItem* anInteractable);
	inline HoldableItem* GetHeldItem() { return myHeldInteractable; }
	void DeliverItem();
	void SetAnimationBool(const std::string& aVariableName, bool aValue);
	void FinishAction();
	void RemoveInteractable(Interactable* anInteractable);

	bool GetIsDashing();
private:
	void UpdateInput(Vector3f& aDirection);
	void Move(Vector3f& aDirection);
	void Pickup();
	void Interact();
	void Dash();
	void Aim(bool anIsAiming);
	void Throw();
	void Emote();
	void Catch();
	void UpdateRespawn();
	void SetupHat();
	void SetupPlayer();
	void SlowRotate(RigidBody* aRigidBody, Vector3f& aDirection);

	CommonUtilities::XboxControllerInput* myController = nullptr;
	void DetermineClosestInteractable();

	int myPlayerId = -1;
	PlayerControls myControls;
	PlayerInputState myInputs;
	PlayerBounce myBounce;
	RigidBody* myRigidBody = nullptr;
	Vector3f mySpawnPoint = {0.0f, 0.0f, 0.0f};
	Vector3f myDirection = {0.0f, 0.0f, 0.0f};
	Vector3f myAdditionalVelocity = {0.0f, 0.0f, 0.0f};

	bool myIsAiming = false;
	bool myIsThrowing = false;
	bool myIsDashing = false;
	bool myCanMove = true;
	bool myFreezeAllInput = false;
	bool myIsPerformingAction = false;
	bool myCanThrow = true;
	bool myCanRotate = true;
	bool myHasSetupPlayer = false;
	bool myIsGrounded = true;
	bool myCollidingWithSide = false;

	float mySpeed = 800.0f;
	Vector3f myHeldInteractableOffset = { 0.0f, 0.6f, 0.6f };
	float myDashSpeed = 35.0f;
	float myDashTimer = 0.0f;
	float myDashDuration = 0.33f;
	float myDashCooldown = 0.5f;
	float myThrowingStrength = 10.0f;
	float myThrowingArc = 0.5f;
	float myRotationSpeed = 600.0f;
	float myControllerDeadlock = 0.6f;
	float myDrownTimer = 0.0f;
	float myGravityGraceTimer = 1.0f;
	float myInputFrozenTimer = 0.0f;

	std::vector<Interactable*> myInteractablesInRange;
	Interactable* mySelectedInteractable;
	HoldableItem* myHeldInteractable;
	Animator* myAnimator;
	ParticleSystem* myRunTrail;
	Transform* myHoldableItemBone;
	AudioMonoComponent* myAudio;
	ModelInstance* myHatModel;
	int myHoldableItemBoneId = -1;

	int myDeliverAnimationFrameCount = 0;
	float myDeliverTimer = 0.0f;
	float myThrottle = 0.f;
};