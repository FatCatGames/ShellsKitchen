#include "stdafx.h"
#include "Light.h"
#include "ComponentTools\SceneManager.h"
#include "Assets/Texture.h"
#include "Graphics\Rendering\DX11\DX11.h"
#include "Graphics\Rendering\DepthStencil.h"


void Light::Update()
{
	if (myData.castShadows) CastShadows();
}

void Light::RunInEditor()
{
	if (myData.castShadows) CastShadows();
	if (myGameObject->IsSelected(0))
	{
		if (myData.type == 0) 
		{
			const float aspectRatio = 1.0f;
			const float orthoSize = 41.0f; // Adjust this value based on your desired orthographic size
			const float farPlane = 1000.0f;
			const float halfWidth = orthoSize * aspectRatio;
			const float halfHeight = orthoSize;

			Vector3f forward = myTransform->forward();
			Vector3f right = myTransform->right();
			Vector3f up = myTransform->up();
			Vector3f center = myTransform->worldPos();

			Vector3f farBottomLeftV3 = center - right * halfWidth + up * halfHeight + forward * farPlane;
			Vector3f farTopLeftV3 = center - right * halfWidth - up * halfHeight + forward * farPlane;
			Vector3f farTopRightV3 = center + right * halfWidth - up * halfHeight + forward * farPlane;
			Vector3f farBottomRightV3 = center + right * halfWidth + up * halfHeight + forward * farPlane;

			Vector3f nearBottomLeftV3 = center - right * halfWidth + up * halfHeight;
			Vector3f nearTopLeftV3 = center - right * halfWidth - up * halfHeight;
			Vector3f nearTopRightV3 = center + right * halfWidth - up * halfHeight;
			Vector3f nearBottomRightV3 = center + right * halfWidth + up * halfHeight;

			DebugDrawer::DrawLine(nearBottomLeftV3, farBottomLeftV3);
			DebugDrawer::DrawLine(nearTopLeftV3, farTopLeftV3);
			DebugDrawer::DrawLine(nearTopRightV3, farTopRightV3);
			DebugDrawer::DrawLine(nearBottomRightV3, farBottomRightV3);

			DebugDrawer::DrawLine(nearBottomLeftV3, nearBottomRightV3);
			DebugDrawer::DrawLine(nearBottomRightV3, nearTopRightV3);
			DebugDrawer::DrawLine(nearTopRightV3, nearTopLeftV3);
			DebugDrawer::DrawLine(nearTopLeftV3, nearBottomLeftV3);

			DebugDrawer::DrawLine(farBottomLeftV3, farBottomRightV3);
			DebugDrawer::DrawLine(farBottomRightV3, farTopRightV3);
			DebugDrawer::DrawLine(farTopRightV3, farTopLeftV3);
			DebugDrawer::DrawLine(farTopLeftV3, farBottomLeftV3);
		}
		else 
		{
			DebugDrawer::DrawLine(myData.pos, myData.pos + myData.forward * 20.f, Color::Yellow());
			DebugDrawer::DrawBillboard(myData.pos, 1, AssetRegistry::GetInstance()->GetAsset<Texture>("T_Default_C"));
		}
	}
}

void Light::CreateShadowMap()
{
	myShadowMap = std::make_shared<DepthStencil>();
	myShadowMap->CreateDepthStencil(res.x, res.y);
}

void Light::CastShadows()
{
	Engine::GetInstance()->GetGraphicsEngine()->AddToRenderQueue(this);
}

void Light::OnTransformChanged()
{
	myData.pos = myTransform->worldPos();
	myData.forward = myTransform->forward().GetNormalized();
	myData.view = Catbox::Matrix4x4<float>::GetFastInverse(myTransform->GetWorldTransformMatrix());
}


void Light::Init()
{
	if (Engine::GetInstance()->GetSceneManager()->GetCurrentScene() && myGameObject->GetObjectInstanceID() != -1)
	{
		Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->AddLight(this);
	}
	myData.pos = myTransform->worldPos();
	myData.forward = myTransform->forward();
	myTexture = AssetRegistry::GetInstance()->GetAsset<Texture>("studio_cubemap");
	myData.isActive = myIsEnabled && myGameObject->IsActive();

	constexpr float nearPlane = 0.1f;
	constexpr float farPlane = 1000.0f;

	myData.nearPlane = nearPlane;
	myData.farPlane = farPlane;
	myData.view = Catbox::Matrix4x4<float>::GetFastInverse(myTransform->GetWorldTransformMatrix());

	Catbox::Matrix4x4<float> projection;
	float orthoSize = res.x * 0.5f; // half-size of projection volume in world units
	float aspectRatio = 1.0f; // aspect ratio of viewport

	float left = -orthoSize * aspectRatio;
	float right = orthoSize * aspectRatio;
	float bottom = -orthoSize;
	float top = orthoSize;

	projection(1, 1) = 60.0f / (right - left);
	projection(2, 2) = 60.0f / (top - bottom);
	projection(3, 3) = 1.0f / (farPlane - nearPlane);
	projection(4, 1) = -(right + left) / (right - left);
	projection(4, 2) = -(top + bottom) / (top - bottom);
	projection(4, 3) = -nearPlane / (farPlane - nearPlane);
	projection(4, 4) = 1.0f;

	myData.projection = projection.ToColumnMajor(projection);
	myData.forward = myTransform->forward().GetNormalized();


	myData.castShadows = false;
	if (myData.castShadows)
	{
		myShadowMap = std::make_shared<DepthStencil>();
		myShadowMap->CreateDepthStencil(res.x, res.y);
	}
}

