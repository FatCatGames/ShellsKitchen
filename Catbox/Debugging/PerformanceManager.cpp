#include "stdafx.h"
std::vector<std::chrono::time_point<std::chrono::system_clock>> PerformanceManager::myStarts;
bool PerformanceManager::myStartedTimer = false;
PerformanceTimer PerformanceManager::myWholeFrame;
std::vector<PerformanceTimer> PerformanceManager::myCurrentTimers;


void PerformanceManager::BeginTimer(const std::string& aName, const std::string& aParent = "Whole Frame"/*, PerformanceProfilerFlags aFlags*/)
{
	if (EDITORMODE) 
	{
		myStarts.push_back(std::chrono::system_clock::now());
		PerformanceTimer timer;
		if (aName.size() == 0)
		{
			timer.name = "Undefined";
		}
		else
		{
			timer.name = aName;
		}

		if (aParent.size() == 0)
		{
			timer.parent = "Whole Frame";
		}
		else
		{
			timer.parent = aParent;
		}
		AddTimerToWholeFrame(timer);
		myCurrentTimers.push_back(timer);
	}
}

void PerformanceManager::StopTimer(const std::string& aName)
{
	if (EDITORMODE)
	{
		myStartedTimer = false;
		auto aEnd = std::chrono::system_clock::now();

		int aId = -1;

		for (int i = 0; i < myCurrentTimers.size(); i++)
		{
			if (myCurrentTimers[i].name == aName) 
			{

				aId = i;
				break;
			}
		}

		PerformanceTimer* timer = ParentExists(myCurrentTimers[aId].parent, &myWholeFrame);
		if (timer)
		{
			for (int i = 0; i < timer->myChildren.size(); i++)
			{
				if (timer->myChildren[i].name == myCurrentTimers[aId].name)
				{
					timer->myChildren[i].myTimer = std::chrono::duration<double>(aEnd - myStarts[aId]).count();
					break;
				}
			}
		}
		myCurrentTimers.erase(myCurrentTimers.begin() + aId);
		myStarts.erase(myStarts.begin() + aId);
	}
}

PerformanceTimer PerformanceManager::GetWholeFrame()
{
	//SortChildren(&myWholeFrame);
	return myWholeFrame;
}

void PerformanceManager::ClearTimers()
{
	myWholeFrame.myTimer = realDeltaTime;
	myStartedTimer = false;
	myCurrentTimers.clear();
	myStarts.clear();
}

PerformanceTimer* PerformanceManager::ParentExists(const std::string& aParentName, PerformanceTimer* aTimer)
{
	if (aTimer->name == aParentName) 
	{
		return aTimer;
	}
	else
	{
		for (int i = 0; i < aTimer->myChildren.size(); i++)
		{
			PerformanceTimer* checkTimer = ParentExists(aParentName, &aTimer->myChildren[i]);
			if (checkTimer)
			{
				return checkTimer;
			}
		}
	}
	return nullptr;
}

void PerformanceManager::AddTimerToWholeFrame(PerformanceTimer aTimer)
{
	PerformanceTimer* timer = ParentExists(aTimer.parent, &myWholeFrame);
	if (!timer) 
	{
		timer = &myWholeFrame;
	}
	bool exists = false;
	for (int i = 0; i < timer->myChildren.size(); i++)
	{
		if (timer->myChildren[i].name == aTimer.name) 
		{
			exists = true;
			break;
		}
	}
	if (!exists) 
	{
		timer->myChildren.push_back(aTimer);
	}
}

void PerformanceManager::SortChildren(PerformanceTimer* aTimer) 
{
	if (aTimer->myChildren.size() > 1) 
	{
		int i, j;
		for (i = 0; i < aTimer->myChildren.size() - 1; ++i)
			for (j = 0; j < aTimer->myChildren.size() - i - 1; ++j)
				if (aTimer->myChildren[j].myTimer < aTimer->myChildren[j + 1].myTimer)
					std::swap(aTimer->myChildren[j], aTimer->myChildren[j + 1]);
	}
	for (int i = 0; i < aTimer->myChildren.size(); ++i)
	{
		SortChildren(&aTimer->myChildren[i]);
	}
}
