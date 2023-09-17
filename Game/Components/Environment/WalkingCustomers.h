#pragma once

class WalkingCustomers : public Component
{

public:
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

private:

	struct CustomerPath
	{
		Color color;
		std::vector<Vector3f> wayPoints;
	};

	std::vector<CustomerPath> myPaths;
	float myTimeUntilCustomerSpawn;
	int myPreviousPath = -1;
};