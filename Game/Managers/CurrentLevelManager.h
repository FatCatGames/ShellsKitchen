#pragma once

class PlateHolder;
class DeliveryDesk;
class Stove;
class Counter;
class CurrentLevel
{
public:
	static CurrentLevel* GetInstance() { return Instance; }
	CurrentLevel();
	void ResetLevel();
	void AddStove(Stove* aStove) { myStoves.push_back(aStove); }
	Stove* GetAvailableStove();

	void SetPlateHolder(PlateHolder* aPlateHolder) { myPlateHolder = aPlateHolder; }
	PlateHolder* GetPlateHolder() { return myPlateHolder; }

	void SetDeliveryDesk(DeliveryDesk* aDeliveryDesk) { myDeliveryDesk = aDeliveryDesk; }
	DeliveryDesk* GetDeliveryDesk() {return myDeliveryDesk; }

private:
	static CurrentLevel* Instance;
	PlateHolder* myPlateHolder;
	DeliveryDesk* myDeliveryDesk;
	std::vector<Stove*> myStoves;
};