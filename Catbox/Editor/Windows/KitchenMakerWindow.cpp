#include "stdafx.h"
#include "KitchenMakerWindow.h"
#include "Editor/Gizmos.h"
#include "Editor/Windows/SceneWindow.h"
#include "Editor/Windows/SceneHierarchyWindow.h"
//#include "Editor/Windows/PropertiesWindow.h"
#include "GameObjects\GameObjectSelection.h"
//#include "../Game/Managers/LevelSelectManager.h"
#include "ComponentTools/SceneManager.h"
#include "PopupManager.h"
#include "CameraController.h"

KitchenMakerWindow::KitchenMakerWindow()
{
	RefreshAssetList();

	mySelectedPrefab = AssetRegistry::GetInstance()->GetAsset<Prefab>("Trashcan");
}

KitchenMakerWindow::~KitchenMakerWindow()
{
	//if (mySelectedPrefab) 
	//{
	//	mySelectedPrefab.reset();
	//}
}

bool KitchenMakerWindow::GetShouldSave()
{
	return myShouldSave;
}

bool KitchenMakerWindow::GetRequestRecipes()
{
	return myRequestingRecipes;
}

KitchenMakerWindow::KitchenData& KitchenMakerWindow::GetLevelData()
{
	return myKitchenData;
}

void KitchenMakerWindow::PushRecipeNames(std::vector<std::string>& aNameList)
{
	myRecipeNames = aNameList;
	myKitchenData.recipes.resize(myRecipeNames.size());
	myRequestingRecipes = false;
}

void KitchenMakerWindow::SetLevelData(KitchenData& someData)
{
	myKitchenData = someData;
}

void KitchenMakerWindow::Saved()
{
	myShouldSave = false;
}

void KitchenMakerWindow::Update()
{
	if (myAssets.empty())
	{
		RefreshAssetList();
	}

	if (myIsEditing)
	{
		auto viewport = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneWindow>(WindowType::Scene);
		if (viewport && viewport->IsHovered())
		{
			if (!myShowGizmo)
			{
				if (Input::GetKeyHeld(KeyCode::MOUSELEFT))
				{
					if (mySelectedPrefab->GetName() == "Crab")
					{
						Crabify();
					}
					else if (mySelectedPrefab->GetName() == "MovingInteractable")
					{
						AddMovingInteractable();
					}
					else if (mySelectedPrefab->GetName() == "Teleporter")
					{
						CreateTeleporter();
					}
					else if (mySelectedPrefab->GetPathStr().find("Holdables") == std::string::npos)
					{
						CreateObject();
					}
					else
					{
						CreateHoldable();
					}
				}

				if (Input::GetKeyPress(KeyCode::MOUSERIGHT))
				{
					RemoveObject();
				}

				if (Input::GetKeyHeld(KeyCode::MOUSERIGHT))
				{
					myRMBHeldTime += deltaTime;
					if (myRMBHeldTime > 0.5f)
					{
						RemoveObject();
						if (mySelectedPrefab->GetName() == "MovingInteractable") RemoveMovingInteractable();
					}
				}
			}


			myYscroll += Input::GetMouseScroll() * myYscrollMultiplier;
			HandleScroll();

		}

		if (Input::GetKeyPress(KeyCode::A))
		{
			myShowGizmo = !myShowGizmo;
			if (!myShowGizmo)
			{
				Editor::GetInstance()->SetPrefabBrushMode(true);
				Editor::GetInstance()->GetGizmos().SetGizmoActive(false);
				ClearSelectedObjects();
			}
			else
			{
				Editor::GetInstance()->SetPrefabBrushMode(false);
				Editor::GetInstance()->GetGizmos().SetGizmoActive(true);
			}
		}

		if (Input::GetKeyReleased(KeyCode::SHIFT) || Input::GetKeyReleased(KeyCode::MOUSELEFT) || Input::GetKeyReleased(KeyCode::MOUSERIGHT))
		{
			myPaintDirection = Direction::None;
			myRMBHeldTime = 0;
		}
	}
}

Vector3f KitchenMakerWindow::GetClickPosCentered()
{
	Catbox::Vector3<float> ClickPosNear = GetClickPos();
	ClickPosNear = SnapToGridCenteredPivotBottom(ClickPosNear);

	return ClickPosNear;
}

void KitchenMakerWindow::Render()
{
	if (!BeginWindow("Kitchen Editor Window", &myShouldOpen)) return;

	if (ImGui::GetContentRegionAvail().x > 600)
	{
		ImGui::BeginChild("ChildL##KitchenWindow", ImVec2(ImGui::GetContentRegionAvail().x - 500, 0), false);
		RenderAssets();
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("ChildR##KitchenWindow", ImVec2(500, 0), false);
		RenderSettings();
		ImGui::EndChild();
	}
	else
	{
		RenderAssets();

		RenderSettings();
	}


	EndWindow();
}

