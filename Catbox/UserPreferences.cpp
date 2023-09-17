#include "stdafx.h"
#include "UserPreferences.h"
#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include "Editor/Windows/EditorWindow.h"


void UserPreferences::StyleColorsCustom(Color& aMainColor, Color& aSecondaryColor, Color& aTextColor)
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	Color main = aMainColor;
	Color mainLighter = aMainColor;
	mainLighter.r += 0.1f;
	mainLighter.g += 0.1f;
	mainLighter.b += 0.1f;
	Color mainLighter2 = aMainColor;
	mainLighter2.r += 0.2f;
	mainLighter2.g += 0.2f;
	mainLighter2.b += 0.2f;
	Color mainDarker = aMainColor * 0.8f;
	Color mainDarker2 = aMainColor * 0.6f;

	colors[ImGuiCol_WindowBg] = ImVec4(main.r, main.g, main.b, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(main.r, main.g, main.b, 0.58f);
	colors[ImGuiCol_PopupBg] = ImVec4(mainLighter.r, mainLighter.g, mainLighter.b, 0.90f);
	colors[ImGuiCol_FrameBg] = ImVec4(mainLighter.r, mainLighter.g, mainLighter.b, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(main.r, main.g, main.b, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(mainLighter2.r, mainLighter2.g, mainLighter2.b, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(mainDarker.r, mainDarker.g, mainDarker.b, 1);

	Color secondary = aSecondaryColor;
	Color secondaryLighter = aSecondaryColor;
	secondaryLighter.r += 0.05f;
	secondaryLighter.g += 0.05f;
	secondaryLighter.b += 0.05f;
	Color secondaryLighter2 = aSecondaryColor;
	secondaryLighter2.r += 0.1f;
	secondaryLighter2.g += 0.1f;
	secondaryLighter2.b += 0.1f;

	Color secondaryLighter3 = aSecondaryColor * 1.4f;
	Color secondaryDarker = aSecondaryColor * 0.8f;
	Color secondaryDarker2 = aSecondaryColor * 0.6f;
	Color secondaryDarker3 = aSecondaryColor * 0.4f;


	colors[ImGuiCol_Border] = ImVec4(secondaryDarker.r, secondaryDarker.g, secondaryDarker.b, 0.60f);
	colors[ImGuiCol_BorderShadow] = ImVec4(secondaryDarker2.r, secondaryDarker2.g, secondaryDarker2.b, 0.60f);

	colors[ImGuiCol_MenuBarBg] = ImVec4(secondaryDarker.r, secondaryDarker.g, secondaryDarker.b, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(secondaryDarker.r, secondaryDarker.g, secondaryDarker.b, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(secondaryDarker.r, secondaryDarker.g, secondaryDarker.b, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(secondaryDarker.r, secondaryDarker.g, secondaryDarker.b, 1.00f);

	Color text = aTextColor;
	colors[ImGuiCol_Text] = ImVec4(text.r, text.g, text.b, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(text.r, text.g, text.b, 0.4f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(mainLighter2.r, mainLighter2.g, mainLighter2.b, 1.00f);

	colors[ImGuiCol_Button] = ImVec4(secondaryDarker.r, secondaryDarker.g, secondaryDarker.b, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(secondaryDarker2.r, secondaryDarker2.g, secondaryDarker2.b, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(secondaryDarker3.r, secondaryDarker3.g, secondaryDarker3.b, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(secondary.r, secondary.g, secondary.b, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(secondaryDarker2.r, secondaryDarker2.g, secondaryDarker2.b, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(secondary.r, secondary.g, secondary.b, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(secondaryDarker.r, secondaryDarker.g, secondaryDarker.b, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(secondaryDarker2.r, secondaryDarker2.g, secondaryDarker2.b, 1.00f);

	colors[ImGuiCol_CheckMark] = ImVec4(secondaryLighter2.r, secondaryLighter2.g, secondaryLighter2.b, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(secondary.r, secondary.g, secondary.b, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(secondaryDarker.r, secondaryDarker.g, secondaryDarker.b, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(secondaryDarker2.r, secondaryDarker2.g, secondaryDarker2.b, 1.00f);

	colors[ImGuiCol_Separator] = ImVec4(secondary.r, secondary.g, secondary.b, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(secondaryDarker.r, secondaryDarker.g, secondaryDarker.b, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(secondaryDarker2.r, secondaryDarker2.g, secondaryDarker2.b, 1.00f);

	colors[ImGuiCol_Tab] = ImVec4(secondaryDarker.r, secondaryDarker.g, secondaryDarker.b, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(secondaryDarker2.r, secondaryDarker2.g, secondaryDarker2.b, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(secondary.r, secondary.g, secondary.b, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(secondaryDarker.r, secondaryDarker.g, secondaryDarker.b, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(secondary.r, secondary.g, secondary.b, 1.00f);

	colors[ImGuiCol_DockingPreview] = ImVec4(secondary.r, secondary.g, secondary.b, 1.00f);



	//colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 0.58f, 0.88f, 0.00f);
	//colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 0.67f, 0.90f, 1.00f);
	//colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 0.64f, 0.89f, 1.00f);


	//colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.58f, 0.88f, 1.00f);
	//colors[ImGuiCol_PlotLines] = ImVec4(0.64f, 0.00f, 0.44f, 0.63f);
	//colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.67f, 0.90f, 1.00f);
	//colors[ImGuiCol_PlotHistogram] = ImVec4(0.64f, 0.00f, 0.44f, 0.63f);
	//colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.67f, 0.90f, 1.00f);
	//colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	//colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	//colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	//colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	//colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	//colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.55f, 0.87f, 1.00f);
	//colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	//colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	//colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	//colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.61f, 0.90f, 0.73f);
}


void UserPreferences::SaveUserPreferences()
{
	int version = 11;
	rapidjson::Document output;
	output.SetObject();
	auto& alloc = output.GetAllocator();
	rapidjson::Value value;

	value.SetInt(version);
	output.AddMember("Version", value, alloc);
	value.SetInt(static_cast<int>(myTheme));
	output.AddMember("Theme", value, alloc);
	value.SetFloat(myCamMoveSpeed);
	output.AddMember("CamMoveSpeed", value, alloc);
	value.SetFloat(myCamScrollSpeed);
	output.AddMember("CamScrollSpeed", value, alloc);
	value.SetFloat(myCamRotateSpeed);
	output.AddMember("CamTurnSpeed", value, alloc);

	myClearColor.ParseToJsonObject(output, output, "ClearColor");
	mySelectionOutlineColor.ParseToJsonObject(output, output, "OutlineColor");
	myThemeMainColor.ParseToJsonObject(output, output, "MainColor");
	myThemeSecondaryColor.ParseToJsonObject(output, output, "SecondaryColor");
	myTextColor.ParseToJsonObject(output, output, "TextColor");

	value.SetObject();
	rapidjson::Value moveArray(rapidjson::kArrayType);
	moveArray.SetArray();
	moveArray.PushBack(myMoveSnap[0], alloc);
	moveArray.PushBack(myMoveSnap[1], alloc);
	moveArray.PushBack(myMoveSnap[2], alloc);
	value.AddMember("Move", moveArray, alloc);

	rapidjson::Value rotArray(rapidjson::kArrayType);
	rotArray.PushBack(myRotateSnap[0], alloc);
	rotArray.PushBack(myRotateSnap[1], alloc);
	rotArray.PushBack(myRotateSnap[2], alloc);
	value.AddMember("Rotate", rotArray, alloc);

	rapidjson::Value scaleArray(rapidjson::kArrayType);
	scaleArray.PushBack(myScaleSnap[0], alloc);
	scaleArray.PushBack(myScaleSnap[1], alloc);
	scaleArray.PushBack(myScaleSnap[2], alloc);
	value.AddMember("Scale", scaleArray, alloc);

	rapidjson::Value snapToggle;
	snapToggle.SetBool(mySnapToggle);
	value.AddMember("Snap Toggle", snapToggle, alloc);

	output.AddMember("Snap Settings", value, alloc);

	rapidjson::Value debugDrawerToggle;
	debugDrawerToggle.SetBool(myDebugDrawerToggle);
	output.AddMember("Debug Drawer Toggle", debugDrawerToggle, alloc);

	rapidjson::Value windows(rapidjson::kArrayType);
	std::vector<std::shared_ptr<EditorWindow>>& activeWindows = Editor::GetInstance()->GetWindowHandler().GetActiveWindows();
	for (int i = static_cast<int>(activeWindows.size() - 1); i >= 0; i--)
	{
		windows.PushBack(static_cast<int>(activeWindows[i]->GetType()), alloc);
	}
	output.AddMember("Open Windows", windows, alloc);

	value.SetString(rapidjson::StringRef(myDefaultIP.c_str()));
	output.AddMember("IP", value, alloc);
	value.SetString(rapidjson::StringRef(myUsername.c_str()));
	output.AddMember("Username", value, alloc);
	value.SetString(rapidjson::StringRef(myChatColor.c_str()));
	output.AddMember("ChatColor", value, alloc);

	value.SetObject();
	rapidjson::Value debugSettings(rapidjson::kArrayType);
	debugSettings.PushBack(myDebugSettings[0], alloc);
	debugSettings.PushBack(myDebugSettings[1], alloc);
	debugSettings.PushBack(myDebugSettings[2], alloc);
	debugSettings.PushBack(myDebugSettings[3], alloc);
	debugSettings.PushBack(myDebugSettings[4], alloc);
	debugSettings.PushBack(myDebugSettings[5], alloc);
	output.AddMember("DebugDrawerSettings", debugSettings, alloc);

	std::ofstream ofs("EditorSettings.json");
	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	output.Accept(writer);
}

void UserPreferences::SaveTheme()
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	wrapper->CreateDocument();
	auto& doc = wrapper->GetDocument();
	myThemeMainColor.ParseToJsonObject(doc, doc, "Main");
	myThemeSecondaryColor.ParseToJsonObject(doc, doc, "Secondary");
	myTextColor.ParseToJsonObject(doc, doc, "Text");
	std::string path = Catbox::SaveFile("Catbox Theme (*.theme)\0*.theme\0");
	wrapper->SaveDocument(path.c_str());
}

void UserPreferences::LoadTheme()
{
	std::string path = Catbox::OpenFile("Catbox Theme (*.theme)\0*.theme\0");
	if (!path.empty()) 
	{
		auto& settings = RapidJsonWrapper::GetInstance()->LoadDocument(path.c_str());
		myThemeMainColor.LoadFromJson(settings["Main"]);
		myThemeSecondaryColor.LoadFromJson(settings["Secondary"]);
		myTextColor.LoadFromJson(settings["Text"]);
		StyleColorsCustom(myThemeMainColor, myThemeSecondaryColor, myTextColor);
	}
}

void UserPreferences::LoadUserPreferences()
{
	FILE* fp;
	bool error = fopen_s(&fp, "EditorSettings.json", "rb");
	if (error) //no editor settings file found
	{
		ImGui::StyleColorsDark();
		Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::AssetBrowser);
		Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::SceneHierarchy);
		Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::Properties);
		Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::Scene);
		return;
	}

	rapidjson::Document document;
	char readbuffer[65536]{};
	rapidjson::FileReadStream is(fp, readbuffer, sizeof(readbuffer));
	document.ParseStream(is);
	fclose(fp);

	myTheme = (Theme)document["Theme"].GetInt();
	
	if (myTheme == Theme::Night) ImGui::StyleColorsDark();
	else ImGui::StyleColorsLight();
	
	if (document.HasMember("Version"))
	{
		int version = document["Version"].GetInt();
		if (version >= 1)
		{
			myClearColor.LoadFromJson(document["ClearColor"].GetObj());
			Engine::GetInstance()->GetGraphicsEngine()->SetClearColor(myClearColor);
			if (version >= 2)
			{
				myCamMoveSpeed = document["CamMoveSpeed"].GetFloat();
				myCamScrollSpeed = document["CamScrollSpeed"].GetFloat();
				myCamRotateSpeed = document["CamTurnSpeed"].GetFloat();
				Editor::GetInstance()->GetEditorCamera().SetMoveSpeed(myCamMoveSpeed);
				Editor::GetInstance()->GetEditorCamera().SetScrollSpeed(myCamScrollSpeed);
				Editor::GetInstance()->GetEditorCamera().SetRotateSpeed(myCamRotateSpeed);

				if (version >= 3)
				{
					auto snapSettings = document["Snap Settings"].GetObj();
					auto moveSettings = snapSettings["Move"].GetArray();
					auto rotSettings = snapSettings["Rotate"].GetArray();
					auto scaleSettings = snapSettings["Scale"].GetArray();

					for (int i = 0; i < 3; i++)
					{
						myMoveSnap[i] = moveSettings[i].GetFloat();
						myRotateSnap[i] = rotSettings[i].GetFloat();
						myScaleSnap[i] = scaleSettings[i].GetFloat();
					}

					if (version >= 6)
					{
						mySnapToggle = snapSettings["Snap Toggle"].GetBool();
						if (version >= 7)
						{
							myDefaultIP = document["IP"].GetString();
							myUsername = document["Username"].GetString();
							myChatColor = document["ChatColor"].GetString();
						}
						if (version >= 8)
						{
							myDebugDrawerToggle = document["Debug Drawer Toggle"].GetBool();
							Engine::GetInstance()->GetGraphicsEngine()->SetDebugDrawerToggle(myDebugDrawerToggle);

							if (version >= 10)
							{
								myThemeMainColor.LoadFromJson(document["MainColor"].GetObj());
								myThemeSecondaryColor.LoadFromJson(document["SecondaryColor"].GetObj());
								myTextColor.LoadFromJson(document["TextColor"].GetObj());
								if (myTheme == Theme::Custom)
								{
									StyleColorsCustom(myThemeMainColor, myThemeSecondaryColor, myTextColor);
								}
								if (version >= 11) 
								{
									auto debugSettings = document["DebugDrawerSettings"].GetArray();
									for (int i = 0; i < debugSettings.Size(); i++)
									{
										myDebugSettings[i] = debugSettings[i].GetBool();
									}
								}
							}
						}
					}

					Editor::GetInstance()->GetGizmos().SetMoveSnap(myMoveSnap);
					Editor::GetInstance()->GetGizmos().SetRotateSnap(myRotateSnap);
					Editor::GetInstance()->GetGizmos().SetScaleSnap(myScaleSnap);
					Editor::GetInstance()->GetGizmos().SetSnapToggle(mySnapToggle);

					if (document.HasMember("Open Windows"))
					{
						auto windows = document["Open Windows"].GetArray();
						for (auto& wnd : windows)
						{
							Editor::GetInstance()->GetWindowHandler().OpenWindow(static_cast<WindowType>(wnd.GetInt()));
						}
					}
					else
					{
						Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::AssetBrowser);
						Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::SceneHierarchy);
						Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::Properties);
						Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::Scene);
					}

					if (version >= 5)
					{
						mySelectionOutlineColor.LoadFromJson(document["OutlineColor"].GetObj());

					}
				}
			}
		}
	}
}
