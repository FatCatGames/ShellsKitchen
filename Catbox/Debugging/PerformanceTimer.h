#pragma once
#include <string>
#include <vector>
struct PerformanceTimer
{
	std::string parent;
	std::string name = "Whole Frame";
	std::vector<PerformanceTimer> myChildren;
	float myTimer = 0;
};