void KitchenMakerWindow::RenderAssets(bool aSplitScreen)
{
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.7f,0.7f,0.7f,1 });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.7f,0.7f,0.7f,1 });
	float iconSize = 90.0f;
	float padding = 10.0f;
	float expectedTextPadding = 30.0f;
	float screenWidth = ImGui::GetContentRegionAvail().x;
	if (aSplitScreen)
	{
		screenWidth = ImGui::GetContentRegionAvail().x - 500;
	}

	int columnCount = static_cast<int>(std::floor(screenWidth / (iconSize + (padding * 1.5f))));
	bool itemHovered = false;
	bool windowHovered = false;
	if (columnCount > 10)
	{
		columnCount = 10;
	}
	if (columnCount == 0) columnCount = 1;
	float aff = ((float)std::ceil(myAssets.size() / static_cast<float>(columnCount)));
	ImVec2 kitchenAssetSize = { (float)columnCount * (iconSize + padding * 1.5f), ((float)std::ceil(myAssets.size() / static_cast<float>(columnCount)) * (iconSize + padding + expectedTextPadding)) };
	if (ImGui::BeginTable("Kitchen Assets", columnCount, ImGuiTableFlags_ScrollY, kitchenAssetSize))
	{
		for (int i = 0; i < myAssets.size(); i++)
		{
			ImGui::TableNextColumn();
			std::string name = myAssets[i]->GetName() + "##kitchenWindow";
			//std::shared_ptr<Texture> texture = AssetRegistry::GetInstance()->GetAsset<Texture>("aff");
			std::shared_ptr<Texture> texture = AssetRegistry::GetInstance()->GetAsset<Texture>(myAssets[i]->GetName() + "_kitchen");
			if (myAssets[i] == mySelectedPrefab)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, { 1,1,1,1 });
				if (texture)
				{
					ImGui::ImageButton(name.c_str(), static_cast<ImTextureID>(texture->GetShaderResourceView().Get()), { iconSize, iconSize });
				}
				else
				{
					ImGui::ImageButton(name.c_str(), static_cast<ImTextureID>(AssetRegistry::GetInstance()->GetAsset<Texture>("MissingTexture_kitchen")->GetShaderResourceView().Get()), { iconSize, iconSize });
				}
				ImGui::PopStyleColor();
			}
			else
			{
				if (texture)
				{
					ImGui::ImageButton(name.c_str(), static_cast<ImTextureID>(texture->GetShaderResourceView().Get()), { iconSize, iconSize });
				}
				else
				{
					ImGui::ImageButton(name.c_str(), static_cast<ImTextureID>(AssetRegistry::GetInstance()->GetAsset<Texture>("MissingTexture_kitchen")->GetShaderResourceView().Get()), { iconSize, iconSize });
				}
			}

			if (ImGui::IsItemHovered())
			{
				itemHovered = true;
				if (ImGui::IsMouseClicked(0))
				{
					mySelectedPrefab = myAssets[i];
				}
			}
			ImGui::TextWrapped(myAssets[i]->GetName().c_str());
		}

		if (ImGui::IsWindowHovered())
		{
			windowHovered = true;
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(2);


	if (ImGui::IsWindowHovered())
	{
		windowHovered = true;
	}
	if (!itemHovered && windowHovered && ImGui::IsMouseClicked(1))
	{
		ImGui::OpenPopup("kitchen_menu");
	}
	if (ImGui::BeginPopup("kitchen_menu"))
	{
		if (ImGui::MenuItem("Refresh"))
		{
			RefreshAssetList();
		}
		ImGui::EndPopup();
	}
	//ImGui::End();
}

void KitchenMakerWindow::RenderSettings()
{
	if (!myIsEditing)
	{
		if (ImGui::Button("Start Editing"))
		{
			myIsEditing = true;
			myRequestingRecipes = true;
			Editor::GetInstance()->GetEditorCamera().LockTransform(true);
			Engine::GetInstance()->GetGraphicsEngine()->SetCamera(FindGameCamera());
			if (!myShowGizmo)
			{
				Editor::GetInstance()->SetPrefabBrushMode(true);
				Editor::GetInstance()->GetGizmos().SetGizmoActive(false);
				ClearSelectedObjects();
			}
			else
			{
				Editor::GetInstance()->SetPrefabBrushMode(false);
				Editor::GetInstance()->GetGizmos().SetGizmoActive(true);
			}
			if (!mySelectedPrefab)
			{
				mySelectedPrefab = AssetRegistry::GetInstance()->GetAsset<Prefab>("FloorCube");
			}
			AddRequiredPrefabsToScene();
			myLightType = GetLightType();
		}
	}
	else
	{
		if (ImGui::Button("Stop Editing"))
		{
			Editor::GetInstance()->GetEditorCamera().LockTransform(false);
			myIsEditing = false;
			myPaintDirection = Direction::None;
			myRMBHeldTime = 0;
			//myShowGizmo = false;
			Editor::GetInstance()->SetPrefabBrushMode(false);
			Editor::GetInstance()->GetGizmos().SetGizmoActive(true);
			Engine::GetInstance()->GetGraphicsEngine()->SetCamera(Editor::GetInstance()->GetEditorCamera().GetCamera());
		}

		Catbox::InputFloat("YHeight", &myYHeight, 1);
		Catbox::InputFloat("Y Scrolling speed", &myYscrollMultiplier);

		if (Catbox::Checkbox("Gizmo?", &myShowGizmo))
		{
			if (myIsEditing)
			{
				if (!myShowGizmo)
				{
					Editor::GetInstance()->SetPrefabBrushMode(true);
					Editor::GetInstance()->GetGizmos().SetGizmoActive(false);
				}
				else
				{
					Editor::GetInstance()->SetPrefabBrushMode(false);
					Editor::GetInstance()->GetGizmos().SetGizmoActive(true);
				}
			}
		}

		if (myShowGizmo)
		{
			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S) && !Input::GetKeyHeld(KeyCode::CTRL))
			{

				std::vector<GameObject*> selectedObjects = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy)->GetSelectedObjects();
				for (size_t i = 0; i < selectedObjects.size(); i++)
				{
					//if (selectedObjects[i]->GetName().find("DeliveryDesk") != std::string::npos)
					//{
					//	Rotate2x1(selectedObjects[i], false);
					//}
					//else
					{
						selectedObjects[i]->GetTransform()->Rotate({ 0,-90.f,0 });
					}
				}
			}
			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_D) && !Input::GetKeyHeld(KeyCode::CTRL))
			{
				std::vector<GameObject*> selectedObjects = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy)->GetSelectedObjects();
				for (size_t i = 0; i < selectedObjects.size(); i++)
				{
					//if (selectedObjects[i]->GetName().find("DeliveryDesk") != std::string::npos)
					//{
					//	Rotate2x1(selectedObjects[i], true);
					//}
					//else
					{
						selectedObjects[i]->GetTransform()->Rotate({ 0,90.f,0 });
					}
				}
			}
		}
		else
		{
			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S) && !Input::GetKeyHeld(KeyCode::CTRL))
			{
				int rot = static_cast<int>(myRotation);
				rot--;
				if (rot < 0) rot = 3;
				myRotation = static_cast<PlaceDirection>(rot);
			}
			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_D) && !Input::GetKeyHeld(KeyCode::CTRL))
			{
				int rot = static_cast<int>(myRotation);
				rot++;
				if (rot > 3) rot = 0;
				myRotation = static_cast<PlaceDirection>(rot);
			}
		}

		std::vector<std::string> directionText = { "North", "East", "South" , "West" };

		if (ImGui::BeginCombo("PlaceDirection##combo", directionText[static_cast<int>(myRotation)].c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < static_cast<int>(PlaceDirection::COUNT); n++)
			{
				bool is_selected = (static_cast<PlaceDirection>(n) == myRotation); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(directionText[n].c_str(), is_selected))
					myRotation = static_cast<PlaceDirection>(n);
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Floor me"))
		{
			GenerateFloor(20, 20);
		}

		if (ImGui::Button("Camera Me"))
		{
			FixCamera();
		}

		KitchenMakerWindow::LevelSettings();
		KitchenMakerWindow::ObjectSettings();
	}
}

