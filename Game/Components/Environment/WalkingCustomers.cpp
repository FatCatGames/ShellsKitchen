#include "Game.pch.h"
#include "WalkingCustomers.h"
#include "WalkingCustomer.h"

void WalkingCustomers::Update()
{
	if (myPaths.empty()) return;

	myTimeUntilCustomerSpawn -= deltaTime;
	if (myTimeUntilCustomerSpawn <= 0)
	{
		myTimeUntilCustomerSpawn = Catbox::GetRandom(8, 15);
		auto customer = InstantiatePrefab("WalkingCustomer");
		int pathIndex = Catbox::GetRandom(0, static_cast<int>(myPaths.size() - 1));

		if (pathIndex == myPreviousPath)
		{
			++pathIndex;
			if (pathIndex > myPaths.size()) pathIndex = 0;
		}

		myPreviousPath = pathIndex;
		auto& path = myPaths[pathIndex];
		if (maybe)
		{
			std::reverse(path.wayPoints.begin(), path.wayPoints.end());
		}
		customer->GetComponent<WalkingCustomer>()->SetWaypoints(path.wayPoints);
	}
}

void WalkingCustomers::RenderInProperties(std::vector<Component*>& aComponentList)
{
	for (size_t i = 0; i < myPaths.size(); i++)
	{
		if (ImGui::CollapsingHeader(("Path " + std::to_string(i)).c_str()))
		{
			Color color = myPaths[i].color;
			float colorF[3] = { color.r, color.g, color.b };
			if (ImGui::ColorEdit3(("Color##CustomerWayPoint" + std::to_string(i)).c_str(), colorF))
			{
				myPaths[i].color.r = colorF[0];
				myPaths[i].color.g = colorF[1];
				myPaths[i].color.b = colorF[2];
			}

			auto& wayPoints = myPaths[i].wayPoints;
			for (size_t j = 0; j < wayPoints.size(); j++)
			{
				auto& point = wayPoints[j];
				DebugDrawer::DrawSphere(point, 0.5f, myPaths[i].color);

				if (j < wayPoints.size() - 1)
				{
					Vector3f direction = (wayPoints[j + 1] - point).GetNormalized();
					DebugDrawer::DrawLine(point + direction * 0.5f, wayPoints[j + 1] - direction * 0.f);


					float arrowheadLength = 0.5f;
					Vector3f arrowhead1 = wayPoints[j + 1] - (direction * arrowheadLength) - (direction.Cross(Vector3f::up()) * arrowheadLength);
					Vector3f arrowhead2 = wayPoints[j + 1] - (direction * arrowheadLength) + (direction.Cross(Vector3f::up()) * arrowheadLength);

					DebugDrawer::DrawLine(wayPoints[j + 1] - direction * 0.5f, arrowhead1 - direction * 0.5f);
					DebugDrawer::DrawLine(wayPoints[j + 1] - direction * 0.5f, arrowhead2 - direction * 0.5f);
				}

				if (ImGui::TreeNodeEx((std::to_string(j + 1) + "##CustomerWayPoint" + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::DragFloat(("X##CustomerWayPoint" + std::to_string(i) + ":" + std::to_string(j)).c_str(), &point.x, 0.1f);
					ImGui::DragFloat(("Y##CustomerWayPoint" + std::to_string(i) + ":" + std::to_string(j)).c_str(), &point.y, 0.1f);
					ImGui::DragFloat(("Z##CustomerWayPoint" + std::to_string(i) + ":" + std::to_string(j)).c_str(), &point.z, 0.1f);
					if (ImGui::Button(("Remove##CustomerWayPoint" + std::to_string(i) + ":" + std::to_string(j)).c_str()))
					{
						wayPoints.erase(wayPoints.begin() + j);
						j--;
					}
					if (ImGui::Button(("Insert##CustomerWayPoint" + std::to_string(i) + ":" + std::to_string(j)).c_str()))
					{
						Vector3f newWayPoint = wayPoints[j];
						wayPoints.insert(wayPoints.begin() + j, newWayPoint);
					}
					ImGui::TreePop();
				}
			}

			if (ImGui::Button(("New waypoint##" + std::to_string(i)).c_str()))
			{
				wayPoints.emplace_back();
				wayPoints.back().y = 0.f;
				if (wayPoints.size() > 1)
				{
					wayPoints.back() = wayPoints[wayPoints.size() - 2];
				}
			}

			if (ImGui::Button(("Remove path##" + std::to_string(i)).c_str()))
			{
				myPaths.erase(myPaths.begin() + i);
				i--;
			}
		}

	}

	if (ImGui::Button("New path"))
	{
		myPaths.emplace_back();
		myPaths.back().wayPoints.emplace_back();
		myPaths.back().wayPoints.back().y = 0.f;
		myPaths.back().wayPoints.emplace_back();
		myPaths.back().wayPoints.back().z = 2;
		myPaths.back().wayPoints.back().y = 0.f;
	}
}

void WalkingCustomers::Save(rapidjson::Value& aComponentData)
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper->GetDocument().GetAllocator();

	auto pathArray = wrapper->CreateArray();
	pathArray.SetArray();

	for (size_t i = 0; i < myPaths.size(); i++)
	{
		rapidjson::Value path;
		path.SetObject();
		auto wayPointsArray = wrapper->CreateArray();

		for (size_t j = 0; j < myPaths[i].wayPoints.size(); j++)
		{
			rapidjson::Value wayPoint;
			wayPoint.SetObject();
			wayPoint.AddMember("x", myPaths[i].wayPoints[j].x, alloc);
			wayPoint.AddMember("y", myPaths[i].wayPoints[j].y, alloc);
			wayPoint.AddMember("z", myPaths[i].wayPoints[j].z, alloc);
			wayPointsArray.PushBack(wayPoint, alloc);
		}

		path.AddMember("Waypoints", wayPointsArray, alloc);
		myPaths[i].color.ParseToJsonObject(path, wrapper->GetDocument(), "Color");
		pathArray.PushBack(path, alloc);
	}

	aComponentData.AddMember("Paths", pathArray, alloc);
	wrapper->SetTarget(aComponentData);
}

void WalkingCustomers::Load(rapidjson::Value& aComponentData)
{
	for (auto& path : aComponentData["Paths"].GetArray())
	{
		CustomerPath newPath;
		newPath.color.LoadFromJson(path["Color"]);
		for (auto& waypoint : path["Waypoints"].GetArray())
		{
			Vector3f newWaypoint;
			newWaypoint.x = waypoint["x"].GetFloat();
			newWaypoint.z = waypoint["z"].GetFloat();

			if (waypoint.HasMember("y"))
			{
				newWaypoint.y = waypoint["y"].GetFloat();
			}
			else
			{
				newWaypoint.y = 0.f;
			}

			newPath.wayPoints.push_back(newWaypoint);
		}
		myPaths.push_back(newPath);
	}
}
