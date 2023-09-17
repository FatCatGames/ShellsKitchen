#include "stdafx.h"
#include "Engine.h"
#include "ComponentTools/SceneManager.h"
#include "../GameObjects/GameObjectFactory.h"
#include <iostream>
#include "../CommonUtilities/InputHandler.h"
#include "CommonUtilities\RapidJsonWrapper.h"
#include "ComponentTools\UIEventHandler.h"
#include "Physics/Collisions/CollisionManager.h"
#include "Components/ComponentHandler.h"
#include "Editor\Windows\SceneWindow.h"
#include "Audio/AudioManager.h"
#include "ComponentTools\ThreadPool.h"
#include "ProjectSettings.h"
#include "../Game/Project.h"
#include "CameraController.h"
#include "Physics/PhysicsEngine.h"
#include "Networking/NetworkManager.h"
#include "TimeManager.h"
#include "CrowdManager.h"
#include "Physics/PhysXUtilities.h"

Engine* Engine::myInstance = nullptr;
std::unordered_map<std::string, std::pair<double, unsigned int>> Engine::myComponentFrameTimeData;
std::unordered_map < std::string, double> Engine::myGameObjectFrameTimeData;
Engine::Engine()
{
	if (!myInstance)
	{
		myInstance = this;
		myGraphicsEngine = std::make_shared<GraphicsEngine>();
		myAssetRegistry = std::make_shared<AssetRegistry>();
		myGameObjectFactory = std::make_shared <GameObjectFactory>();
		mySceneManager = std::make_shared<SceneManager>();
		myRapidJsonWrapper = std::make_shared<RapidJsonWrapper>();
		myCollisionManager = std::make_shared<CollisionManager>();
		myAudioManager = std::make_shared<AudioManager>();
		myThreadPool = std::make_shared<ThreadPool>();
		myThreadPool->Start();
		myCameraController = std::make_shared<CameraController>();
		myPhysicsEngine = std::make_shared<PhysicsEngine>();
		myCrowdManager = std::make_shared<CrowdManager>();
		ComponentHandler::ManuallyRegisterComponents();
		UIEventHandler::Init();
		ProjectSettings::LoadProjectSettings();
		ProjectSettings::LoadKeyBinds();
		
	}
}

Engine::~Engine()
{
	if (myNetworkManger) {
		myNetworkManger->Disconnect();
		myNetworkManger.reset();
	}
}

void Engine::Init(bool aIsEditorMode)
{
	myAssetRegistry->LoadAssets();
	myGameObjectFactory->Init(mySceneManager.get());
	myPhysicsEngine->Init();
	myIsEditorMode = aIsEditorMode;
}
std::shared_ptr<CatNet::NetworkManager> Engine::GetNetworkManager()
{
	if (!myNetworkManger)
	{
		myNetworkManger = std::make_shared<CatNet::NetworkManager>();
	}
	
	return myNetworkManger;
}
GameObject* Engine::GetGameObject(int id)
{
	return myInstance->myGameObjectFactory->GetObjectById(id);
}

void Engine::RemoveGameObject(int id)
{
	myInstance->myGameObjectFactory->RemoveObjectById(id);
}

void Engine::AddComponentFrameTimeData(const std::string& aKey, double aValue)
{
	if (myComponentFrameTimeData.find(aKey) == myComponentFrameTimeData.end()) 
	{
		myComponentFrameTimeData.insert({ aKey, {aValue, 1} });
	}
	else 
	{
		myComponentFrameTimeData[aKey].first += aValue;
		myComponentFrameTimeData[aKey].second++;
	}
}

void Engine::AddGameObjectFrameTimeData(const std::string& aKey, double aValue)
{
	myGameObjectFrameTimeData.insert({ aKey, aValue });
}

void Engine::SetMainCamera(Camera* aCamera)
{
	myMainCamera = aCamera;
	SetActiveCamera(aCamera);
}

