#include "stdafx.h"
#include "AnimationController.h"
#include "Editor\Windows\AnimationControllerWindow.h"
#include "Editor\ScriptGraph\AnimationController\AnimationControllerEditor.h"

#include "Editor\ScriptGraph\AnimationController\AnimationGraphSchema.h"
#include "Graphics\Animations\SkeletonData.h"
#include "ScriptGraphVariable.h"
#include "Editor/ScriptGraph/AnimationController/AnimationTransition.h"
#include "Animator.h"
#include <mutex>

std::mutex myGuardLock;

void AnimationController::SaveAsset(const char* aPath)
{
	if (myLayers.empty())
	{
		myLayers.insert({ "Default", AnimationGraphSchema::CreateAnimationGraph(this) });
	}
	Editor::GetInstance()->GetAnimationControllerEditor()->SaveGraph(this, myPath.string());
}

void AnimationController::LoadFromPath(const char* aPath)
{
	if (EDITORMODE)
	{
		Editor::GetInstance()->GetAnimationControllerEditor()->LoadGraph(this, myPath.string());
	}
	else
	{
		std::ifstream file(aPath);

		const std::string inGraph = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		file.close();

		AnimationGraphSchema::DeserializeAnimationGraph(this, inGraph);
	}
}

void AnimationController::DoubleClickAsset()
{
	Editor::GetInstance()->GetAnimationControllerEditor()->ClearSelectedObject();
	Editor::GetInstance()->GetAnimationControllerEditor()->SetController(this);
	Editor::GetInstance()->GetAnimationControllerEditor()->SetGraph(myLayers.begin()->second, myPath.string());
}

void AnimationController::RenderInProperties(std::vector<Asset*>& anAssetList)
{
	char nameInput[32] = "";
	for (size_t i = 0; i < myName.size(); i++)
	{
		nameInput[i] = myName[i];
	}

	std::string emptyText = "##ACNameInput";
	if (ImGui::InputText(emptyText.c_str(), nameInput, IM_ARRAYSIZE(nameInput)))
	{
		myName = nameInput;
	}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		AssetRegistry::GetInstance()->RenameAsset(this, myName);
	}

	bool edit = false;
	std::shared_ptr<Skeleton> newSk = AssetRegistry::GetInstance()->AssetDropdownMenu(mySkeleton.get(), "Skeleton", edit);
	if (edit)
	{
		mySkeleton = newSk;
	}
}

void AnimationController::Reset()
{
	for (auto& layer : myLayers)
	{
		layer.second->Reset();
	}
}

void AnimationController::Restart()
{
	for (auto& layer : myLayers)
	{
		layer.second->Restart();
	}
}

void AnimationController::UpdateSkeleton(Animator* anActor, std::shared_ptr<SkeletonInstance> aSkeleton)
{
	auto& ogTransform = anActor->GetTransform()->GetWorldTransformMatrix();

	for (size_t i = 0; i < aSkeleton->boneTransforms.size(); i++)
	{
		Catbox::Matrix4x4<float> m(aSkeleton->boneMatrices[i]);
		m = m.Transpose(m);

		m = m * ogTransform;
		Vector3f scale = m.GetScaleFromMatrix(m);
		Vector3f rot = m.GetRotationFromMatrix(m);
		Vector3f pos = Vector3f(m(4, 1), m(4, 2), m(4, 3));

		aSkeleton->boneTransforms[i].SetWorldTransform(pos, rot, scale);
	}

	for (size_t i = 0; i < aSkeleton->sharedData->bones.size(); i++)
	{
		aSkeleton->boneMatrices[i] *= aSkeleton->sharedData->bones[i].bindPoseInverse;
	}
}

float CullSmoothingDistanceSqr = 100;
float BaseTickRate = 0.0001f;

