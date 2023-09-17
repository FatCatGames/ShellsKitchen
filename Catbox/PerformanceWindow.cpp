#include "stdafx.h"
#include "PerformanceWindow.h"
#include "ImGui/imgui_stdlib.h"
#include "Graphics/Rendering/Renderers/DeferredRenderer.h"
#include "Graphics/Rendering/Renderers/ForwardRenderer.h"
#include "Physics/PhysicsEngine.h"
#include "ComponentTools/SceneManager.h"
#include "CameraController.h"
#include "ComponentTools/UIEventHandler.h"
#include "..\Game\Project.h"

void PerformanceWindow::Render()
{
	if (!BeginWindow("Performance Statistics", true)) return;
	
	UpdateData();

	std::string temp = "FPS : " + std::to_string(myFPS);
	ImGui::Text(&temp[0]);

	temp = "Deferred Draw Calls : " + std::to_string(Engine::GetInstance()->GetGraphicsEngine()->GetDeferredRenderer().GetDrawCalls());
	ImGui::Text(&temp.c_str()[0]);
	temp = "Forward Draw Calls : " + std::to_string(Engine::GetInstance()->GetGraphicsEngine()->GetForwardRenderer().GetDrawCalls());
	ImGui::Text(&temp.c_str()[0]);
	
	RenderLeafs(currentTimer, currentTimer.myTimer);

	EndWindow();
}

void PerformanceWindow::UpdateData()
{
	myFPSCounter++;
	myTimer += realDeltaTime;
	if (myTimer >= 1.0f) 
	{
		currentTimer = PerformanceManager::GetWholeFrame();

		myTimer = 0;
		myFPS = myFPSCounter;
		myFPSCounter = 0;
	}
}

void PerformanceWindow::RenderLeafs(PerformanceTimer aTimer, float maxTimer)
{
	std::string timer = std::to_string(aTimer.myTimer * 1000.0f) + " ms ";
	bool open = false;
	if (aTimer.myChildren.size() > 0) 
	{
		open = ImGui::TreeNodeEx((aTimer.name + ": ").c_str(), ImGuiTreeNodeFlags_FramePadding);
	}
	else 
	{
		open = ImGui::TreeNodeEx((aTimer.name + ": ").c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf);
	}

	ImGui::SameLine();
	ImGui::Text(&timer[0]);
	float max = realDeltaTime;
	std::string procentage = std::to_string((aTimer.myTimer / maxTimer) * 100.0f) + " %%";
	ImGui::SameLine();
	ImGui::Text(&procentage[0]);

	if (open)
	{
		for (int i = 0; i < aTimer.myChildren.size(); i++)
		{
			RenderLeafs(aTimer.myChildren[i], aTimer.myTimer);
		}
		ImGui::TreePop();
	}
}
