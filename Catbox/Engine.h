#pragma once
#include "CommonUtilities/Timer.h" 
#include"GameObjects/GameObjectFactory.h"
#include "Assets\Prefab.h"
#include <unordered_map>

class Camera;
class Transform;
class SceneManager;
class GraphicsEngine;
class JsonParser;
class GameObject;
class CollisionManager;
class AudioManager;
class AssetRegistry;
class RapidJsonWrapper;
class ThreadPool;
class CameraController;
class PhysicsEngine;
class CrowdManager;
namespace CatNet 
{
	class NetworkManager;
}


class Engine
{
public:
	static Engine* GetInstance() { return myInstance; }
	Engine();
	~Engine();
	void Init(bool aIsEditorMode);

	std::shared_ptr<SceneManager> GetSceneManager() const { return mySceneManager; }
	std::shared_ptr<GameObjectFactory> GetGameObjectFactory() const { return myGameObjectFactory; }
	std::shared_ptr<GraphicsEngine> GetGraphicsEngine() const { return myGraphicsEngine; }
	std::shared_ptr<CollisionManager> GetCollisionManager() const { return myCollisionManager; }
	std::shared_ptr<AudioManager> GetAudioManager() const { return myAudioManager; }
	std::shared_ptr<ThreadPool> GetThreadPool() const { return myThreadPool; }
	std::shared_ptr<CameraController> GetCameraController() const { return myCameraController; }
	std::shared_ptr<CrowdManager> GetCrowdManager() const { return myCrowdManager; }
	std::shared_ptr<PhysicsEngine> GetPhysicsEngine() const { return myPhysicsEngine; }
	std::shared_ptr<CatNet::NetworkManager> GetNetworkManager();

	
	static void LerpTimeSpeed(float aTargetProcentage, float t);
	static GameObject* GetGameObject(int id);
	static void RemoveGameObject(int id);
	static void AddComponentFrameTimeData(const std::string& aKey, double aValue);
	static void AddGameObjectFrameTimeData(const std::string& aKey, double aValue);
	void SetMainCamera(Camera* aCamera);
	inline Camera* GetMainCamera() { return myMainCamera; }
	void SetActiveCamera(Camera* aCamera);
	inline Camera* GetActiveCamera() { return myActiveCamera; }
	inline bool IsEditorMode() { return myIsEditorMode; }
	bool IsGameRunning();
	void SetGamePaused(bool aIsPaused);
	bool IsGamePaused();
	bool IsSceneLoaded();
	void StartPlayMode();
	void StopPlayMode();
	void ShutDown();

	const std::unordered_map<std::string, std::pair<double, unsigned int>>& GetComponentFrameTimeData();
	const std::unordered_map<std::string, double>& GetGameObjectFrameTimeData();

	inline double GetEngineFrameTime() const { return myEngineFrameTime; }

	//This is for when we have a console window
	//static void Print(std::string aMsg);
	//static void PrintError(std::string aMsg);

	void SetWindowSize(const int& aWidth, const int& aHeight) { myWindowSize = { aWidth, aHeight }; }
	Vector2i GetWindowSize() { return myWindowSize; }
	Vector2i ViewportToScreenPos(int anX, int anY);
	Vector2i ViewportToScreenPos(Vector2i aScreenPos);
	Vector2i ScreenToViewportPos(Vector2i aScreenPos);
	bool IsViewportHovered();
	float GetWindowRatio() { return myWindowSize.x / (float)myWindowSize.y; }

	const float GetDeltaTime();
	const double GetTotalTime();
	const float GetRealDeltaTime();
	const double GetRealTotalTime();
	const void ChangeTimeSpeed(float aProcentage);
	const float GetTimeSpeed();
	void Update();

	Vector3f tempData = {1,0,1};
	float tempData2 = 4.0f;

	bool HasFinishedFrame() const { return myHasFinishedFrame; }

private:
	static Engine* myInstance;
	static std::unordered_map<std::string, std::pair<double, unsigned int>> myComponentFrameTimeData;
	static std::unordered_map<std::string, double> myGameObjectFrameTimeData;
	std::shared_ptr<SceneManager> mySceneManager;
	std::shared_ptr<GameObjectFactory> myGameObjectFactory;
	std::shared_ptr<GraphicsEngine> myGraphicsEngine;
	std::shared_ptr<CollisionManager> myCollisionManager;
	std::shared_ptr<AudioManager> myAudioManager;
	std::shared_ptr<AssetRegistry> myAssetRegistry;
	std::shared_ptr<RapidJsonWrapper> myRapidJsonWrapper;
	std::shared_ptr<ThreadPool> myThreadPool;
	std::shared_ptr<CameraController> myCameraController;
	std::shared_ptr<PhysicsEngine> myPhysicsEngine;
	std::shared_ptr<CatNet::NetworkManager> myNetworkManger;
	std::shared_ptr<CrowdManager> myCrowdManager;
	Camera* myMainCamera = nullptr;
	Camera* myActiveCamera = nullptr;
	Vector2i myWindowSize;
	bool myIsEditorMode = false;
	bool myIsGameRunning = false;
	double myEngineFrameTime;
	bool myHasFinishedFrame = false;

	float myStepSize = 1.0f / 60.0f;
	float myStepTimer = 0.0f;
};


static GameObject* Instantiate()
{
	return Engine::GetInstance()->GetGameObjectFactory()->CreateGameObject();
}

static GameObject* InstantiatePrefab(const std::string& aName)
{
	return Engine::GetInstance()->GetGameObjectFactory()->CreateGameObjectWithPrefab(aName);
}

static GameObject* InstantiatePrefab(std::shared_ptr<Prefab> aPrefab)
{
	return Engine::GetInstance()->GetGameObjectFactory()->CreateGameObjectWithPrefab(aPrefab->GetName());
}

static GameObject* Instantiate(Transform* aParent)
{
	return Engine::GetInstance()->GetGameObjectFactory()->CreateGameObject(aParent);
}