#include "Game.pch.h"
#include "LineupCustomers.h"
#include "LineupCustomer.h"
#include "../Interactables/Surfaces/DeliveryDesk.h"

void LineupCustomers::Awake()
{
	GameManager::GetInstance()->GetCurrentLevelManager().GetDeliveryDesk();

	myOrderDeliveredListener.action = [this](void* data) {OnOrderDelivered(reinterpret_cast<OrderResult*>(data)); };
	OrderHandler::GetInstance()->AddOrderDeliveredListener(myOrderDeliveredListener);

	myOrderGeneratedListener.action = [this](void* data) {SpawnCustomer(reinterpret_cast<Order*>(data)); };
	OrderHandler::GetInstance()->AddOrderGeneratedListener(myOrderGeneratedListener);

	myOrderTimedOutListener.action = [this](void* data) {OnOrderDelivered(reinterpret_cast<OrderResult*>(data)); };
	OrderHandler::GetInstance()->AddOrderTimedOutListener(myOrderTimedOutListener);
}

void LineupCustomers::OnOrderDelivered(OrderResult* anOrder)
{
	auto deliveryDesk = GameManager::GetInstance()->GetCurrentLevelManager().GetDeliveryDesk()->GetTransform();

	if (anOrder->order.myOrderID == -1) return;
	for (size_t i = 0; i < myQueue.size(); i++)
	{
		if (myQueue[i]->GetOrderId() == anOrder->order.myOrderID)
		{
			myQueue[i]->WalkAway(deliveryDesk->right());
			myQueue.erase(myQueue.begin() + i);
			UpdateQueue();
			return;
		}
	}
}

void LineupCustomers::SpawnCustomer(Order* anOrder)
{
	LineupCustomer* customer = InstantiatePrefab("LineupCustomer")->GetComponent<LineupCustomer>();

	auto deliveryDesk = GameManager::GetInstance()->GetCurrentLevelManager().GetDeliveryDesk()->GetTransform();

	Vector3f spawnPos = deliveryDesk->worldPos() + deliveryDesk->back() * myDeliveryDeskSpawnOffset;
	Vector3f targetPos = deliveryDesk->worldPos() + deliveryDesk->back() * myDeliveryDeskStayOffset +(deliveryDesk->back() * static_cast<float>(myQueue.size()) * myCustomerOffset);
	Vector3f dir = (targetPos * 100.0f) - spawnPos;
	float angleY = (std::atan2(dir.x, dir.z) * 57.3f);

	customer->SetInitialPosition(spawnPos, Vector3f(0, angleY,0));
	customer->SetNextWaypoint(targetPos);
	customer->SetOrderId(anOrder->myOrderID);
	myQueue.push_back(customer);
}

void LineupCustomers::UpdateQueue()
{
	auto deliveryDesk = GameManager::GetInstance()->GetCurrentLevelManager().GetDeliveryDesk()->GetTransform();

	for (size_t i = 0; i < myQueue.size(); i++)
	{
		Vector3f targetPos = deliveryDesk->worldPos() + deliveryDesk->back() * myDeliveryDeskStayOffset + (deliveryDesk->back() * static_cast<float>(i * myCustomerOffset));
		myQueue[i]->SetNextWaypoint(targetPos);
	}
}


void LineupCustomers::RenderInProperties(std::vector<Component*>& aComponentList)
{
	Catbox::DragFloat("Spawn offset", &myDeliveryDeskSpawnOffset);
}

void LineupCustomers::Save(rapidjson::Value& aComponentData)
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	wrapper->SaveValue<DataType::Float>("SpawnOffset", myDeliveryDeskSpawnOffset);
}

void LineupCustomers::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("SpawnOffset"))
	{
		myDeliveryDeskSpawnOffset = aComponentData["SpawnOffset"].GetFloat();
	}
}
