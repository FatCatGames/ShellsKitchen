#include "stdafx.h"
#include "SceneManager.h"
#include "../GameObjects/GameObjectFactory.h"
#include "../Components/Physics/Collisions/CollisionManager.h"
#include "Editor/Windows/SceneHierarchyWindow.h"
#include "ComponentTools\ThreadPool.h"
#include "Physics/PhysicsEngine.h"
#include "CameraController.h"
#include "CrowdManager.h"


SceneManager::~SceneManager()
{
	//myCurrentScene->Unload();
	myScenes.clear();
}


void SceneManager::LoadScene(std::shared_ptr<Scene> aScene)
{
	Engine::GetInstance()->GetGameObjectFactory()->ClearAllObjects();
	Engine::GetInstance()->GetCameraController()->Clear();
	myIsLoadingScene = true;

	Engine::GetInstance()->GetGraphicsEngine()->EmptyVectors();
	Engine::GetInstance()->GetCollisionManager()->ClearColliders();
	Engine::GetInstance()->GetPhysicsEngine()->ResetScene();
	if (PLAYMODE)
	{
		Engine::GetInstance()->GetGraphicsEngine()->SetCamera(nullptr);
		Engine::GetInstance()->SetMainCamera(nullptr);
	}
	myIsLoading = true;

	if (myCurrentScene != nullptr)
	{
		Engine::GetInstance()->GetGameObjectFactory()->SetSceneToInstantiateTo(aScene.get());
		Engine::GetInstance()->GetCrowdManager()->ClearCrowds();
		myCurrentScene->Unload();
		myCurrentScene = aScene;
		myCurrentScene->LoadScene();
	}
	else
	{
		myCurrentScene = aScene;
		Engine::GetInstance()->GetGameObjectFactory()->SetSceneToInstantiateTo(aScene.get());
		myInstantiatingIndex = myIndexToLoad;
		myCurrentScene->LoadScene();
	}

	if (EDITORMODE)
	{
		Editor::GetInstance()->ClearAllSelectedObjects();

		for (auto& hierarchy : Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy))
		{
			hierarchy->SetScene(myCurrentScene.get());
		}
		Editor::GetInstance()->OpenScene(myCurrentScene);
		Engine::GetInstance()->GetCrowdManager()->ClearCrowds();

		auto parser = RapidJsonWrapper::GetInstance();
		parser->CreateDocument();
		string scenePath = myCurrentScene->GetPath().string();
		parser->SaveString("Last Scene", scenePath.c_str());
		if (!std::filesystem::exists("Temp"))
		{
			CreateDirectoryA("Temp", 0);
		}
		parser->SaveDocument("Temp/DefaultScene.json");
	}

	Engine::GetInstance()->GetGraphicsEngine()->EmptyVectors();
	myIsLoading = false;

	if (!EDITORMODE || PLAYMODE)
	{
		Engine::GetInstance()->StartPlayMode();
	}
	myIsLoadingScene = false;
	changeTimeSpeed(1.0f);
	myOnSceneLoadedEvent.Invoke();
}

bool SceneManager::GetIsLoading()
{
	return myIsLoading;
}

Scene* SceneManager::GetCurrentScene()
{
	return myCurrentScene.get();
}

int SceneManager::GetActiveSceneIndex()
{
	return myInstantiatingIndex;
}

void SceneManager::Update()
{
	if (!myCurrentScene || myIsLoading) return;

	PerformanceManager::BeginTimer("Update", "Engine");
	myCurrentScene->Update();
	PerformanceManager::StopTimer("Update");
}


void SceneManager::FixedUpdate()
{
	PerformanceManager::BeginTimer("Fixed Update", "Engine");
	if (!myCurrentScene || myIsLoading) return;

	myCurrentScene->FixedUpdate();

	PerformanceManager::StopTimer("Fixed Update");
}

void SceneManager::SetPaused(bool aPaused)
{
	myPaused = aPaused;
}

void SceneManager::StartGame()
{
	myCurrentScene->StartGameObjects();
	//myOnSceneLoadedEvent.Invoke();
}

bool SceneManager::GameIsPaused() const
{
	return myPaused;
}

void SceneManager::AddOnSceneLoadedListener(Listener& aListener)
{
	myOnSceneLoadedEvent.AddListener(aListener);
}

void SceneManager::ThreadLoadScene()
{
	while (true)
	{
		if (mySceneToLoad != nullptr)
		{
			myIsLoadingScene = true;
			Engine::GetInstance()->GetGraphicsEngine()->EmptyVectors();
			Engine::GetInstance()->GetCollisionManager()->ClearColliders();


			if (PLAYMODE)
			{
				Engine::GetInstance()->GetGraphicsEngine()->SetCamera(nullptr);
				Engine::GetInstance()->SetMainCamera(nullptr);
			}
			myIsLoading = true;

			if (myCurrentScene != nullptr)
			{
				Engine::GetInstance()->GetGameObjectFactory()->SetSceneToInstantiateTo(mySceneToLoad.get());

				Engine::GetInstance()->GetThreadPool()->ClearJobs();
				myCurrentScene->Unload();
				myCurrentScene = mySceneToLoad;
				myCurrentScene->LoadScene();

				// Testing to load in levels differently, should not make any difference.
				/*std::swap(myCurrentScene, mySceneToLoad);
				myCurrentScene->LoadScene();
				mySceneToLoad->Unload();*/
			}
			else
			{
				myCurrentScene = mySceneToLoad;
				Engine::GetInstance()->GetGameObjectFactory()->SetSceneToInstantiateTo(mySceneToLoad.get());
				myInstantiatingIndex = myIndexToLoad;
				myCurrentScene->LoadScene();
			}

			if (EDITORMODE)
			{
				Editor::GetInstance()->ClearAllSelectedObjects();

				for (auto& hierarchy : Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy))
				{
					hierarchy->SetScene(myCurrentScene.get());
				}
				Editor::GetInstance()->OpenScene(myCurrentScene);

				auto parser = RapidJsonWrapper::GetInstance();
				parser->CreateDocument();
				string scenePath = myCurrentScene->GetPath().string();
				parser->SaveString("Last Scene", scenePath.c_str());
				if (!std::filesystem::exists("Temp"))
				{
					CreateDirectoryA("Temp", 0);
				}
				parser->SaveDocument("Temp/DefaultScene.json");
			}

			Engine::GetInstance()->GetGraphicsEngine()->EmptyVectors();
			myIsLoading = false;

			if (!EDITORMODE || PLAYMODE)
			{
				Engine::GetInstance()->StartPlayMode();
			}

			mySceneToLoad = nullptr;
			myIsLoadingScene = false;
		}
	}
	myThreadActive = false;
}