std::shared_ptr<Animation> AnimationController::UpdateAnimations(Animator* anActor)
{
	const std::scoped_lock<std::mutex> lock(myGuardLock);

	auto& skeleton = anActor->GetSkeleton();
	std::array<Catbox::Matrix4x4<float>, 128> boneMatrices;

	float distSqr = (anActor->GetTransform()->worldPos() - GraphicsEngine::GetInstance()->GetMainCamera()->GetTransform()->worldPos()).LengthSqr();
	//bool useSmoothing = distSqr < CullSmoothingDistanceSqr;
	bool useSmoothing = true;
	
	float tickRate = BaseTickRate;
	/*
	if (anActor->UseDistanceBasedTickRate())
	{
		tickRate *= distSqr;
	}
	//print("Distance " + std::to_string(distSqr));
	*/

	myTimer += deltaTime;

	for (auto& layerName : myLayersIndices)
	{
		auto& layer = myLayers.at(layerName);
		//animationResults.emplace_back();
		if (!layer->isActive) continue;
		bool printName = false;
		if (layer->Animate())
		{
			printName = true;
			if (layer->myCurrentState == AnimationState::Transitioning)
			{
				layer->Reset(true);
			}
			else
			{
				layer->Reset();
			}
			auto& events = layer->GetCurrentNode()->GetEvents();
			for (size_t i = 0; i < events.size(); i++)
			{
				if (events[i].myFrame == layer->myFrameIndex)
				{
					events[i].Execute(anActor);
				}
			}

			if (!layer->GetCurrentNode()->GetAnimation())
			{
				continue;
			}
		}

		bool shouldBlend = useSmoothing;
		auto currentNode = layer->GetCurrentNode();
		auto previousNode = layer->GetPreviousNode();
		auto currentAnimation = currentNode->GetAnimation();
		std::shared_ptr<Animation> previousAnimation = nullptr;
		if (previousNode)
		{
			previousAnimation = previousNode->GetAnimation();
		}
		if (currentAnimation && layer->myCurrentState == AnimationState::Playing)
		{
			float timePerFrame = 1 / currentAnimation->framesPerSecond;
			layer->myTimeSinceFrame += deltaTime;
			if (layer->myTimeSinceFrame >= timePerFrame)
			{
				layer->myTimeSinceFrame = 0;

				if (++layer->myFrameIndex >= currentAnimation->frameCount)
				{
					if (!currentNode->myShouldLoop)
					{
						shouldBlend = false;
						layer->myFrameIndex = currentAnimation->frameCount - 1;
						layer->myCurrentState = AnimationState::Finished;
					}
					else layer->myFrameIndex = 1;
				}

				auto& events = currentNode->GetEvents();
				for (size_t i = 0; i < events.size(); i++)
				{
					if (events[i].myFrame == layer->myFrameIndex)
					{
						events[i].Execute(anActor);
					}
				}
			}


			layer->myFramePercent = layer->myTimeSinceFrame / timePerFrame;
			layer->myNextFrameIndex = layer->myFrameIndex + 1;

			if (layer->myNextFrameIndex >= currentAnimation->frameCount)
			{
				if (currentNode->myShouldLoop)
				{
					layer->myNextFrameIndex = (layer->myNextFrameIndex % currentAnimation->frameCount) + 1;
				}
				else
				{
					layer->myNextFrameIndex = layer->myFrameIndex;
				}
			}

		}
		else if (currentAnimation && layer->myCurrentState == AnimationState::Transitioning)
		{
			//Current Animation
			float timePerFrame = 1 / currentAnimation->framesPerSecond;
			layer->myTimeSinceFrame += deltaTime;
			if (layer->myTimeSinceFrame >= timePerFrame)
			{
				layer->myTimeSinceFrame = 0;

				if (++layer->myFrameIndex >= currentAnimation->frameCount)
				{
					if (!currentNode->myShouldLoop)
					{
						shouldBlend = false;
						layer->myFrameIndex = currentAnimation->frameCount - 1;
						layer->myCurrentState = AnimationState::Finished;
					}
					else layer->myFrameIndex = 1;
				}

				auto& events = currentNode->GetEvents();
				for (size_t i = 0; i < events.size(); i++)
				{
					if (events[i].myFrame == layer->myFrameIndex)
					{
						events[i].Execute(anActor);
					}
				}
			}
			else if (layer->myFrameIndex >= currentAnimation->frameCount)
			{
				print("erros");
			}

			layer->myFramePercent = layer->myTimeSinceFrame / timePerFrame;
			layer->myNextFrameIndex = layer->myFrameIndex + 1;

			if (layer->myNextFrameIndex >= currentAnimation->frameCount)
			{
				//if (currentNode->myShouldLoop)
				//{
				layer->myNextFrameIndex = (layer->myNextFrameIndex % currentAnimation->frameCount) + 1;
				//}
				//else
				//{
				//	layer->myNextFrameIndex = layer->myFrameIndex;
				//}
			}

			//Previous Animation To Blend With
			if (previousAnimation)
			{
				timePerFrame = 1 / previousAnimation->framesPerSecond;
				layer->myTimeSinceFrameTransition += deltaTime;
				if (layer->myTimeSinceFrameTransition >= timePerFrame)
				{
					layer->myTimeSinceFrameTransition = 0;

					if (++layer->myFrameIndexTransition >= previousAnimation->frameCount)
					{
						if (!currentNode->myShouldLoop)
						{
							//shouldBlend = false;
							layer->myFrameIndexTransition = previousAnimation->frameCount - 1;
							//layer->myCurrentState = AnimationState::Playing;
						}
						/*else */
						//layer->myCurrentState = AnimationState::Playing;
						layer->myFrameIndexTransition = 1;
					}

					auto& events = layer->GetPreviousNode()->GetEvents();
					for (size_t i = 0; i < events.size(); i++)
					{
						if (events[i].myFrame == layer->myFrameIndexTransition)
						{
							events[i].Execute(anActor);
						}
					}
				}

				layer->myFramePercentTransition = layer->myTimeSinceFrameTransition / timePerFrame;
				layer->myNextFrameIndexTransition = layer->myFrameIndexTransition + 1;

				if (layer->myNextFrameIndexTransition >= previousAnimation->frameCount)
				{
					if (currentNode->myShouldLoop)
					{
						layer->myNextFrameIndexTransition = (layer->myNextFrameIndexTransition % previousAnimation->frameCount) + 1;
					}
					else
					{
						layer->myNextFrameIndexTransition = layer->myFrameIndexTransition;
					}
				}
			}
		}


		if (myTimer > tickRate && currentAnimation)
		{
			if (shouldBlend)
			{
				if (layer->myCurrentState == AnimationState::Transitioning && previousAnimation)
				{
					//print("Transitioning");
					skeleton->UpdateAnimationHierarchy(0, currentAnimation->frames[layer->myFrameIndex], currentAnimation->frames[layer->myNextFrameIndex], previousAnimation->frames[layer->myFrameIndexTransition], previousAnimation->frames[layer->myNextFrameIndexTransition], Catbox::Matrix4x4<float>(), layer->myFramePercent, layer->myFramePercentTransition, layer->myTransitionTimer / layer->GetCurrentTransition()->TransitionTime, layer->myBoneMask);
					//skeleton->UpdateAnimationHierarchy(0, currentAnimation->frames[layer->myFrameIndex], currentAnimation->frames[layer->myNextFrameIndex], Catbox::Matrix4x4<float>(), layer->myFramePercent, layer->myBoneMask);
				}
				else
				{
					skeleton->UpdateAnimationHierarchy(0, currentAnimation->frames[layer->myFrameIndex], currentAnimation->frames[layer->myNextFrameIndex], Catbox::Matrix4x4<float>(), layer->myFramePercent, layer->myBoneMask);
				}
			}
			else
			{
				skeleton->UpdateAnimationHierarchyNoBlend(0, currentAnimation->frames[layer->myFrameIndex], Catbox::Matrix4x4<float>(), layer->myBoneMask);
			}
		}
	}

	if (myTimer > tickRate)
	{
		myTimer = 0;

		UpdateSkeleton(anActor, skeleton);
	}

	return nullptr;
}