GameObject* KitchenMakerWindow::GetKitchen()
{
	auto rootObjects = Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->GetRootObjects();

	GameObject* kitchen = nullptr;
	for (size_t i = 0; i < rootObjects.size(); i++)
	{
		if (!rootObjects[i]->GetPrefab() && rootObjects[i]->GetName() == "Kitchen")
		{
			kitchen = rootObjects[i];
			break;
		}
	}
	if (!kitchen)
	{
		kitchen = Instantiate();
		kitchen->SetName("Kitchen");
	}

	return kitchen;
}

GameObject* KitchenMakerWindow::GetGroupObject(const std::string& aPrefabName)
{
	std::string objectName = aPrefabName + "-Group";

	GameObject* kitchen = GetKitchen();

	auto kitchenObjects = kitchen->GetTransform()->GetChildren();
	GameObject* group = nullptr;
	for (size_t i = 0; i < kitchenObjects.size(); i++)
	{
		if (!kitchenObjects[i]->GetGameObject()->GetPrefab() && kitchenObjects[i]->GetGameObject()->GetName() == objectName)
		{
			group = kitchenObjects[i]->GetGameObject();
			break;
		}
	}
	if (!group)
	{
		group = Instantiate();
		group->SetName(objectName.c_str());
		group->GetTransform()->SetParent(kitchen->GetTransform());
	}


	//GameObject* group = nullptr;
	//for (size_t i = 0; i < rootObjects.size(); i++)
	//{
	//	if (!rootObjects[i]->GetPrefab() && rootObjects[i]->GetName() == objectName)
	//	{
	//		group = rootObjects[i];
	//		break;
	//	}
	//}
	//if (!group)
	//{
	//	group = Instantiate();
	//	group->SetName(objectName.c_str());
	//}

	return group;
}

