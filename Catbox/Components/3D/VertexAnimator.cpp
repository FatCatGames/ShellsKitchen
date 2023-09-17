#include "stdafx.h"
#include "VertexAnimator.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Material.h"
#include "TimeManager.h"

void VertexAnimator::AddVAT(const int aId, std::shared_ptr<Texture> aTexture, const std::string& aName, bool aFlag)
{
	VAT newVat;
	newVat.loop = aFlag;
	newVat.name = aName;
	newVat.texture = aTexture;

	myVATS.insert({ aId, newVat });
}

void VertexAnimator::StopVAT()
{
	myIsPlaying = false;
	myGameObject->SetAliveTime(0);
}

VertexAnimator::~VertexAnimator()
{
	for (int i = 0; i < myTimerIds.size(); i++)
	{
		TimeManager::RemoveTimer(myTimerIds[i]);
	}
}

void VertexAnimator::Init()
{
	PlayVAT(0);
}

void VertexAnimator::RenderInProperties(std::vector<Component*>& aComponentList)
{
	for (auto& vat : myVATS)
	{
		if (ImGui::CollapsingHeader(std::to_string(vat.first).c_str(), ImGuiTreeNodeFlags_FramePadding))
		{
			ImGui::Indent(20);
			ImGui::InputText(("Name ##" + std::to_string(vat.first)).c_str(), &vat.second.name);
			ImGui::Text(("ID ##" + std::to_string(vat.first)).c_str());
			bool edited = false;
			auto texture = AssetRegistry::GetInstance()->AssetDropdownMenu<Texture>(vat.second.texture.get(), "Texture ##" + vat.second.name, edited);
			if (texture) 
			{
				vat.second.texture = texture;
			}
			ImGui::Unindent(20);
		}
		if (ImGui::Button(("Play ##" + std::to_string(vat.first)).c_str())) 
		{
			PlayVAT(vat.first);
		}
	}
	ImGui::InputInt("Current ID", &myCurrentVATSId);
	ImGui::SameLine();
	if (ImGui::Button("Add VAT")) 
	{
		AddVAT(myCurrentVATSId, AssetRegistry::GetInstance()->GetAsset<Texture>("T_Default_C"), "UnDefined");
		PlayVAT(myCurrentVATSId);
	}
	if (ImGui::Button("Stop"))
	{
		StopVAT();
	}
}

void VertexAnimator::Awake()
{
	PlayVAT(0);
}

void VertexAnimator::Save(rapidjson::Value& aComponentData)
{
	int version = 1;
	auto wrapper = RapidJsonWrapper::GetInstance();
	auto alloc = wrapper->GetDocument().GetAllocator();
	wrapper->SaveValue<DataType::Int, int>("Version", version);
	auto vatArray = wrapper->CreateArray();
	vatArray.SetArray();

	for (auto& vat : myVATS)
	{
		rapidjson::Value object;
		object.SetObject();
		int id = vat.first;
		wrapper->SetTarget(object);
		wrapper->SaveValue<DataType::Int, int>("ID", id);
		wrapper->SaveString("Name", vat.second.name.c_str());
		wrapper->SaveString("TextureName", vat.second.texture->GetName().c_str());
		wrapper->SaveValue<DataType::Bool, bool>("ShouldLoop", vat.second.loop);
		vatArray.PushBack(object, alloc);
	}
	aComponentData.AddMember("VATS", vatArray, alloc);
	wrapper->SetTarget(aComponentData);
}

void VertexAnimator::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Version")) 
	{
		int version = aComponentData["Version"].GetInt();
		if (version >= 1)
		{
			auto vatArray = aComponentData["VATS"].GetArray();
			for (int i = 0; i < vatArray.Size(); i++)
			{
				VAT newVat;

				int id = vatArray[i]["ID"].GetInt();

				std::string name = vatArray[i]["Name"].GetString();
				std::string textureName = vatArray[i]["TextureName"].GetString();

				auto texture = AssetRegistry::GetInstance()->GetAsset<Texture>(textureName);
				if (!texture) 
				{
					newVat.texture = AssetRegistry::GetInstance()->GetAsset<Texture>("T_Default_C");
				}
				else 
				{
					newVat.texture = texture;
				}
				newVat.loop = vatArray[i]["ShouldLoop"].GetBool();

				myVATS.insert({ id, newVat });
			}
		}
	}
}

void VertexAnimator::Update()
{
	if (!myIsPlaying) 
	{
		StopVAT();
	}
}

void VertexAnimator::RunInEditor() 
{
	if (!PLAYMODE) 
	{
		if (myCurrentVATSId == -1) 
		{
			PlayVAT(0);
			StopVAT();
		}
		Update();
	}
}

void VertexAnimator::PlayVAT(const int aId, float aDelay)
{
	if (myVATS.find(aId) == myVATS.end()) 
	{
		return;
	}

	auto modelInstance = myGameObject->GetComponent<ModelInstance>();
	if (modelInstance) 
	{
		myGameObject->SetAliveTime(0);
		myIsPlaying = true;
		auto& materials = modelInstance->GetMaterials();
		myCurrentVATSId = aId;

		if (aDelay > 0)
		{
			TimerPayload aPayLoad;
			aPayLoad.aPayLoadFuction = [&] { PlayVAT(myCurrentVATSId); };
			aPayLoad.myTime = aDelay;
			myTimerIds.push_back(TimeManager::AddTimer(aPayLoad));
		}
		else 
		{
			for (int i = 0; i < materials.size(); i++)
			{
				materials[i]->SetCustomTexture(11, myVATS[aId].texture);
			}
		}
	}
}