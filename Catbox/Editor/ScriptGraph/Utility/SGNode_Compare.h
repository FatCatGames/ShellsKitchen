#pragma once
#include "MuninScriptGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"


BeginScriptGraphNode(SGNode_Compare)
{
public:
	void Init() override;

	size_t DoOperation() override;
};

BeginScriptGraphNode(SGNode_Compare_String)
{
public:
	void Init() override;

	size_t DoOperation() override;
};


