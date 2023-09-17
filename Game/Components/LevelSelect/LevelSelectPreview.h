#pragma once

class LevelSelectPreview : public Component
{
public:
	void Awake() override;
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	void SetValues(const bool& aBronze, const bool& aSilver, const bool& aGold, const int& bronzeLevel, const int& silverLevel, const int& goldLevel);
	void SetStarSprites(const bool& aBronze, const bool& aSilver, const bool& aGold);
	void SetCounterValues(const int& aBronze, const int& aSilver, const int& aGold);

private:
	UISprite* myBronzeStar = nullptr;
	UISprite* mySilverStar = nullptr;
	UISprite* myGoldStar = nullptr;
	Camera* myCamera = nullptr;
	RayPickerComponent* myRayPickerComponent = nullptr;

	Transform* myZerothChild = nullptr;
	Transform* myFirstChild = nullptr;
	Transform* mySecondChild = nullptr;
	Transform* myThirdChild = nullptr;
	Transform* myFourthChild = nullptr;
	Transform* myFifthChild = nullptr;
	Transform* mySixthChild = nullptr;

	Vector3f myParentPos;

	const Vector3f myGeneralOffset = { 0.4f, 0.0f, 0.0f };

	const Vector3f myStarOneOffset = { -0.1f, -0.1f, 0.1f };
	const Vector3f myStarTwoOffset = { 0.0f, -0.1f, 0.1f };
	const Vector3f myStarThreeOffset = { 0.1f, -0.1f, 0.1f };

	const Vector3f myScoreOneOffset = { -0.09f, -0.18f, 0.1f }; // This is painful to tweak, I know. 
	const Vector3f myScoreTwoOffset = { 0.012f, -0.18f, 0.1f };
	const Vector3f myScoreThreeOffset = { 0.112f, -0.18f, 0.1f };

	float myXScale = 1.0f;
	float myYScale = 1.0f;
	float myYOffset = 0.0f;

	int myBronzeLevel = 0;
	int mySilverLevel = 0;
	int myGoldLevel = 0;
	
	bool myHasBronze = false;
	bool myHasSilver = false;
	bool myHasGold = false;
};