#pragma once

class KitchenWindowDummyComponent : public Component
{
public:
	KitchenWindowDummyComponent();
	void Awake() override;
	void RunInEditor() override;
private:
	//Ignore this. You have not seen this.
	int GetLevel();
};