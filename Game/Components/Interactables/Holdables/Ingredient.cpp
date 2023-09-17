#include "Game.pch.h"
#include "Ingredient.h"
#include "Components\3D\ModelInstance.h"
#include "Assets\Material.h"
#include "Assets\Model.h"
#include "Audio\AudioManager.h"
#include "Components\ParticleSystem.h"

void Ingredient::Update()
{
	HoldableItem::Update();

	if (!myTransform->GetParent()) 
	{
		//Respawn
		if (myTransform->worldPos().y < -5.f && !myHasSplashed)
		{
			auto splash = InstantiatePrefab("WaterSplashVFXsmaller");
			splash->GetTransform()->SetWorldPos((myTransform->worldPos()));
			auto bubbles = InstantiatePrefab("BubblesSpawner");
			myHasSplashed = true;
			bubbles->GetTransform()->SetWorldPos((myTransform->worldPos()));
			bubbles->GetTransform()->SetParent(myTransform);
		}
		else if (myTransform->worldPos().y < -6.f)
		{
			SetIconActive(false);
		}
		else if (myTransform->worldPos().y < -10.f)
		{
			myGameObject->Destroy();
		}
	}
}

void Ingredient::Awake()
{
	HoldableItem::Awake();
	myFoodTrail = myGameObject->GetComponent<ParticleSystem>();
	myInteractPriority = 2;
}

void Ingredient::RenderInProperties(std::vector<Component*>& aComponentList)
{
	vector<string> ingredientNamesStr(IngredientType::COUNT);
	vector<const char*> ingredientNames(IngredientType::COUNT);
	for (size_t i = 0; i < IngredientType::COUNT; i++)
	{
		std::string ingredientName = IngredientTypeToName(static_cast<IngredientType>(i));
		ingredientNamesStr[i] = ingredientName;
		ingredientNames[i] = ingredientNamesStr[i].c_str();
	}

	int ingredientIndex = static_cast<int>(myType);
	if (ImGui::Combo("Ingredient", &ingredientIndex, ingredientNames.data(), static_cast<int>(IngredientType::COUNT)))
	{
		myType = static_cast<IngredientType>(ingredientIndex);
	}

	Catbox::Checkbox("Boilable", &myCanBeBoiled);
	Catbox::Checkbox("Friable", &myCanBeFried);
	Catbox::Checkbox("Choppable", &myCanBeChopped);

	//Catbox::DragFloat("Cook time", &myTargetCookTime);
	//Catbox::DragFloat("Cut time", &myTargetCutTime);
}


void Ingredient::Save(rapidjson::Value& /*aComponentData*/)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();
	wrapper.SaveValue<DataType::Int>("Type", myType);
	wrapper.SaveValue<DataType::Bool>("Boilable", myCanBeBoiled);
	wrapper.SaveValue<DataType::Bool>("Friable", myCanBeFried);
	wrapper.SaveValue<DataType::Bool>("Choppable", myCanBeChopped);
	//wrapper.SaveValue<DataType::Float>("CookTime", myTargetCookTime);
	//wrapper.SaveValue<DataType::Float>("CutTime", myTargetCutTime);
}

void Ingredient::Load(rapidjson::Value& aComponentData)
{
	myType = static_cast<IngredientType>(aComponentData["Type"].GetInt());

	if (!aComponentData.HasMember("Boilable")) return;
	myCanBeBoiled = aComponentData["Boilable"].GetBool();
	myCanBeFried = aComponentData["Friable"].GetBool();
	myCanBeChopped = aComponentData["Choppable"].GetBool();
	/*if (!aComponentData.HasMember("CookTime")) return;
	myTargetCookTime = aComponentData["CookTime"].GetFloat();
	myTargetCutTime = aComponentData["CutTime"].GetFloat();*/
}


bool Ingredient::Cook()
{
	if (myIsBurnt) return false;

	myCookTimer += deltaTime;
	if (!myIsCooked)
	{
		if (myCookTimer >= myTargetCookTime)
		{
			SetIconActive(true);
			auto modelInstance = myGameObject->GetComponent<ModelInstance>();
			std::shared_ptr<Material> mat;
			if (myIsBoiled)
			{
				mat = AssetRegistry::GetInstance()->GetAsset<Material>(IngredientTypeToName(myType) + "_fried_mat");
			}
			else mat = AssetRegistry::GetInstance()->GetAsset<Material>(IngredientTypeToName(myType) + "_cooked_mat");
			if (mat) 
			{
				modelInstance->SetMaterial(mat, 0);
				CreateInteractableShader();
			}

			myIsCooked = true;
			return true;
		}
	}
	else
	{
		myBurnTimer = myCookTimer - myTargetCookTime;
		if (myCookTimer >= myTargetCookTime * 2)
		{
			myIsBurnt = true;
			auto model = myGameObject->GetComponent<ModelInstance>();
			if (model)
			{
				auto burntFoodMat = AssetRegistry::GetInstance()->GetAsset<Material>("Burnt_Food_mat");
				if (burntFoodMat)
				{
					model->SetMaterial(burntFoodMat, 0);
					CreateInteractableShader();
				}
			}

			auto explosion = InstantiatePrefab("FoodExplosion");
			explosion->GetTransform()->SetWorldPos(myTransform->worldPos());

			Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_FOOD_POOF));

			return true;
		}
	}

	return false;
}