void KitchenMakerWindow::LevelSettings()
{
	ImGui::Separator();
	ImGui::TextWrapped("Level settings");

	ImGui::SetCursorPos({ ImGui::GetCursorPos().x,ImGui::GetCursorPos().y + 13 });
	ImGui::TextWrapped("Light settings");
	if (ImGui::RadioButton("Fishmarket", &myLightType, 0))
	{
		SetLightType(0);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Beach", &myLightType, 1))
	{
		SetLightType(1);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Shipwreck", &myLightType, 2))
	{
		SetLightType(2);
	}

	ImGui::SetCursorPos({ ImGui::GetCursorPos().x,ImGui::GetCursorPos().y + 13 });
	if (myKitchenData.currentLevel < 0)
	{
		ImGui::TextWrapped("Not a valid level for rest of the settings");
		return;
	}
	Camera* cam = FindGameCamera();

	if (cam)
	{
		ImGui::InputInt("Bronze", &myKitchenData.bronzeScore);
		ImGui::InputInt("Silver", &myKitchenData.silverScore);
		ImGui::InputInt("Gold", &myKitchenData.goldScore);
		ImGui::InputInt("Level Length (seconds)", &myKitchenData.levelDuration);
	}

	if (ImGui::TreeNode("Recipes"))
	{
		bool temp = false;
		for (size_t i = 0; i < myRecipeNames.size(); i++)
		{
			temp = myKitchenData.recipes[i];
			ImGui::Checkbox(myRecipeNames[i].c_str(), &temp);
			myKitchenData.recipes[i] = temp;
		}
		ImGui::TreePop();
	}

	if (ImGui::Button("Save Level##Data"))
	{
		std::string levelName = Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->GetName();
		myShouldSave = true;
	}
}

void KitchenMakerWindow::ObjectSettings()
{
	std::vector<GameObject*> selectedObjects = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy)->GetSelectedObjects();
	if (!selectedObjects.empty())
	{
		if (auto parent = selectedObjects[0]->GetTransform()->GetParent())
		{
			if (parent->GetGameObject()->GetName() == "Crab")
			{
				auto allComponents = parent->GetGameObject()->GetComponents();
				for (auto component : allComponents)
				{
					if (component->GetName() == "Moving Platform")
					{
						ImGui::Separator();
						ImGui::TextWrapped("  Crab Settings");
						std::vector<Component*> uuuuuh;
						component->RenderInProperties(uuuuuh);
					}
				}
			}
		}
		if (selectedObjects[0]->GetName() == "MovingPlatform")
		{
			auto allComponents = selectedObjects[0]->GetComponents();
			for (auto component : allComponents)
			{
				if (component->GetName() == "Moving Platform")
				{
					ImGui::Separator();
					ImGui::TextWrapped("  Moving Platform Settings");
					std::vector<Component*> uuuuuh;
					component->RenderInProperties(uuuuuh);
				}
			}
		}
	}

	if (GameObject* movingInteractable = FindMovingInteractable())
	{
		auto allComponents = movingInteractable->GetComponents();
		for (auto component : allComponents)
		{
			if (component->GetName() == "Moving Interactable")
			{
				ImGui::Separator();
				ImGui::TextWrapped("  Fishes go splash splash");
				std::vector<Component*> uuuuuh;
				component->RenderInProperties(uuuuuh);
			}
		}
	}
}

void KitchenMakerWindow::RemoveObject()
{
	if (auto obj = GameObjectSelection::GetHoveredObject())
	{
		Editor::GetInstance()->GetGizmos().ClearSelectedObjects();
		obj->SetSelected(0, false);

		if (mySelectedPrefab->GetPathStr().find("Holdables") == std::string::npos)
		{
			auto obj2 = obj->GetTransform()->GetRootParent()->GetGameObject();
			if (obj2 && obj2->GetPrefab())
			{
				if (Input::GetKeyHeld(KeyCode::SHIFT) || (obj2->GetPrefab() && (obj2->GetPrefab()->GetName() == mySelectedPrefab->GetName() && abs(obj2->GetTransform()->worldPos().y - myYHeight) < 0.01f)))
				{
					if (obj2->GetTransform()->GetParent() && obj2->GetTransform()->GetParent()->GetGameObject()->GetName() == "Crab")
					{
						obj2->GetTransform()->GetParent()->GetGameObject()->Destroy();
					}
					else
					{
						obj2->Destroy();
					}
				}
			}
			else if ((Input::GetKeyHeld(KeyCode::SHIFT) && ((obj->GetPrefab() && obj->GetPrefab()->GetPathStr().find("Holdables") == std::string::npos) || !obj->GetPrefab())) 	|| (((obj->GetPrefab() && (obj->GetPrefab()->GetName() == mySelectedPrefab->GetName()) || mySelectedPrefab->GetName() == "Crab" || obj->GetName() == mySelectedPrefab->GetName()) && abs(obj->GetTransform()->worldPos().y - myYHeight) < 0.8f)))
			{
				if (obj->GetName() == "Teleporter")
				{
					if (obj->GetTransform()->GetParent())
					{
						if (obj->GetTransform()->GetParent()->GetGameObject()->GetName() == "Crab")
						{
							obj->GetTransform()->GetParent()->GetParent()->GetGameObject()->Destroy();
						}
						else
						{
							obj->GetTransform()->GetParent()->GetGameObject()->Destroy();
						}
					}
				}
				else
				{
					if (obj->GetTransform()->GetParent() && obj->GetTransform()->GetParent()->GetGameObject()->GetName() == "Crab")
					{
						obj->GetTransform()->GetParent()->GetGameObject()->Destroy();
					}
					else
					{
						obj->Destroy();
					}
				}
			}
		}
		else
		{
			auto obj2 = obj->GetTransform()->GetRootParent()->GetGameObject();
			if (obj2 && obj2->GetPrefab())
			{
				if (Input::GetKeyHeld(KeyCode::SHIFT))
				{
					if (obj2->GetTransform()->GetParent() && obj2->GetTransform()->GetParent()->GetGameObject()->GetName() == "Crab")
					{
						obj2->GetTransform()->GetParent()->GetGameObject()->Destroy();
					}
					else
					{
						obj2->Destroy();
					}
				}
				else if (obj2->GetPrefab() && ((obj->GetPrefab()->GetName().find(mySelectedPrefab->GetName()) != std::string::npos) && abs(obj2->GetTransform()->worldPos().y - myYHeight) < 0.01f))
				{
					FindHoldableParent(obj2);
				}
			}
			else
			{
				if ((Input::GetKeyHeld(KeyCode::SHIFT) && ((obj->GetPrefab() && obj->GetPrefab()->GetPathStr().find("Holdables") == std::string::npos) || !obj->GetPrefab())))
				{
					if (obj->GetName() == "Teleporter")
					{
						if (obj->GetTransform()->GetParent())
						{
							if (obj->GetTransform()->GetParent()->GetGameObject()->GetName() == "Crab")
							{
								obj->GetTransform()->GetParent()->GetParent()->GetGameObject()->Destroy();
							}
							else
							{
								obj->GetTransform()->GetParent()->GetGameObject()->Destroy();
							}
						}
					}
					else
					{
						if (obj->GetTransform()->GetParent() && obj->GetTransform()->GetParent()->GetGameObject()->GetName() == "Crab")
						{
							obj->GetTransform()->GetParent()->GetGameObject()->Destroy();
						}
						else
						{
							obj->Destroy();
						}
					}
				}
				else if (obj->GetPrefab() && ((obj->GetPrefab()->GetName().find(mySelectedPrefab->GetName()) != std::string::npos) && abs(obj->GetTransform()->worldPos().y - myYHeight) < 0.8f) && obj->GetPrefab()->GetPathStr().find("Holdables") == std::string::npos)
				{
					FindHoldableParent(obj);
				}
			}
		}
	}
}

void KitchenMakerWindow::FindHoldableParent(GameObject* parent)
{
	if (parent->GetName() == mySelectedPrefab->GetName() && parent->GetTransform()->GetParent())
	{
		RemoveHoldableFromParent(parent->GetTransform()->GetParent()->GetGameObject());
	}
	else if (!parent->GetTransform()->GetChildren().empty())
	{
		auto children = parent->GetTransform()->GetChildren();
		for (size_t i = 0; i < children.size(); i++)
		{
			if (children[i]->GetGameObject()->GetName() == mySelectedPrefab->GetName())
			{
				RemoveHoldableFromParent(parent);
				return;
			}
		}
	}
}

void KitchenMakerWindow::RemoveHoldableFromParent(GameObject* parent)
{
	std::string oldName = parent->GetName();
	int pos = oldName.find("_" + mySelectedPrefab->GetName());
	if (pos != std::string::npos)
	{
		std::string newName = oldName.erase(pos);
		auto newObject = InstantiatePrefab(newName);
		if (newObject)
		{
			auto test = newObject->GetComponents();
			auto test2 = test[0]->GetName();
			newObject->GetTransform()->SetParent(parent->GetTransform()->GetParent());
			newObject->GetTransform()->SetWorldPos(parent->GetTransform()->worldPos());
			newObject->GetTransform()->SetWorldRot(parent->GetTransform()->worldRot());
			parent->Destroy();
		}
	}
	else
	{
		printerror("KitchenMakerWindow::RemoveHoldableFromParent: Oldname did not contain prefabs name or something.");
	}
}

void KitchenMakerWindow::CreateObject()
{
	Catbox::Vector3<float> ClickPosNear = GetClickPos();

	GameObject* group = GetGroupObject(mySelectedPrefab->GetName());
	GameObject* movingPlatformGroup = GetGroupObject("MovingPlatform");

	ClickPosNear = SnapToGridCenteredPivotBottom(ClickPosNear);

	if (Input::GetKeyHeld(KeyCode::SHIFT))
	{
		if (myPaintDirection == Direction::Undetermined)
		{
			if (abs(ClickPosNear.x - myStartingPaintPos.x) < 0.01f && abs(ClickPosNear.z - myStartingPaintPos.z) > 0.01f)
			{
				myPaintDirection = Direction::Horizontal;
			}
			else if (abs(ClickPosNear.x - myStartingPaintPos.x) > 0.01f && abs(ClickPosNear.z - myStartingPaintPos.z) < 0.01f)
			{
				myPaintDirection = Direction::Vertical;
			}
			else
			{
				return; //Guesstimating here.
			}
		}
		if (myPaintDirection == Direction::None)
		{
			myPaintDirection = Direction::Undetermined;
			myStartingPaintPos = ClickPosNear;
		}
		if (myPaintDirection == Direction::Horizontal && abs(ClickPosNear.x - myStartingPaintPos.x) > 0.01f)
		{
			return;
		}
		if (myPaintDirection == Direction::Vertical && abs(ClickPosNear.z - myStartingPaintPos.z) > 0.01f)
		{
			return;
		}

	}

	if (mySelectedPrefab->GetPathStr().find("FloorCube") != std::string::npos || mySelectedPrefab->GetPathStr().find("MovingPlatform") != std::string::npos)
	{
		ClickPosNear -= {0.5f, 0, 0.5f};
	}

	for (auto objects : group->GetTransform()->GetChildren())
	{
		if (abs(ClickPosNear.x - objects->worldPos().x) < 0.01f && abs(ClickPosNear.y - objects->worldPos().y) < 0.01f && abs(ClickPosNear.z - objects->worldPos().z) < 0.01f)
		{
			return;
		}
	}
	for (auto platforms : movingPlatformGroup->GetTransform()->GetChildren())
	{
		for (auto objectOnPlatform : platforms->GetChildren())
		{
			if ((objectOnPlatform->GetGameObject()->GetName() == mySelectedPrefab->GetName() || (objectOnPlatform->GetGameObject()->GetPrefab() && objectOnPlatform->GetGameObject()->GetPrefab() == mySelectedPrefab)) && abs(ClickPosNear.x - objectOnPlatform->worldPos().x) < 0.01f && abs(ClickPosNear.y - objectOnPlatform->worldPos().y) < 0.01f && abs(ClickPosNear.z - objectOnPlatform->worldPos().z) < 0.01f)
			{
				return;
			}
		}
		Vector3f platPos = platforms->worldPos();
		Vector3f platScale = platforms->worldScale();
		if (ClickPosNear.x >= platPos.x &&
			ClickPosNear.x <= platPos.x + (platScale.x * 100) &&
			ClickPosNear.y >= platPos.y + 0.5f &&
			ClickPosNear.y <= platPos.y + (platScale.y * 100) + 0.5f &&
			ClickPosNear.z >= platPos.z &&
			ClickPosNear.z <= platPos.z + (platScale.z * 100))
		{
			group = platforms->GetGameObject();
		}
	}

	auto object = InstantiatePrefab(mySelectedPrefab);
	object->GetTransform()->SetParent(group->GetTransform());
	object->GetTransform()->SetWorldPos(ClickPosNear);
	object->GetTransform()->SetWorldRot({ 0,90.f * static_cast<int>(myRotation),0 });
	if (mySelectedPrefab->GetPathStr().find("FloorCube") != std::string::npos || mySelectedPrefab->GetPathStr().find("MovingPlatform") != std::string::npos)
	{
		object->GetTransform()->SetWorldRot({ 0,0,0, });
	}
	if (mySelectedPrefab->GetName() == "MovingPlatform")
	{
		object->GetTransform()->SetWorldRot({ 0,0,0, });
		object->SetPrefab(nullptr);
	}
}

void KitchenMakerWindow::CreateHoldable()
{
	Catbox::Vector3<float> ClickPosNear = GetClickPos();

	ClickPosNear = SnapToGridCenteredPivotBottom(ClickPosNear);

	//This is insanity
	Transform* newParent = nullptr;

	std::vector<GameObject*> counterGroup;
	std::vector<Transform*> platformGroup;
	std::vector<std::string> counterGroupNames;
	counterGroupNames.push_back("Counter_Fishmarket");
	counterGroupNames.push_back("Counter_Beach");
	counterGroupNames.push_back("Counter_Shipwreck");
	counterGroupNames.push_back("Stove");
	counterGroup.push_back(GetGroupObject(counterGroupNames[0]));
	counterGroup.push_back(GetGroupObject(counterGroupNames[1]));
	counterGroup.push_back(GetGroupObject(counterGroupNames[2]));
	counterGroup.push_back(GetGroupObject(counterGroupNames[3]));
	platformGroup = GetGroupObject("MovingPlatform")->GetTransform()->GetChildren();


	for (size_t i = 0; i < counterGroup.size(); i++)
	{
		auto objectsInGroup = counterGroup[i]->GetTransform()->GetChildren();
		for (size_t object = 0; object < objectsInGroup.size(); object++)
		{
			if (abs(ClickPosNear.x - objectsInGroup[object]->worldPos().x) < 0.01f && abs(ClickPosNear.y - objectsInGroup[object]->worldPos().y) < 0.1f && abs(ClickPosNear.z - objectsInGroup[object]->worldPos().z) < 0.01f)
			{
				if (objectsInGroup[object]->GetGameObject()->GetName() == counterGroupNames[i])
				{
					newParent = objectsInGroup[object];
					ClickPosNear -= {0.5f, 0, 0.5f};
				}
				break;
			}
		}
	}

	if (!newParent)
	{
		for (size_t i = 0; i < platformGroup.size(); i++)
		{
			auto objectsOnPlatform = platformGroup[i]->GetChildren();
			if (newParent)
			{
				break;
			}

			for (auto object : objectsOnPlatform)
			{
				if (object->GetGameObject()->GetName() == counterGroupNames[0]
					|| object->GetGameObject()->GetName() == counterGroupNames[1]
					|| object->GetGameObject()->GetName() == counterGroupNames[2]
					|| object->GetGameObject()->GetName() == counterGroupNames[3])
				{
					if (abs(ClickPosNear.x - object->worldPos().x) < 0.01f && abs(ClickPosNear.y - object->worldPos().y) < 0.1f && abs(ClickPosNear.z - object->worldPos().z) < 0.01f)
					{
						newParent = object;
						ClickPosNear -= {0.5f, 0, 0.5f}; //not sure why this one is here. Copy paste from above. Doesnt seem like it is ever used. Delete?
						break;
					}
				}
			}
		}
	}


	if (newParent)
	{
		if (newParent->GetGameObject()->GetName() + "_" + mySelectedPrefab->GetName() == "Stove_Plate")
		{
			return;
		}
		auto object = InstantiatePrefab(newParent->GetGameObject()->GetName() + "_" + mySelectedPrefab->GetName());
		if (object)
		{
			object->GetTransform()->SetParent(newParent->GetParent());
			object->GetTransform()->SetWorldPos(newParent->worldPos());
			object->GetTransform()->SetWorldRot(newParent->worldRot());
			newParent->GetGameObject()->Destroy();
		}
		else
		{
			print("prefab " + newParent->GetGameObject()->GetName() + "_" + mySelectedPrefab->GetName() + " not found");
		}
	}

}

void KitchenMakerWindow::CreateTeleporter()
{
	Catbox::Vector3<float> ClickPosNear = GetClickPos();

	ClickPosNear = SnapToGridCenteredPivotBottom(ClickPosNear);

	GameObject* group = GetGroupObject(mySelectedPrefab->GetName());

	auto groupChildren = group->GetTransform()->GetChildren();

	GameObject* teleportSubGroup = nullptr;

	for (size_t i = 0; i < groupChildren.size(); i++)
	{
		for (size_t j = 0; j < groupChildren[i]->GetChildren().size(); j++)
		{
			auto objects = groupChildren[i]->GetChildren()[j];
			if (abs(ClickPosNear.x - objects->worldPos().x) < 0.01f && abs(ClickPosNear.y - objects->worldPos().y) < 0.01f && abs(ClickPosNear.z - objects->worldPos().z) < 0.01f)
			{
				return;
			}
		}
	}

	for (size_t i = 0; i < groupChildren.size(); i++)
	{
		if (groupChildren[i]->GetChildren().size() < 2)
		{
			teleportSubGroup = groupChildren[i]->GetGameObject();
			break;
		}
	}

	if (teleportSubGroup == nullptr)
	{
		teleportSubGroup = Instantiate();
		teleportSubGroup->SetName("TeleportPair");
		teleportSubGroup->GetTransform()->SetParent(group->GetTransform());
	}

	GameObject* teleport = InstantiatePrefab(mySelectedPrefab);
	teleport->GetTransform()->SetParent(teleportSubGroup->GetTransform());
	teleport->GetTransform()->SetWorldPos(ClickPosNear);
	teleport->GetTransform()->SetWorldRot({ 0,90.f * static_cast<int>(myRotation),0 });
}

Vector3f KitchenMakerWindow::GetClickPos()
{
	float anYvalue = myYHeight;

	auto aMousePos = Input::GetMousePosition();
	Catbox::Vector2<int> mouseViewport = Engine::GetInstance()->ViewportToScreenPos(static_cast<int>(aMousePos.x), static_cast<int>(aMousePos.y));

	int winWidth = DX11::GetResolution().x;
	int winHeight = DX11::GetResolution().y;
	float normalised_x = 2 * mouseViewport.x / static_cast<float>(winWidth) - 1;
	float normalised_y = 1 - 2 * mouseViewport.y / static_cast<float>(winHeight);

	Catbox::Vector4<float> clipCoordsNear = { normalised_x, normalised_y , 0, 1 }; //Nearplane

	Catbox::Matrix4x4<float> invereprojection = Catbox::Matrix4x4<float>::GetFastInverse(Engine::GetInstance()->GetGraphicsEngine()->GetMainCamera()->GetProjectionMatrix());
	Catbox::Matrix4x4<float> inverseViewMatrix = Engine::GetInstance()->GetGraphicsEngine()->GetMainCamera()->GetTransform()->GetWorldTransformMatrix();

	//convert from 2d to a 3d direction
	Catbox::Vector4<float> clipCoordsInCameraSpaceNear = clipCoordsNear * invereprojection;
	clipCoordsInCameraSpaceNear.z = 1;
	clipCoordsInCameraSpaceNear.w = 0;

	clipCoordsInCameraSpaceNear = clipCoordsInCameraSpaceNear * inverseViewMatrix;

	Catbox::Vector3<float> RayDirectionNear = { clipCoordsInCameraSpaceNear.x, clipCoordsInCameraSpaceNear.y, clipCoordsInCameraSpaceNear.z };

	RayDirectionNear.Normalize();

	Catbox::Vector3<float> CameraPos = Engine::GetInstance()->GetGraphicsEngine()->GetMainCamera()->GetTransform()->worldPos();

	float multiplierNear = (myYHeight - CameraPos.y) / RayDirectionNear.y;

	Catbox::Vector3<float> ClickPosNear = CameraPos + (RayDirectionNear * multiplierNear);

	return ClickPosNear;
}

void KitchenMakerWindow::GenerateFloor(int x, int z)
{
	GameObject* parent = GetGroupObject("FloorCube");
	for (size_t xAxis = 0; xAxis < x; xAxis++)
	{
		for (size_t zAxis = 0; zAxis < z; zAxis++)
		{
			auto object = InstantiatePrefab("FloorCube");
			object->GetTransform()->SetWorldPos({ xAxis * myGridSize - (x / 2) * myGridSize, myYHeight - 1 ,zAxis * myGridSize - (z / 2) * myGridSize });
			object->GetTransform()->SetParent(parent->GetTransform());
		}
	}
}

void KitchenMakerWindow::HandleScroll()
{
	if (myYscroll > 1.f)
	{
		myYscroll = 0;
		myYHeight += 1;
		std::string num_text = std::to_string(myYHeight);
		std::string rounded = num_text.substr(0, num_text.find(".") + 3);
		PopupManager::CreatePopup("KitchenYHeight", 2, { 350,75 }, "Y Height set to: " + rounded, { Engine::GetInstance()->GetWindowSize().x * 0.5f, 400 }, true);
	}
	if (myYscroll < -1.f)
	{
		myYscroll = 0;
		myYHeight -= 1;
		std::string num_text = std::to_string(myYHeight);
		std::string rounded = num_text.substr(0, num_text.find(".") + 3);
		PopupManager::CreatePopup("KitchenYHeight", 2, { 350,75 }, "Y Height set to: " + rounded, { Engine::GetInstance()->GetWindowSize().x * 0.5f, 400 }, true);
	}
}

void KitchenMakerWindow::RefreshAssetList()
{
	myAssets.clear();
	auto aff = AssetRegistry::GetInstance()->GetAssets<Prefab>();
	for (size_t i = 0; i < aff.size(); i++)
	{
		if (aff[i]->GetPathStr().find("Interactables") != std::string::npos || aff[i]->GetPathStr().find("Floor") != std::string::npos || aff[i]->GetName() == "Player" || aff[i]->GetName() == "MovingInteractable")
		{
			if (aff[i]->GetPathStr().find("_Rice") == std::string::npos
				&& aff[i]->GetName() != "CuttingBoard"
				&& aff[i]->GetName() != "Counter"
				&& aff[i]->GetName() != "Counter1"
				&& aff[i]->GetName() != "Counter2")
				myAssets.push_back(std::static_pointer_cast<Prefab>(aff[i]));
		}
	}
	//std::shared_ptr<Prefab> crab = std::make_shared<Prefab>();
	//crab->SetName("Crab");
	//myAssets.push_back(crab);
}

void KitchenMakerWindow::FixCamera()
{
	SetCameraSettings(FindGameCamera());
	Engine::GetInstance()->GetGraphicsEngine()->SetCamera(FindGameCamera());
}

void KitchenMakerWindow::SetCameraSettings(Camera* aCamera)
{
	auto transform = aCamera->GetTransform();
	//grid 0.9 camera
	//transform->SetWorldPos({ -1.464f, 10.613f, 8.380f });
	//transform->SetWorldRot({ -123.6f, 0.f, 180.f });
	//transform->SetWorldScale({ 1.f, 1.f, 1.f });

	//grid 1 camera
	//transform->SetWorldPos({ -1.627f, 11.75f, 9.311f });
	//transform->SetWorldRot({ -123.6f, 0.f, 180.f });
	//transform->SetWorldScale({ 1.f, 1.f, 1.f });

	//grid 1 camera rotation 180
	transform->SetWorldPos({ 1.627f, 11.75f, -9.311f });
	transform->SetWorldRot({ -123.6f, 180.f, 180.f });
	transform->SetWorldScale({ 1.f, 1.f, 1.f });

	aCamera->SetNearPlane(0.100f);
	aCamera->SetFarPlane(1000.f);
	aCamera->SetFoV(80.200f); //this needs to be last because it is the one that creates a new projection matrix
}

Camera* KitchenMakerWindow::FindGameCamera()
{
	auto allCameras = Engine::GetInstance()->GetCameraController()->GetCameras();
	Camera* mainCamera = nullptr;

	for (size_t i = 0; i < allCameras.size(); i++)
	{
		if (allCameras[i]->GetName() == "Camera")
		{
			//if (allCameras[i]->GetGameObject().GetPrefab())
			//{
				mainCamera = allCameras[i];
				break;
			//}
			//else
			//{
			//	allCameras[i]->GetGameObject().Destroy();
			//}
		}
	}

	if (!mainCamera)
	{
		auto newCamGameObject = InstantiatePrefab("Camera");
		mainCamera = newCamGameObject->GetComponent<Camera>();
		SetCameraSettings(mainCamera);
	}

	if (mainCamera->GetTransform()->GetChildren().empty())
	{
		GameObject* helper = InstantiatePrefab("KitchenEditorHelper");
		helper->GetTransform()->SetParent(mainCamera->GetTransform());
	}

	return mainCamera;
}

Vector3f KitchenMakerWindow::SnapToGridCornerPivot(const Vector3f& aClickedPos)
{
	int xMultiplier = static_cast<int>(aClickedPos.x / myGridSize);
	int zMultiplier = static_cast<int>(aClickedPos.z / myGridSize);
	if (aClickedPos.x < 0)
	{
		xMultiplier--;
	}
	if (aClickedPos.z < 0)
	{
		zMultiplier--;
	}

	Vector3f newPos = aClickedPos;
	newPos.x = xMultiplier * myGridSize;
	newPos.z = zMultiplier * myGridSize;
	return newPos;
}

Vector3f KitchenMakerWindow::SnapToGridCenteredPivotBottom(const Vector3f& aClickedPos)
{
	int xMultiplier = static_cast<int>(aClickedPos.x / myGridSize);
	int zMultiplier = static_cast<int>(aClickedPos.z / myGridSize);
	if (aClickedPos.x < 0)
	{
		xMultiplier--;
	}
	if (aClickedPos.z < 0)
	{
		zMultiplier--;
	}

	//print("grid point = " + std::to_string(xMultiplier) + "," + std::to_string(zMultiplier));

	Vector3f newPos = aClickedPos;
	newPos.x = xMultiplier * myGridSize + 0.5f * myGridSize;
	newPos.z = zMultiplier * myGridSize + 0.5f * myGridSize;
	return newPos;
}

void KitchenMakerWindow::ClearSelectedObjects()
{
	auto selectedObjects = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy)->GetSelectedObjects();
	for (size_t i = 0; i < selectedObjects.size(); i++)
	{
		selectedObjects[i]->Deselect();
	}
	Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy)->ClearSelectedObjects();
}