std::shared_ptr<Animation> AnimationController::GetCurrentAnimation()
{
	if (!myCurrentLayer) return nullptr;
	auto currentNode = myCurrentLayer->GetCurrentNode();
	return currentNode->GetAnimation();
}

bool AnimationController::GetShouldLoop()
{
	return myCurrentLayer->GetCurrentNode()->myShouldLoop;
}

std::shared_ptr<AnimationController> AnimationController::MakeInstance()
{
	auto newInstance = std::make_shared<AnimationController>();
	newInstance->SetPath(myPath.string());

	std::ifstream file(myPath);
	const std::string inGraph = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.close();
	AnimationGraphSchema::DeserializeAnimationGraph(newInstance.get(), inGraph);

	newInstance->myRootAsset = this;
	return newInstance;
}

std::shared_ptr<AnimationGraph> AnimationController::AddLayer(std::string& aName)
{
	auto newGraph = AnimationGraphSchema::CreateAnimationGraph(this);
	myLayersIndices.push_back(aName);
	myLayers.insert({ aName, newGraph });
	if (!myCurrentLayer)
	{
		myCurrentLayer = newGraph;
		myCurrentLayer->isActive = true;
	}

	for (size_t i = 0; i < newGraph->myBoneMask.size(); i++)
	{
		newGraph->myBoneMask[i] = true;
	}
	return newGraph;
}

void AnimationController::RemoveLayer(std::string& aName)
{
	for (size_t i = 0; i < myLayersIndices.size(); i++)
	{
		if (myLayersIndices[i] == aName)
		{
			myLayersIndices.erase(myLayersIndices.begin() + i);
			break;
		}
	}
	myLayers.erase(aName);
}

void AnimationController::SetActiveLayer(const std::string& aLayer, bool aIsActive)
{
	if (myLayers.find(aLayer) != myLayers.end())
	{
		myLayers.at(aLayer)->isActive = aIsActive;
		if (aIsActive)
		{
			myLayers.at(aLayer)->SetCurrentNode(myLayers.at(aLayer)->GetEntryPoint());
			myLayers.at(aLayer)->Animate();
		}
	}
	else
	{
		printerror("Tried to access layer " + aLayer + " but it does not exist!");
	}
}

void AnimationController::RandomizeFrame()
{
	int maxFrames = myCurrentLayer->GetCurrentNode()->GetAnimation()->frameCount;
	myCurrentLayer->myFrameIndex = Catbox::GetRandom(0, maxFrames - 1);
	myCurrentLayer->myNextFrameIndex = myCurrentLayer->myFrameIndex + 1;
}

void AnimationController::SetSkeleton(std::shared_ptr<Skeleton> aSkeleton)
{
	mySkeleton = aSkeleton;
	for (auto& layer : myLayers)
	{
		for (size_t i = 0; i < layer.second->myBoneMask.size(); i++)
		{
			layer.second->myBoneMask[i] = true;
		}
	}
}
