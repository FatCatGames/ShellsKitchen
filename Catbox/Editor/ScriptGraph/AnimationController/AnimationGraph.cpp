#include "stdafx.h"
#include "MuninGraph.pch.h"
#include "AnimationGraph.h"
#include "MuninGraph.h"
#include "AnimationGraphPin.h"
#include "AnimationGraphNode.h"
#include "AnimationGraphSchema.h"
#include <fstream>
#include "..\nlohmann\json.hpp"

#include "AnimationGraphNode.h"
#include "Nodes/SGNode_Variable.h"
#include "AnimationTransition.h"
#include "Assets\AnimationController.h"
#include <Animator.cpp>
#include "Graphics\Animations\Animation.h"
#include <stack>


const AnimationGraphPin& AnimationGraphInternal::GetDataSourcePin(size_t aPinUID) const
{
	const AnimationGraphPin& dataPin = Super::GetDataSourcePin(aPinUID);
	if (dataPin.GetUID() != aPinUID && !dataPin.GetOwner()->IsExecNode())
	{
		dataPin.GetOwner()->DoOperation();
	}

	return dataPin;
}

void AnimationGraphInternal::ReportEdgeFlow(size_t anEdgeUID)
{
	myLastExecutedPath.push_back(anEdgeUID);
}

void AnimationGraphInternal::ReportFlowError(size_t aNodeUID, const std::string& anErrorMessage) const
{
	if (myErrorDelegate)
	{
		myErrorDelegate(*dynamic_cast<const AnimationGraph*>(this), aNodeUID, anErrorMessage);
	}
}

bool AnimationGraphInternal::TransitionToNextState(AnimationTransition* aTransition)
{
	if (aTransition->TransitionTime > 0)
	{
		myCurrentTransition = aTransition;
		myPreviousNode = myCurrentNode;
		myPreviousNode->isRunning = false;
		myCurrentNode = aTransition->GetToNode();
		myCurrentNode->isRunning = true;
		myCurrentState = AnimationState::Transitioning;
		return true;
	}
	else
	{
		myCurrentNode->isRunning = false;
		myPreviousNode = myCurrentNode;
		myPreviousNode->isRunning = false;
		myCurrentNode = aTransition->GetToNode();
		myCurrentNode->isRunning = true;
		myCurrentState = AnimationState::Playing;
		return true;
	}
}

bool AnimationGraphInternal::Animate()
{
	if (!myCurrentNode)
	{
		Run();
		return false;
	}

	myCurrentNode->DoOperation();

	if (myCurrentState == AnimationState::Transitioning)
	{
		myTransitionTimer += deltaTime;
		if (myTransitionTimer > myCurrentTransition->TransitionTime)
		{
			myTransitionTimer = 0;
			myCurrentTransition = nullptr;
			myCurrentState = AnimationState::Playing;
			//return true;
		}
	}
	for (auto [transitionUID, transition] : myCurrentNode->myTransitions)
	{
		auto& conditions = transition->GetConditions();
		if (conditions.empty())
		{
			if (myCurrentState == AnimationState::Finished || !myCurrentNode->GetAnimation())
			{
				return TransitionToNextState(transition);
			}
			continue;
		}
		else if (!transition->HasExitTime || myCurrentState == AnimationState::Finished)
		{
			bool conditionsMet = true;
			for (size_t i = 0; i < conditions.size(); i++)
			{
				if (!conditions[i].ConditionMet())
				{
					conditionsMet = false;
					break;
				}
			}

			if (conditionsMet)
			{
				return TransitionToNextState(transition);
			}
		}
	}

	for (auto [nodeID, node] : myAnyStateNodes)
	{
		for (auto [transitionUID, transition] : node->myTransitions)
		{
			if (myCurrentNode->GetAnimation() == transition->GetToNode()->GetAnimation())
			{
				if (!node->myCanTransitionToItself) continue;
			}

			auto& conditions = transition->GetConditions();
			if (conditions.empty()) continue; //Add exit time here

			bool conditionsMet = true;
			for (size_t i = 0; i < conditions.size(); i++)
			{
				if (!conditions[i].ConditionMet())
				{
					conditionsMet = false;
					break;
				}
			}

			if (conditionsMet)
			{
				return TransitionToNextState(transition);
			}
		}
	}

	return false;
}

void AnimationGraphInternal::RegenerateBoneMask(std::shared_ptr<Skeleton> aSkeleton)
{
	if (!aSkeleton) return;

	for (size_t i = 0; i < myBoneMask.size(); i++)
	{
		myBoneMask[i] = false;
	}

	std::stack<unsigned int> bonesToVisit;

	for (size_t i = 0; i < myBoneMasks.size(); i++)
	{
		bonesToVisit.push(myBoneMasks[i].id);

		while (!bonesToVisit.empty())
		{
			unsigned int currentBoneID = bonesToVisit.top();
			bonesToVisit.pop();

			if (myBoneMask[currentBoneID] == myBoneMasks[i].include) continue;

			myBoneMask[currentBoneID] = myBoneMasks[i].include;
			for (unsigned int bone : aSkeleton->bones[currentBoneID].childBoneIndices)
			{
				bonesToVisit.push(bone);
			}
		}
	}
}

bool AnimationGraphInternal::Run()
{
	if (myEntryPoint)
	{
		myCurrentNode = myEntryPoint;
		myLastExecutedPath.clear();
		return myEntryPoint->Exec(0) != 0;
	}
	return false;
}

void AnimationGraphInternal::Reset(bool transition)
{
	if (!transition)
	{
		myCurrentState = AnimationState::Playing;
	}
	else
	{
		myCurrentState = AnimationState::Transitioning;
	}

	myFrameIndex = 0;
	myNextFrameIndex = 1;
	myFramePercent = 0;
	myTimeSinceFrame = 0;
	myTransitionTimer = 0;
	myFrameIndexTransition = 0;
	myNextFrameIndexTransition = 1;
	myFramePercentTransition = 0;
	myTimeSinceFrameTransition = 0;
}

void AnimationGraphInternal::Restart()
{
	SetCurrentNode(myEntryPoint);
	Reset();
}

void AnimationGraphInternal::SetCurrentNode(std::shared_ptr<AnimationGraphNode> aNode)
{
	if (myCurrentNode)
	{
		myCurrentNode->isRunning = false;
	}

	myCurrentNode = aNode;
	myCurrentNode->isRunning = true;
	Reset();
}

void AnimationGraphInternal::SetController(AnimationController* aController)
{
	myController = aController;
	myVariables = &aController->GetVariables();
}


void AnimationGraphInternal::BindErrorHandler(ScriptGraphErrorHandlerSignature&& aErrorHandler)
{
	myErrorDelegate = aErrorHandler;
}

void AnimationGraphInternal::UnbindErrorHandler()
{
	myErrorDelegate = nullptr;
}