void KitchenMakerWindow::Rotate2x1(GameObject* anOutGameObject, bool aClockwise)
{
	float angle = fmodf(anOutGameObject->GetTransform()->worldRot().y, 360.f);
	//if (anOutGameObject->GetName().find("DeliveryDesk") != std::string::npos)
	//{
	//	angle -= 90;
	//}
	if (angle < 0) angle += 360.f;
	print("float angle = " + std::to_string(angle));

	auto oldPos = anOutGameObject->GetTransform()->worldPos();
	Vector3f posDiff;
	//Ugly. Might fix later
	if (aClockwise)
	{
		if (abs(angle) < 0.5f)
		{
			posDiff.x -= (myGridSize / 2.f);
			posDiff.z -= (myGridSize / 2.f);
		}
		else if (abs(angle - 90) < 0.5f)
		{
			posDiff.z += (myGridSize / 2.f);
			posDiff.x -= (myGridSize / 2.f);
		}
		else if (abs(angle - 180) < 0.5f)
		{
			posDiff.x += (myGridSize / 2.f);
			posDiff.z += (myGridSize / 2.f);
		}
		else if (abs(angle - 270) < 0.5f)
		{
			posDiff.z -= (myGridSize / 2.f);
			posDiff.x += (myGridSize / 2.f);
		}
		anOutGameObject->GetTransform()->Rotate({ 0,90.f,0 });
	}
	else
	{
		if (abs(angle) < 0.5f)
		{
			posDiff.x -= (myGridSize / 2.f);
			posDiff.z += (myGridSize / 2.f);
		}
		else if (abs(angle - 90) < 0.5f)
		{
			posDiff.z += (myGridSize / 2.f);
			posDiff.x += (myGridSize / 2.f);
		}
		else if (abs(angle - 180) < 0.5f)
		{
			posDiff.x += (myGridSize / 2.f);
			posDiff.z -= (myGridSize / 2.f);
		}
		else if (abs(angle - 270) < 0.5f)
		{
			posDiff.z -= (myGridSize / 2.f);
			posDiff.x -= (myGridSize / 2.f);
		}

		anOutGameObject->GetTransform()->Rotate({ 0,-90.f,0 });
	}
	anOutGameObject->GetTransform()->SetWorldPos(oldPos + posDiff);
}