void Engine::SetActiveCamera(Camera* aCamera)
{
	myActiveCamera = aCamera;
	myGraphicsEngine->SetCamera(myActiveCamera);
}

bool Engine::IsGameRunning()
{
	return myIsGameRunning && !Engine::GetInstance()->GetSceneManager()->GetIsLoading();
}

void Engine::SetGamePaused(bool aIsPaused)
{
	TimeManager::SetPaused(aIsPaused);
}

void Engine::LerpTimeSpeed(float aTargetProcentage, float t) 
{
	TimeManager::LerpTimeSpeed(aTargetProcentage, t);
}

bool Engine::IsGamePaused()
{
	return TimeManager::IsPaused();
}

bool Engine::IsSceneLoaded()
{
	if (mySceneManager->GetCurrentScene())
	{
		return !mySceneManager->GetIsLoading();
	}
	return false;
}

void Engine::StartPlayMode()
{
	//RapidJsonWrapper::GetInstance()->ClearDocument();
	myIsGameRunning = true;

	if (EDITORMODE)
	{
		mySceneManager->GetCurrentScene()->TempSave();
	}
	Project::Start();
	myCameraController->Clear();
	mySceneManager->StartGame();
	myGraphicsEngine->SetCamera(myActiveCamera);
}

void Engine::StopPlayMode()
{
	myGameObjectFactory->ClearAllObjects();
	myPhysicsEngine->ResetScene();
	myGraphicsEngine->EmptyVectors();
	myAudioManager->StopAllSound();
	myIsGameRunning = false;
	TimeManager::SetPaused(false);
	TimeManager::ResetTimeSpeed();
	myGraphicsEngine->SetCamera(Editor::GetInstance()->GetEditorCamera().GetCamera());
	myCameraController->Clear();
	mySceneManager->GetCurrentScene()->Unload();
	mySceneManager->GetCurrentScene()->TempLoad();
	Editor::GetInstance()->ClearAllSelectedObjects();
	Editor::GetInstance()->ResetHierarchy();
	myCollisionManager->ClearColliders();
	myCrowdManager->ClearCrowds();
	myMainCamera = nullptr;
	myActiveCamera = nullptr;
}

void Engine::ShutDown()
{
	myGameObjectFactory->ClearAllObjects();
}

const std::unordered_map<std::string, std::pair<double, unsigned int>>& Engine::GetComponentFrameTimeData()
{
	return myComponentFrameTimeData;
}

const std::unordered_map<std::string, double>& Engine::GetGameObjectFrameTimeData()
{
	return myGameObjectFrameTimeData;
}


void Engine::Update()
{
	if (Input::GetKeyPress(KeyCode::G)) 
	{
		myGraphicsEngine->SetAntiAliasingEnabled(!myGraphicsEngine->GetAntiAliasingEnabled());
	}
	myHasFinishedFrame = false;
	TimeManager::Update();
	if (deltaTime > 0.5f) { printerror("TOO LONG DELTA TIME! SKIPPING FRAME!"); return; }
	PerformanceManager::BeginTimer("Engine", "Whole Frame");
	Input::Update();
	myAudioManager->Update();
	mySceneManager->Update();
	if (myNetworkManger)
	{
		myNetworkManger->Update();
	}

	if (PLAYMODE)
	{
		myCrowdManager->Update();

		myStepTimer += deltaTime;


		while (myStepTimer > myStepSize)
		{
			myStepTimer -= myStepSize;

			if (EDITORMODE) 
			{
				if (myStepTimer > myStepSize)
				{
					myPhysicsEngine->ToggleDebugDrawing(false);
				}
				else
				{
					myPhysicsEngine->ToggleDebugDrawing(true);
				}
			}

			myPhysicsEngine->Update();
			mySceneManager->FixedUpdate();
		}

		myPhysicsEngine->DrawLines();

		UIEventHandler::Update();
		//myCollisionManager->UpdateCollisions();
		myCameraController->Update();
	}
	PerformanceManager::StopTimer("Engine");
	myHasFinishedFrame = true;
	//while(!myGraphicsEngine->HasFinishedFrame())
	//{
	//	continue;
	//}
	//myGraphicsEngine->FrameSwap();

}


