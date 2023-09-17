#pragma once
#include <thread>

#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_Test)
{
	void threadfunc();

	std::thread myThread;

public:

	void Init() override;
	size_t DoOperation() override;
};
