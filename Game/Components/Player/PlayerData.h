#pragma once

class PlayerData
{
public:
	PlayerData();
	~PlayerData();

	void SetHatID(int aID);
	int GetHatID() { return myHatID; }
	void SetIsReady(bool aValue) { myIsReady = aValue; }
	bool GetIsReady() { return myIsReady; }
private:

	int myHatID = -1;
	bool myIsReady = false;
};