Vector2i Engine::ViewportToScreenPos(int anX, int anY)
{
	float x = static_cast<float>(anX);
	float y = static_cast<float>(anY);
	float resX = static_cast<float>(DX11::GetResolution().x);
	float resY = static_cast<float>(DX11::GetResolution().y);

	if (x < 0) x = 0;
	if (y < 0) y = 0;

	if (EDITORMODE)
	{
		std::vector<SceneWindow*> sceneWindows = Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<SceneWindow>(WindowType::Scene);
		if (sceneWindows.size() == 0) return { static_cast<int>(std::roundf(x)), static_cast<int>(std::roundf(y)) };
		SceneWindow* viewPort = sceneWindows[0];

		//I want to die :)
		//mood
		Vector2f viewPortPos = viewPort->GetPosition();
		if (viewPortPos.x < 0)
		{
			viewPortPos.x += resX;
		}
		else if (viewPortPos.x > resX)
		{
			viewPortPos.x -= resX;
		}

		x -= viewPortPos.x;
		y -= viewPortPos.y;

		x *= resX / viewPort->GetSize().x;
		y *= resY / viewPort->GetSize().y;
		//hardcoded values to account for borders
		x -= resX * 0.005f;
		y -= resY * 0.046f;
	}

	return { static_cast<int>(std::roundf(x)), static_cast<int>(std::roundf(y)) };
}

Vector2i Engine::ViewportToScreenPos(Vector2i aScreenPos)
{
	return ViewportToScreenPos(aScreenPos.x, aScreenPos.y);
}

Vector2i Engine::ScreenToViewportPos(Vector2i aScreenPos)
{
	if (aScreenPos.x < 0) aScreenPos.x = 0;
	if (aScreenPos.y < 0) aScreenPos.y = 0;

	if (EDITORMODE)
	{
		const int width = DX11::GetResolution().x;
		const int height = DX11::GetResolution().y;

		std::vector<SceneWindow*> sceneWindows = Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<SceneWindow>(WindowType::Scene);
		if (sceneWindows.size() == 0) return { aScreenPos.x, aScreenPos.y };
		SceneWindow* viewPort = sceneWindows[0];

		Vector2f viewPortPos = viewPort->GetPosition();
		Vector2f viewPortSize = viewPort->GetSize();

		float posRelativeToScreenX = aScreenPos.x / static_cast<float>(width);
		float posRelativeToScreenY = aScreenPos.y / static_cast<float>(height);

		Vector2f viewportPosF;
		viewportPosF.x = viewPortPos.x;
		viewportPosF.y = viewPortPos.y;

		viewportPosF += Vector2f(posRelativeToScreenX * viewPortSize.x, posRelativeToScreenY * viewPortSize.y);
		return Vector2i(round(viewportPosF.x), round(viewportPosF.y));
	}

	return { aScreenPos.x, aScreenPos.y };
}


const float Engine::GetDeltaTime()
{
	return TimeManager::GetDeltaTime();
}

const double Engine::GetTotalTime()
{
	return TimeManager::GetTotalTime();
}


const float Engine::GetRealDeltaTime()
{
	return TimeManager::GetRealDeltaTime();
}

const float Engine::GetTimeSpeed()
{
	return TimeManager::GetTimeSpeed();
}

const double Engine::GetRealTotalTime()
{
	return TimeManager::GetRealTotalTime();
}

const void Engine::ChangeTimeSpeed(float aProcentage)
{
	TimeManager::ChangeTimeSpeed(aProcentage);
}

bool Engine::IsViewportHovered()
{
	if (!EDITORMODE) return true;
	auto viewport = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneWindow>(WindowType::Scene);
	if (viewport && !viewport->IsHovered())
	{
		return false;
	}
	return true;
}