bool Ingredient::Boil()
{
	if (myIsBurnt) return false;

	myBoilTimer += deltaTime;


	if (!myIsBoiled)
	{
		if (myBoilTimer >= myTargetBoilTime)
		{
			SetIconActive(true);
			auto modelInstance = myGameObject->GetComponent<ModelInstance>();
			auto mat = AssetRegistry::GetInstance()->GetAsset<Material>(IngredientTypeToName(myType) + "_cooked_mat");
			if (mat)
			{
				modelInstance->SetMaterial(mat, 0);
				CreateInteractableShader();
			}

			myIsBoiled = true;
			return true;
		}
	}
	else
	{
		myBurnTimer = myBoilTimer - myTargetBoilTime;
		if (myBoilTimer >= myTargetBoilTime * 2)
		{
			myIsBurnt = true;
			auto model = myGameObject->GetComponent<ModelInstance>();
			if (model)
			{
				auto burntFoodMat = AssetRegistry::GetInstance()->GetAsset<Material>("Burnt_Food_mat");
				if (burntFoodMat)
				{
					model->SetMaterial(burntFoodMat, 0);
					CreateInteractableShader();
				}
			}

			auto explosion = InstantiatePrefab("FoodExplosion");
			explosion->GetTransform()->SetWorldPos(myTransform->worldPos());

			Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_FOOD_POOF));

			return true;
		}
	}

	return false;
}

bool Ingredient::Cut()
{
	myCutTimer += deltaTime;
	if (!myIsCut && myCutTimer >= myTargetCutTime)
	{
		myIsCut = true;

		SetIconActive(true); // Activate ingredient icon

		std::string ingredientName = IngredientTypeToName(myType);
		std::shared_ptr<Model> choppedMesh = AssetRegistry::GetInstance()->GetAsset<Model>(ingredientName + "_chopped_mesh");
		if (choppedMesh)
		{
			auto modelInstance = myGameObject->GetComponent<ModelInstance>();
			modelInstance->SetModel(choppedMesh);
			auto mat = AssetRegistry::GetInstance()->GetAsset<Material>(ingredientName + "_chopped_mat");
			modelInstance->SetMaterial(mat, 0);
			CreateInteractableShader();
		}
		else
		{
			Vector3f scale = myTransform->worldScale();
			scale.x *= 1.3f;
			scale.y *= 0.5f;
			scale.z *= 1.3f;
			myTransform->SetWorldScale(scale);
		}

		return true;
	}
	return false;
}

bool Ingredient::CanBeFried()
{
	if (myCanBeBoiled && myCanBeFried) return myIsBoiled && !myIsBurnt;
	return myCanBeFried;
}

bool Ingredient::GetIsUnprocessed()
{
	if (myCanBeBoiled) return !myIsBoiled;
	if (myCanBeChopped)
	{
		if (myIsCooked) return false;
		return !myIsCut;
	}
	if (myCanBeFried)
	{
		if (myIsCut) return false;
		return !myIsCooked;
	}

	return false;
}

void Ingredient::SetBoilingMesh()
{
	auto modelInstance = myGameObject->GetComponent<ModelInstance>();
	auto cookingModel = AssetRegistry::GetInstance()->GetAsset<Model>(IngredientTypeToName(myType) + "_boiling_mesh");
	if (cookingModel)
	{
		modelInstance->SetModel(cookingModel);
	}

	auto mat = AssetRegistry::GetInstance()->GetAsset<Material>(IngredientTypeToName(myType) + "_cooked_mat");
	if (mat)
	{
		modelInstance->SetMaterial(mat, 0);
		CreateInteractableShader();
	}

	myTransform->Translate(Vector3f::down() * 0.2f);
}

void Ingredient::SetCookedMesh()
{
	auto modelInstance = myGameObject->GetComponent<ModelInstance>();
	auto cookingModel = AssetRegistry::GetInstance()->GetAsset<Model>(IngredientTypeToName(myType) + "_cooked_mesh");
	if (cookingModel)
	{
		modelInstance->SetModel(cookingModel);
	}
}

void Ingredient::SetIconActive(bool aFlag)
{
	myGameObject->GetTransform()->GetChildren()[0]->GetGameObject()->SetActive(aFlag);
}

void Ingredient::HideMesh()
{
	auto modelInstance = myGameObject->GetComponent<ModelInstance>();
	modelInstance->SetEnabled(false);
}

float Ingredient::GetCookPercent()
{
	if (myCanBeBoiled && !myIsBoiled) return myBoilTimer / myTargetBoilTime;
	return myCookTimer / myTargetCookTime;
}

bool Ingredient::GetIsHeld()
{
	return myIsBeingHeld;
}

void Ingredient::SetType(const IngredientType& aType)
{
	myType = aType;
}

void Ingredient::SetFoodTrailState(bool aShouldPlay)
{
	if (!myFoodTrail) return;
	if (myTrailIsPlaying == aShouldPlay) return;
	myTrailIsPlaying = aShouldPlay;

	if (aShouldPlay)
	{
		myFoodTrail->Replay();
	}
	else myFoodTrail->Pause();
}

void Ingredient::ResetSplash()
{
	myHasSplashed = false;
}


