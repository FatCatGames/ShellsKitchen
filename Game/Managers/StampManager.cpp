#include "Game.pch.h"
#include "StampManager.h"
StampManager* StampManager::myInstance = nullptr;

StampManager* StampManager::GetInstance()
{
    if (!myInstance) 
    {
        StampManager* stampManger = new StampManager();
        myInstance = stampManger;
    }
    return myInstance;
}

StampManager::~StampManager()
{
    if (myInstance) 
    {
        delete myInstance;
    }
    myInstance = nullptr;
}

void StampManager::Reset()
{
    myStampPoints[0] = {0,0,0,0,0};
    myStampPoints[1] = {0,0,0,0,0};
}

void StampManager::AddStampPoints(Stamp aStamp, int aPoint, int aPlayerIndex)
{
    myStampPoints[aPlayerIndex][aStamp] += aPoint;
}

const std::array<bool, 5>& StampManager::GetStamps(int aPlayerIndex)
{
    std::array<bool, 5> stamps = {false, false, false, false, false};
    int otherPlayer = 0;

    if (otherPlayer == aPlayerIndex) 
    {
        otherPlayer = 1;
    }

    if (myStampPoints[aPlayerIndex][0] > myStampPoints[otherPlayer][0]) 
    {
        stamps[0] = true;
    }

    if (myStampPoints[aPlayerIndex][1] > myStampPoints[otherPlayer][1]) 
    {
        if (myStampPoints[aPlayerIndex][1] >= myTrickShotReqiured) 
        {
            stamps[1] = true;
        }
    }

    if (myStampPoints[aPlayerIndex][2] > myStampPoints[otherPlayer][2])
    {
        if (myStampPoints[aPlayerIndex][2] >= myFoodWasterReqiured)
        {
            stamps[2] = true;
        }
    }

    if (myStampPoints[aPlayerIndex][3] > myStampPoints[otherPlayer][3])
    {
        if (myStampPoints[aPlayerIndex][3] > myClumsyRequired)
        {
            stamps[3] = true;
        }
    }

    if (myStampPoints[aPlayerIndex][4] > myStampPoints[otherPlayer][4])
    {
        if (myStampPoints[aPlayerIndex][4] > myAnnoyingRequired)
        {
            stamps[4] = true;
        }
    }
    return stamps;
}
