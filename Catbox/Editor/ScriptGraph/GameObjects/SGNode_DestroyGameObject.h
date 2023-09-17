#pragma once
#include "MuninScriptGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_DestroyGameObject)
{
public:
	void Init() override;
	size_t DoOperation() override;
};