void KitchenMakerWindow::AddRequiredPrefabsToScene()
{
	GameObject* levelTimer = GetPrefab("LevelTimer");
	GameObject* levelCoinCounter = GetPrefab("LevelCoinCounter");
	GameObject* pauseMenu = GetPrefab("PauseMenu");
	GameObject* settingsMenu = GetPrefab("SettingsMenu");
	GameObject* audioPlayer = GetPrefab("AudioPlayer");


	levelTimer->GetTransform()->SetWorldPos(0.670f, -0.700f, 0);
	levelCoinCounter->GetTransform()->SetWorldPos(-0.679f, -0.700f, 0);
	pauseMenu->GetTransform()->SetWorldPos(0, 0, 0);
}

GameObject* KitchenMakerWindow::GetPrefab(const std::string& aPrefab)
{
	auto rootObjects = Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->GetRootObjects();

	GameObject* prefab = nullptr;
	for (size_t i = 0; i < rootObjects.size(); i++)
	{
		if (rootObjects[i]->GetPrefab() && rootObjects[i]->GetPrefab()->GetName() == aPrefab)
		{
			prefab = rootObjects[i];
			break;
		}
	}
	if (!prefab)
	{
		prefab = InstantiatePrefab(aPrefab);
	}

	return prefab;
}

