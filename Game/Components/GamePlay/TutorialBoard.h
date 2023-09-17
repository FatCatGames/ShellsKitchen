#pragma once

class TutorialBoard : public Component
{
public:
	void Awake() override;
	void ActivateMe(const int& anIndex);

private:
	int myTutorialIndex = 0;

};