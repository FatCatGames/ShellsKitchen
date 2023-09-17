#pragma once
#include "Skillet.h"

class Pot : public Skillet
{
public:
	bool GetIsPot() override { return true; };

private:

};