int KitchenMakerWindow::GetLightType()
{
	auto rootObjects = Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->GetRootObjects();

	for (size_t i = 0; i < rootObjects.size(); i++)
	{
		if (rootObjects[i]->GetPrefab() && rootObjects[i]->GetPrefab()->GetName() == "FishMarketLighting")
		{
			return 0;
		}

		if (rootObjects[i]->GetPrefab() && rootObjects[i]->GetPrefab()->GetName() == "BeachLighting")
		{
			return 1;
		}

		if (rootObjects[i]->GetPrefab() && rootObjects[i]->GetPrefab()->GetName() == "ShipwreckLighting")
		{
			return 2;
		}
	}

	GetPrefab("FishmarketLighting");

	return 0;
}

void KitchenMakerWindow::SetLightType(int aLightType)
{
	if (aLightType < 0) return;
	if (aLightType > 2) return;

	auto rootObjects = Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->GetRootObjects();

	GameObject* prefab = nullptr;
	for (size_t i = 0; i < rootObjects.size(); i++)
	{
		if (aLightType != 0 && rootObjects[i]->GetPrefab() && rootObjects[i]->GetPrefab()->GetName() == "FishMarketLighting")
		{
			rootObjects[i]->Destroy();
		}

		if (aLightType != 1 && rootObjects[i]->GetPrefab() && rootObjects[i]->GetPrefab()->GetName() == "BeachLighting")
		{
			rootObjects[i]->Destroy();
		}

		if (aLightType != 2 && rootObjects[i]->GetPrefab() && rootObjects[i]->GetPrefab()->GetName() == "ShipwreckLighting")
		{
			rootObjects[i]->Destroy();
		}

		if (rootObjects[i]->GetName() == "Light" || rootObjects[i]->GetName() == "EnvLight")
		{
			rootObjects[i]->Destroy();
		}
	}

	if (aLightType == 0)
	{
		GetPrefab("FishmarketLighting");
	}
	if (aLightType == 1)
	{
		GetPrefab("BeachLighting");
	}
	if (aLightType == 2)
	{
		GetPrefab("ShipwreckLighting");
	}
}

