#pragma once

class LineupCustomer;
class LineupCustomers : public Component
{

public:
	void Awake() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

private:
	void OnOrderDelivered(OrderResult* anOrder);
	void SpawnCustomer(Order* anOrder);
	void UpdateQueue();

	ListenerWithParameter myOrderDeliveredListener;
	ListenerWithParameter myOrderGeneratedListener;
	ListenerWithParameter myOrderTimedOutListener;

	float myDeliveryDeskSpawnOffset = 5.f;
	float myDeliveryDeskStayOffset = 0.93f;
	float myCustomerOffset = 1.3f;

	std::vector<LineupCustomer*> myQueue;
};