void Light::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto lights = ComponentVectorCast<Light>(aComponentList);
	bool edit = false;
	bool sameType = true;
	int lightTypeIndex = myData.type;
	for (auto& light : lights)
	{
		if (light->myData.type != lightTypeIndex)
		{
			lightTypeIndex = -1;
			sameType = false;
			break;
		}
	}

	const char* items[] = { "Directional", "Point", "Spot", "Environment" };
	ImGui::SetNextItemWidth(115);
	if (ImGui::Combo("Type", &lightTypeIndex, items, IM_ARRAYSIZE(items)))
	{
		for (auto& light : lights)
		{
			light->SetType(static_cast<LightType>(lightTypeIndex));
		}
	}

	bool castShadows = myData.castShadows;
	if (Catbox::Checkbox("Cast shadows", &castShadows))
	{
		for (auto& light : lights)
		{
			light->myData.castShadows = castShadows;
			if (castShadows)
			{
				light->CreateShadowMap();
			}
			else 
			{
				light->myShadowMap = nullptr;
			}
		}
	}

	if (lightTypeIndex == 3)
	{
		std::string texture = "Texture";
		auto choice = AssetRegistry::GetInstance()->AssetDropdownMenu<Texture>(myTexture.get(), texture, edit);
		if (edit)
		{
			for (auto& light : lights)
			{
				light->myTexture = choice;
			}
		}
	}


	float col[3] = { myData.color.r, myData.color.g, myData.color.b };
	if (ImGui::ColorEdit3("Color", col, ImGuiColorEditFlags_NoInputs))
	{
		for (auto& light : lights)
		{
			light->myData.color = { col[0], col[1], col[2], 1 };
		}
	}

	ImGui::SameLine();
	ImGui::SetNextItemWidth(50);
	float intensity = myData.intensity;

	if (Catbox::DragFloat("Intensity", &intensity, 0.01f, 0, 100, "%.3g"))
	{
		for (auto& light : lights)
		{
			light->myData.intensity = intensity;
		}
	}

	if (sameType)
	{
		if (myData.type == (int)LightType::Point)
		{
			ImGui::SetNextItemWidth(50);
			float range = myData.maxRange;
			if (Catbox::DragFloat("Range", &range, 0.05f, 0.01f, 1000, "%.3g"))
			{
				for (auto& light : lights)
				{
					light->myData.maxRange = range;
				}
			}
		}
		else if (myData.type == (int)LightType::Spot)
		{
			float innerRadius = myData.innerRadius;
			if (Catbox::DragFloat("Inner radius", &innerRadius, 0.05f, 0, 1000, "%.3g"))
			{
				for (auto& light : lights)
				{
					light->myData.innerRadius = innerRadius;
				}
			}

			float outerRadius = myData.outerRadius;
			if (Catbox::DragFloat("Outer radius", &outerRadius, 0.05f, 0, 1000, "%.3g"))
			{
				for (auto& light : lights)
				{
					light->myData.outerRadius = outerRadius;
				}
			}
		}
	}
}

void Light::SetType(LightType aType)
{
	Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->ChangeLightType(this, static_cast<int>(aType));
	if (aType == Light::LightType::Directional)
	{
		myData.castShadows = true;
		if (!myShadowMap)
		{
			myShadowMap = std::make_shared<DepthStencil>();
			myShadowMap->CreateDepthStencil(res.x, res.y);
		}
	}
	else myData.castShadows = false;
}

void Light::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	int version = 2;
	wrapper.SaveValue<DataType::Int>("Version", version);

	wrapper.SaveValue<DataType::Int>("Type", myData.type);
	wrapper.SaveValue<DataType::Int>("Shadows", myData.castShadows);
	wrapper.SaveValue<DataType::Float>("Intensity", myData.intensity);
	wrapper.SaveValue<DataType::Float>("Range", myData.maxRange);
	wrapper.SaveString("Tex", myTexture ? myTexture->GetName().c_str() : "");
	myData.color.ParseToJsonObject(aComponentData, wrapper.GetDocument(), "Color");
}

void Light::OnDestroy()
{
	Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->RemoveLight(this);
}

void Light::Load(rapidjson::Value& aComponentData)
{
	int version = aComponentData["Version"].GetInt();

	if (myGameObject->GetObjectInstanceID() != -1 && Engine::GetInstance()->GetSceneManager()->GetCurrentScene())
	{
		SetType(LightType(aComponentData["Type"].GetInt()));
	}
	else myData.type = aComponentData["Type"].GetInt();
	myData.intensity = aComponentData["Intensity"].GetFloat();
	myData.maxRange = aComponentData["Range"].GetFloat();
	myData.color.LoadFromJson(aComponentData["Color"]);

	if (version >= 1)
	{
		if (myData.type == 3)
		{
			std::string tex = aComponentData["Tex"].GetString();
			if (!tex.empty())
			{
				myTexture = AssetRegistry::GetInstance()->GetAsset<Texture>(tex);
			}
		}

		if (version >= 2)
		{
			myData.castShadows = aComponentData["Shadows"].GetInt();
			if (myData.castShadows) CreateShadowMap();
		}
	}
}
