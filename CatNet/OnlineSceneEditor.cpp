#include "OnlineSceneEditor.h"

void OnlineSceneEditor::AddSceme(short aId, const std::string& aSceneName)
{
	if (myScenes.find(aId) == myScenes.end())
	{
		OnlineScene tempOnlineScene;
		tempOnlineScene.myName = aSceneName;
		myScenes.insert({aId, tempOnlineScene});
	}
	std::cout << "Scene id already exists!" << std::endl;
}

void OnlineSceneEditor::Update(float aDelta)
{
	for (auto& scene : myScenes)
	{
		scene.second.myAutoSaveTimer += aDelta;
		if (scene.second.myAutoSaveTimer >= 5) 
		{
			SaveScene(scene.second);
			scene.second.myAutoSaveTimer = 0;
		}
	}
}

void OnlineSceneEditor::ProcessMessage(NetMessage* aNetMessage)
{

}

void OnlineSceneEditor::SaveScene(OnlineScene aScene)
{

}
