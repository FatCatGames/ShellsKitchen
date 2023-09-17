#include "Game.pch.h"
#include "LevelSelectPreview.h"
#include "Components\Physics\Collisions\Collider.h"
#include "Components\Physics\RigidBody.h"
#include "..\Catbox\Components\UI\UISprite.h"
#include "Components\Camera\RayPickerComponent.h"

void LevelSelectPreview::Awake()
{
	myBronzeStar = myGameObject->GetTransform()->GetChildren()[1]->GetGameObject()->GetComponent<UISprite>();
	mySilverStar = myGameObject->GetTransform()->GetChildren()[2]->GetGameObject()->GetComponent<UISprite>();
	myGoldStar = myGameObject->GetTransform()->GetChildren()[3]->GetGameObject()->GetComponent<UISprite>();
	myCamera = GameManager::GetInstance()->GetRayPicker()->GetGameObject().GetComponent<Camera>();

	myZerothChild = myGameObject->GetTransform()->GetChildren()[0];
	myFirstChild = myGameObject->GetTransform()->GetChildren()[1];
	mySecondChild = myGameObject->GetTransform()->GetChildren()[2];
	myThirdChild = myGameObject->GetTransform()->GetChildren()[3];
	myFourthChild = myGameObject->GetTransform()->GetChildren()[4];
	myFifthChild = myGameObject->GetTransform()->GetChildren()[5];
	mySixthChild = myGameObject->GetTransform()->GetChildren()[6];

	myParentPos = myGameObject->GetTransform()->worldPos();
	myRayPickerComponent = GameManager::GetInstance()->GetRayPicker();
}

void LevelSelectPreview::Update()
{
	Catbox::Matrix4x4<float> projection = myCamera->GetProjectionMatrix();
	Catbox::Matrix4x4<float> viewMatrix = Catbox::Matrix4x4<float>::GetFastInverse(myRayPickerComponent->GetTransform()->GetWorldTransformMatrix());

	Vector3f parentPos = myParentPos;
	print(std::to_string(myParentPos.x) + " : " + std::to_string(myParentPos.y) + " : " + std::to_string(myParentPos.z));

	Catbox::Vector4<float> parentPosWorld = { parentPos.x, 1, parentPos.z , 1 };

	parentPosWorld = parentPosWorld * viewMatrix;
	parentPosWorld = parentPosWorld * projection;
	parentPosWorld = parentPosWorld / parentPosWorld.w;

	parentPos.z = 1.0f;

	parentPos = { parentPosWorld.x, parentPosWorld.y, parentPosWorld.z };
	myZerothChild->SetWorldPos(parentPos + myGeneralOffset);

	myFirstChild->SetWorldPos(parentPos + myStarOneOffset + myGeneralOffset);
	mySecondChild->SetWorldPos(parentPos + myStarTwoOffset + myGeneralOffset);
	myThirdChild->SetWorldPos(parentPos + myStarThreeOffset + myGeneralOffset);

	myFourthChild->SetWorldPos(parentPos + myScoreOneOffset + myGeneralOffset);
	myFifthChild->SetWorldPos(parentPos + myScoreTwoOffset + myGeneralOffset);
	mySixthChild->SetWorldPos(parentPos + myScoreThreeOffset + myGeneralOffset);
}

void LevelSelectPreview::RenderInProperties(std::vector<Component*>& aComponentList)
{
	ImGui::InputFloat("xSize", &myXScale);
	ImGui::InputFloat("ySize", &myYScale);
	ImGui::InputFloat("yOffset", &myYOffset);
}

void LevelSelectPreview::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Float>("xScale", myXScale);
	wrapper.SaveValue<DataType::Float>("yScale", myYScale);
	wrapper.SaveValue<DataType::Float>("yOffset", myYOffset);
}

void LevelSelectPreview::Load(rapidjson::Value& aComponentData)
{
	if(aComponentData.HasMember("xScale"))
	{
		myXScale = aComponentData["xScale"].GetFloat();
	}
	if(aComponentData.HasMember("yScale"))
	{
		myYScale = aComponentData["yScale"].GetFloat();
	}
	if(aComponentData.HasMember("yOffset"))
	{
		myYOffset = aComponentData["yOffset"].GetFloat();
	}
}

void LevelSelectPreview::SetValues(const bool& aBronze, const bool& aSilver, const bool& aGold, const int& bronzeLevel, const int& silverLevel, const int& goldLevel)
{
	myHasBronze = aBronze;
	myHasSilver = aSilver;
	myHasGold = aGold;

	myBronzeLevel = bronzeLevel;
	mySilverLevel = silverLevel;
	myGoldLevel = goldLevel;

	SetCounterValues(bronzeLevel, silverLevel, goldLevel);
	SetStarSprites(aBronze, aSilver, aGold);
}

void LevelSelectPreview::SetStarSprites(const bool& aBronze, const bool& aSilver, const bool& aGold)
{
	if(aBronze)
	{
		myBronzeStar->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("StarFilled"));
	}
	else
	{
		myBronzeStar->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("StarUnfilled"));
	}

	if(aSilver)
	{
		mySilverStar->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("StarFilled"));
	}
	else
	{
		mySilverStar->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("StarUnfilled"));
	}

	if(aGold)
	{
		myGoldStar->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("StarFilled"));
	}
	else
	{
		myGoldStar->SetTexture(AssetRegistry::GetInstance()->GetAsset<Texture>("StarUnfilled"));
	}
}

void LevelSelectPreview::SetCounterValues(const int& aBronze, const int& aSilver, const int& aGold)
{
	myTransform->GetChildren()[4]->GetGameObject()->GetComponent<CounterComponent>()->SetValue(aBronze);
	myTransform->GetChildren()[5]->GetGameObject()->GetComponent<CounterComponent>()->SetValue(aSilver);
	myTransform->GetChildren()[6]->GetGameObject()->GetComponent<CounterComponent>()->SetValue(aGold);
}
