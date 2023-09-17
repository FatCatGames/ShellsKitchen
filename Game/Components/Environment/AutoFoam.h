#pragma once
#include "Sharkfin.h"
class AutoFoam : public Component
{
	void Awake() override;
	void Update() override;
	void RunInEditor() override;

private:
	Transform* myParent = nullptr;
	Sharkfin* myFin = nullptr;
};

