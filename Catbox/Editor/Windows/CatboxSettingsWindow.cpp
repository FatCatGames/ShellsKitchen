#include "stdafx.h"
#include "CatboxSettingsWindow.h"

#include "ProjectSettings.h"

CatboxSettingsWindow::CatboxSettingsWindow()
{
	myPrefs = &Editor::GetInstance()->GetUserPrefs();
}


void CatboxSettingsWindow::Render()
{
	if (!BeginWindow("Catbox Settings", true)) return;
	

	int themeIndex = (int)myPrefs->myTheme;
	const char* items[] = { "Catbox Night", "Catbox Light", "Custom"};
	if (ImGui::Combo("Theme", &themeIndex, items, IM_ARRAYSIZE(items)))
	{
		myPrefs->myTheme = (UserPreferences::Theme)themeIndex;
		if (myPrefs->myTheme == UserPreferences::Theme::Night) ImGui::StyleColorsDark();
		else if (myPrefs->myTheme == UserPreferences::Theme::Light) ImGui::StyleColorsLight();
		else myPrefs->StyleColorsCustom(myPrefs->myThemeMainColor, myPrefs->myThemeSecondaryColor, myPrefs->myTextColor);
	}

	if (myPrefs->myTheme == UserPreferences::Theme::Custom) 
	{
		if (ImGui::Button("Save theme"))
		{
			myPrefs->SaveTheme();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load theme"))
		{
			myPrefs->LoadTheme();
		}

		float col[3] = { myPrefs->myThemeMainColor.r, myPrefs->myThemeMainColor.g, myPrefs->myThemeMainColor.b };
		if (ImGui::ColorEdit3("Main Color", col))
		{
			myPrefs->myThemeMainColor = Color(col[0], col[1], col[2], 1);
			myPrefs->StyleColorsCustom(myPrefs->myThemeMainColor, myPrefs->myThemeSecondaryColor, myPrefs->myTextColor);
		}
		if (ImGui::IsItemDeactivatedAfterEdit()) myPrefs->SaveUserPreferences();

		float col2[3] = { myPrefs->myThemeSecondaryColor.r, myPrefs->myThemeSecondaryColor.g, myPrefs->myThemeSecondaryColor.b };
		if (ImGui::ColorEdit3("Secondary Color", col2))
		{
			myPrefs->myThemeSecondaryColor = Color(col2[0], col2[1], col2[2], 1);
			myPrefs->StyleColorsCustom(myPrefs->myThemeMainColor, myPrefs->myThemeSecondaryColor, myPrefs->myTextColor);
		}
		if (ImGui::IsItemDeactivatedAfterEdit()) myPrefs->SaveUserPreferences();

		float textCol[3] = { myPrefs->myTextColor.r, myPrefs->myTextColor.g, myPrefs->myTextColor.b };
		if (ImGui::ColorEdit3("Text Color", textCol))
		{
			myPrefs->myTextColor = Color(textCol[0], textCol[1], textCol[2], 1);
			myPrefs->StyleColorsCustom(myPrefs->myThemeMainColor, myPrefs->myThemeSecondaryColor, myPrefs->myTextColor);
		}
		if (ImGui::IsItemDeactivatedAfterEdit()) myPrefs->SaveUserPreferences();
	}


	float clearColor[3] = { myPrefs->myClearColor.r, myPrefs->myClearColor.g, myPrefs->myClearColor.b };
	if (ImGui::ColorEdit3("Clear color", clearColor))
	{
		myPrefs->myClearColor.r = clearColor[0];
		myPrefs->myClearColor.g = clearColor[1];
		myPrefs->myClearColor.b = clearColor[2];
		Engine::GetInstance()->GetGraphicsEngine()->SetClearColor(myPrefs->myClearColor);
	}
	if (ImGui::IsItemDeactivatedAfterEdit()) myPrefs->SaveUserPreferences();

	float outlineColor[3] = { myPrefs->mySelectionOutlineColor.r, myPrefs->mySelectionOutlineColor.g, myPrefs->mySelectionOutlineColor.b };
	if (ImGui::ColorEdit3("Selection outline color", outlineColor))
	{
		myPrefs->mySelectionOutlineColor.r = outlineColor[0];
		myPrefs->mySelectionOutlineColor.g = outlineColor[1];
		myPrefs->mySelectionOutlineColor.b = outlineColor[2];
	}
	if (ImGui::IsItemDeactivatedAfterEdit()) myPrefs->SaveUserPreferences();

	ImGui::Spacing();
	ImGui::Text("Camera settings");

	ImGui::SliderFloat("Move speed", &myPrefs->myCamMoveSpeed, 0, 100, "%.1f");
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		myPrefs->SaveUserPreferences();
		Editor::GetInstance()->GetEditorCamera().SetMoveSpeed(myPrefs->myCamMoveSpeed);
	}

	ImGui::SliderFloat("Scroll speed", &myPrefs->myCamScrollSpeed, 0, 100, "%.1f");
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		myPrefs->SaveUserPreferences();
		Editor::GetInstance()->GetEditorCamera().SetScrollSpeed(myPrefs->myCamScrollSpeed);
	}

	ImGui::SliderFloat("Turn speed", &myPrefs->myCamRotateSpeed, 0, 100, "%.1f");
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		myPrefs->SaveUserPreferences();
		Editor::GetInstance()->GetEditorCamera().SetRotateSpeed(myPrefs->myCamRotateSpeed);
	}

	ImGui::Spacing();
	ImGui::Text("Grid snap settings");


	ImGui::InputFloat3("Move", myPrefs->myMoveSnap);
	if (ImGui::IsItemDeactivatedAfterEdit()) 
	{
		myPrefs->SaveUserPreferences();
		Editor::GetInstance()->GetGizmos().SetMoveSnap(myPrefs->myMoveSnap);
	}
	ImGui::InputFloat3("Rotate", myPrefs->myRotateSnap);
	if (ImGui::IsItemDeactivatedAfterEdit()) 
	{
		myPrefs->SaveUserPreferences();
		Editor::GetInstance()->GetGizmos().SetRotateSnap(myPrefs->myRotateSnap);
	}
	ImGui::InputFloat3("Scale", myPrefs->myScaleSnap);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		myPrefs->SaveUserPreferences();
		Editor::GetInstance()->GetGizmos().SetScaleSnap(myPrefs->myScaleSnap);
	}

	bool gammaCorrectionEnabled = Engine::GetInstance()->GetGraphicsEngine()->GetGammaCorrectionEnabled();
	if (ImGui::Checkbox("Gamma correction", &gammaCorrectionEnabled))
	{
		Engine::GetInstance()->GetGraphicsEngine()->SetGammaCorrectionEnabled(gammaCorrectionEnabled);
		ProjectSettings::SaveProjectSettings();
	}

	EndWindow();
}