#include "stdafx.h"
#include "NavigationWindow.h"
#include "Navigation\NavMesh.h"
#include "Navigation/NavMeshExporter.h"
#include "Physics/PhysXUtilities.h"

void NavigationWindow::Render()
{
	if (!BeginWindow("Navigation", true)) { myCreatingOffMeshConnections = false; return; }

	bool edit;

	if (mySelectedNavMesh != nullptr) 
	{
		if (ImGui::Button("Bake Navmesh"))
		{
			std::vector<float> someFloats;
			for (int i = 0; i < myOffMeshConnections.size(); i++)
			{
				auto connection = myOffMeshConnections[i];
				someFloats.push_back(connection.positionA.x);
				someFloats.push_back(connection.positionA.y);
				someFloats.push_back(connection.positionA.z);
				someFloats.push_back(connection.positionB.x);
				someFloats.push_back(connection.positionB.y);
				someFloats.push_back(connection.positionB.z);
			}
			/*auto navMesh = NavMeshExporter::BakeNavMesh(mySettings, someFloats);
			if (navMesh) 
			{
				mySelectedNavMesh->Clear();
				mySelectedNavMesh->SetDtNavMesh(navMesh->GetDtNavMesh());
				mySelectedNavMesh->SetDtNavMeshQuery(navMesh->GetDtNavMeshQuery());
				mySelectedNavMesh->SetMesh(navMesh->myData);
				mySelectedNavMesh->SetBakingSettings(mySettings);
				mySelectedNavMesh->SetOffMeshConnections(myOffMeshConnections);
			}*/
		}
		if (ImGui::Button("Bake navmesh collider"))
		{
			if (mySelectedNavMesh) 
			{
				mySelectedNavMesh->UpdateNavMeshCollider();
			}
		}
		ImGui::Checkbox("Offmesh Connection Mode", &myCreatingOffMeshConnections);

		ImGui::Text("Rasterization");
		Catbox::DragFloat("Cell Size", &mySettings.cellSize, 0.005f, 0.0f, 1.0f, "%.2f");
		Catbox::DragFloat("Cell Height", &mySettings.cellHeight, 0.01f, 0.0f, 1.0f, "%.2f");

		ImGui::Text("Agent");
		Catbox::DragFloat("Height", &mySettings.agentHeight, 0.02f, 0.0f, 5.0f, "%.2f");
		Catbox::DragFloat("Radius", &mySettings.agentRadius, 0.02f, 0.0f, 5.0f, "%.2f");

		Catbox::DragFloat("Max Climb", &mySettings.agentMaxClimb, 0.02f, 0.0f, 5.0f, "%.2f");
		Catbox::DragFloat("Max Slope", &mySettings.agentMaxSlope, 0.5f, 0.0f, 90.0f, "%.0f");

		ImGui::Text("Region");
		Catbox::DragFloat("Min Region Size", &mySettings.minRegionSize, 1.0f, 0.0f, 150.0f, "%.0f");

		Catbox::DragFloat("Merged Region Size", &mySettings.mergeRegionSize, 1.0f, 0.0f, 150.0f, "%.0f");

		if (ImGui::BeginCombo("Partitioning", myPartitioningStyles[mySettings.partitioning].c_str()))
		{
			for (int i = 0; i < myPartitioningStyles.size(); i++)
			{
				bool selected = false;
				if (i == mySettings.partitioning)
				{
					selected = true;
				}
				if (ImGui::Selectable(myPartitioningStyles[i].c_str(), selected))
				{
					mySettings.partitioning = (Partitioning)i;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Text("Filters");
		ImGui::Checkbox("Low Hanging Obstacles", &mySettings.myFilters[0]);
		ImGui::Checkbox("Ledge Spans", &mySettings.myFilters[1]);
		ImGui::Checkbox("Walkable Low Height Spans", &mySettings.myFilters[2]);

		ImGui::Text("Polygonization");
		Catbox::DragFloat("Max Edge Length", &mySettings.maxEdgeLength, 0.3f, 0.0f, 50.0f, "%.0f");

		Catbox::DragFloat("Max Edge Error", &mySettings.maxEdgeError, 0.017f, 0.1f, 3.0f, "%.01f");

		ImGui::Text("Detail Mesh");
		Catbox::DragFloat("Sample Distance", &mySettings.sampleDistance, 0.08f, 0.0f, 16.0f, "%.0f");
		Catbox::DragFloat("Max Sample Error", &mySettings.maxSampleError, 0.08f, 0.0f, 16.0f, "%.0f");
	}

	if (Input::GetKeyPress(KeyCode::ESCAPE))
	{
		mySelectingOffMeshConnectionB = false;
	}

	ImGui::Separator();
	
	auto newNavMesh = AssetRegistry::GetInstance()->AssetDropdownMenu(mySelectedNavMesh.get(), "NavMesh to edit", edit);
	if (edit)
	{
		mySelectedNavMesh = newNavMesh;
		if (mySelectedNavMesh) 
		{
			mySavedMeshData = mySelectedMeshData = mySelectedNavMesh->GetData()->meshData;
			auto settings = newNavMesh->GetBakingSettings();
			myOffMeshConnections = newNavMesh->GetOffMeshConnections();
			mySettings = settings;
		}
	}

	if (mySelectedNavMesh)
	{
		auto newMesh = AssetRegistry::GetInstance()->AssetDropdownMenu(mySelectedMeshData.get(), "Mesh", edit);

		if (edit)
		{
			mySelectedMeshData = newMesh;
			mySelectedNavMesh->SetMesh(NavMesh::CreateNavMesh(mySelectedMeshData));
			
		}

		bool popColor = false;
		if (mySelectedMeshData != mySavedMeshData)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.3f, 1 });
			ImGui::Text("Mesh not saved!");
			popColor = true;
		}

		if (ImGui::Button("Save"))
		{
			mySelectedNavMesh->SaveAsset(mySelectedNavMesh->GetPathStr().c_str());
			mySavedMeshData = mySelectedMeshData;
		}


		if (popColor) ImGui::PopStyleColor();
	}


	if (ImGui::Button("Create New"))
	{
		auto path = Catbox::SaveFile("NavMesh(*.nav)\0*.nav\0");

		if (!path.empty())
		{
			mySelectedNavMesh = CreateAsset<NavMesh>(path);
		}
	}

	if (mySelectedNavMesh)
	{
		Engine::GetInstance()->GetGraphicsEngine()->AddToRenderQueue(mySelectedNavMesh.get());
	}

	if(mySelectingOffMeshConnectionA && mySelectingOffMeshConnectionB)
	{
		DebugDrawer::DrawSphere(myOffMeshConnectionAPos, 0.5f, Color::Blue(), 5);
		DebugDrawer::DrawSphere(myOffMeshConnectionBPos, 0.5f, Color::Red(), 5);

		std::array<Vector3f, 4> curve = { myOffMeshConnectionAPos, myOffMeshConnectionAPos + (Vector3f::up() * 5.0f) , myOffMeshConnectionBPos + (Vector3f::up() * 5.0f) , myOffMeshConnectionBPos};
		DebugDrawer::DrawCurve(curve);
	}
	else if (mySelectingOffMeshConnectionA) 
	{
		DebugDrawer::DrawSphere(myOffMeshConnectionAPos, 0.5f, Color::Blue(), 5);
	}

	for (int i = 0; i < myOffMeshConnections.size(); i++)
	{
		DebugDrawer::DrawSphere(myOffMeshConnections[i].positionA, 0.5f, Color::Blue(), 5);
		DebugDrawer::DrawSphere(myOffMeshConnections[i].positionB, 0.5f, Color::Red(), 5);

		std::array<Vector3f, 4> curve = { myOffMeshConnections[i].positionA, myOffMeshConnections[i].positionA + (Vector3f::up() * 5.0f) , myOffMeshConnections[i].positionB + (Vector3f::up() * 5.0f) , myOffMeshConnections[i].positionB };
		DebugDrawer::DrawCurve(curve, Color(1.0f,0.2f,1.0f, 1.0f), 5);
	}

	if (myCreatingOffMeshConnections && mySelectedNavMesh->myNavmeshCollider) 
	{
		if (Input::GetKeyPress(KeyCode::MOUSELEFT) && Engine::GetInstance()->IsViewportHovered())
		{
			if (mySelectingOffMeshConnectionA && mySelectingOffMeshConnectionB) 
			{
				mySelectingOffMeshConnectionA = false;
				mySelectingOffMeshConnectionB = false;
			}
			Vector3f aDir = Catbox::GetClickDirection(Input::GetMousePosition(), Editor::GetInstance()->GetEditorCamera().GetCamera());
			auto hitReport = PhysXUtilities::RayCast(Editor::GetInstance()->GetEditorCamera().GetCamera()->GetTransform()->worldPos(), aDir, 1000, 1.0f);

			if (Input::GetKeyHeld(KeyCode::CTRL)) 
			{
				for (size_t i = myOffMeshConnections.size() - 1; i >= 0 ; i--)
				{
					if (myOffMeshConnections[i].positionA.Distance(hitReport.PositionHit) < 5.0f || myOffMeshConnections[i].positionB.Distance(hitReport.PositionHit) < 5.0f)
					{
						myOffMeshConnections.erase(myOffMeshConnections.begin() + i);
					}
				}
			}
			else 
			{
				//raycast to world point;
				if (hitReport.Hit)
				{
					if (!mySelectingOffMeshConnectionA)
					{
						mySelectingOffMeshConnectionA = true;
						myOffMeshConnectionAPos = hitReport.PositionHit;
					}
					else
					{
						mySelectingOffMeshConnectionA = false;
						mySelectingOffMeshConnectionB = false;
						myOffMeshConnectionBPos = hitReport.PositionHit;
						OffMeshConnection offMeshConnection;
						offMeshConnection.positionA = myOffMeshConnectionAPos;
						offMeshConnection.positionB = myOffMeshConnectionBPos;
						myOffMeshConnections.push_back(offMeshConnection);
					}
				}
			}
		}
	}

	EndWindow();
}