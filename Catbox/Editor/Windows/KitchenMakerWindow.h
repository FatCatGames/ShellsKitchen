#pragma once
#include "Editor/Windows/EditorWindow.h"
class KitchenMakerWindow : public EditorWindow
{
public:
	struct KitchenData 
	{
		int currentLevel = -1;
		int bronzeScore = 0;
		int silverScore = 0;
		int goldScore = 0;
		int levelDuration = 0;
		std::vector<bool> recipes;
	};




	KitchenMakerWindow();
	~KitchenMakerWindow();

	bool GetShouldSave();
	bool GetRequestRecipes();
	KitchenData& GetLevelData();
	void PushRecipeNames(std::vector<std::string>& aNameList);
	void SetLevelData(KitchenData& someData);
	void Saved();
	void Update();
	
	Vector3f GetClickPosCentered();
private:

	enum class Direction
	{
		None,
		Horizontal,
		Vertical,
		Undetermined
	};

	enum class PlaceDirection
	{
		North,
		East,
		South,
		West,
		COUNT
	};


	void Render() override;

	void RenderAssets(bool aSplitScreen = false);
	void RenderSettings();


	GameObject* GetKitchen();
	GameObject* GetGroupObject(const std::string& aPrefabName);
	void LevelSettings();
	void ObjectSettings();
	void RemoveObject();
	void FindHoldableParent(GameObject* parent);
	void RemoveHoldableFromParent(GameObject* parent);
	void CreateObject();
	void CreateHoldable();
	void CreateTeleporter();
	Vector3f GetClickPos();
	void GenerateFloor(int x, int z);
	void HandleScroll();
	void RefreshAssetList();
	void FixCamera();
	void SetCameraSettings(Camera* aCamera);
	Camera* FindGameCamera();
	Vector3f SnapToGridCornerPivot(const Vector3f& aClickedPos);
	Vector3f SnapToGridCenteredPivotBottom(const Vector3f& aClickedPos);
	void ClearSelectedObjects();
	void Rotate2x1(GameObject* anOutGameObject,bool aClockwise);
	void AddRequiredPrefabsToScene();
	GameObject* GetPrefab(const std::string& aPrefab);
	int GetLightType();
	void SetLightType(int aLightType);
	void AddMovingInteractable();
	void RemoveMovingInteractable();
	GameObject* FindMovingInteractable();

	void Crabify();
	bool myIsEditing = false;
	bool myShowGizmo = false;
	int myLightType = 0;
	float myGridSize = 1;
	float myYscroll = 0;
	float myYscrollMultiplier = 0.3f;
	float myYHeight = 0.0f;
	float myRMBHeldTime = 0;
	Vector3f myStartingPaintPos = { 0,0,0 };
	std::shared_ptr<Prefab> mySelectedPrefab;
	std::vector<std::shared_ptr<Prefab>> myAssets;
	std::vector<std::string> myRecipeNames;
	Direction myPaintDirection = Direction::None;
	PlaceDirection myRotation = PlaceDirection::South;
	bool myShouldSave = false;
	bool myRequestingRecipes = false;
	KitchenData myKitchenData;
};

