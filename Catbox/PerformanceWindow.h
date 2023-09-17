#pragma once
#include "Editor/Windows/EditorWindow.h"


class PerformanceWindow : public EditorWindow
{
public:


private:
	void Render() override;
	void UpdateData();

	void RenderLeafs(PerformanceTimer aTimer, float maxTimer);

	float myTimer = 0;
	unsigned int myFPSCounter;
	unsigned int myFPS;


	std::unordered_map<std::string, std::pair<double, unsigned int>> myComponentsFrameTime;
	std::unordered_map<std::string, double> myGameObjectsFrameTime;

	PerformanceTimer currentTimer;

	std::vector<GameObject*> myRootObjects;
};