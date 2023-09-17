#pragma once

class FloatingScore : public Component
{
public:
	void Update() override;
	void SetPoints(const int& aPoints);

private:
	float myAliveTime = 0.0f;
	int myPoints = 1;
	bool myHasSetup = false;

};