void KitchenMakerWindow::AddMovingInteractable()
{
	GameObject* movingInteractable = FindMovingInteractable();

	if (!movingInteractable)
	{
		movingInteractable = InstantiatePrefab(mySelectedPrefab);
		movingInteractable->SetPrefab(nullptr);
	}
}

void KitchenMakerWindow::RemoveMovingInteractable()
{
	GameObject* movingInteractable = FindMovingInteractable();

	if (movingInteractable)
	{
		movingInteractable->Destroy();
	}
}

GameObject* KitchenMakerWindow::FindMovingInteractable()
{
	auto rootObjects = Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->GetRootObjects();

	GameObject* movingInteractable = nullptr;
	for (size_t i = 0; i < rootObjects.size(); i++)
	{
		if (rootObjects[i]->GetName() == "MovingInteractable")
		{
			movingInteractable = rootObjects[i];
			break;
		}
	}

	return movingInteractable;
}

void KitchenMakerWindow::Crabify()
{
	if (auto obj = GameObjectSelection::GetHoveredObject())
	{
		Editor::GetInstance()->GetGizmos().ClearSelectedObjects();
		obj->SetSelected(0, false);
		float high = obj->GetTransform()->worldPos().y;
		float result = abs(obj->GetTransform()->worldPos().y - myYHeight);

		if (mySelectedPrefab->GetName() == "Crab" &&
			abs(obj->GetTransform()->worldPos().y - myYHeight) < 0.01f &&
			obj->GetPrefab() &&
			obj->GetTransform()->GetParent() &&
			obj->GetTransform()->GetParent()->GetGameObject()->GetName() != "Crab")
		{
			GameObject* crabObject = InstantiatePrefab(mySelectedPrefab);
			crabObject->GetTransform()->SetParent(obj->GetTransform()->GetParent());
			crabObject->GetTransform()->SetWorldPos(obj->GetTransform()->worldPos());
			obj->GetTransform()->SetParent(crabObject->GetTransform());
			crabObject->SetPrefab(nullptr);
		}
	}
}