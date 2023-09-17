#pragma once
#include <string>
#include <vector>
#include "NetMessage.h"
#include <unordered_map>
#include "..\Catbox\CommonUtilities\Vector.h"
#include <iostream>

struct OnlineGameObject
{
	std::string name;
	int id;
	int parentId;
	Vector3f position;
	Vector3f rotation;
	std::vector<std::string> components;
	//std::vector<std::string> myCoponentData;
};

struct OnlineScene 
{
	std::string myName;
	std::string myPath;
	std::unordered_map<short, std::vector<Vector3f>> myUsers;
	std::vector<OnlineGameObject> myGameObjects;
	float myAutoSaveTimer = 2;
	bool myOffline = true;
	int myIdCounter = 0;
};

class OnlineSceneEditor
{
public:
	void AddSceme(short aId, const std::string& aSceneName);
	void Update(float aDelta);
	void ProcessMessage(NetMessage* aNetMessage);
	void SaveScene(OnlineScene aScene);
private:
	std::unordered_map<short, OnlineScene> myScenes;
	int mySceneIdCounter = 0;
};

