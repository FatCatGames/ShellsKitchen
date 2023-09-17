#pragma once
#include <functional>


// listener.action = [this] { Function(); };
struct Listener
{
	std::function<void()> action;

private:
	friend class Event;
	int listenerId;
};

//listener.action = [this](void* data) {Function(reinterpret_cast<T*>(data)); };
struct ListenerWithParameter 
{
	std::function<void(void*)> action;

private:
	friend class Event;
	int listenerId;
};

struct ListenerWithParameterAndID
{
	std::function<void(void*, unsigned short)> action;

private:
	friend class Event;
	unsigned int listenerId;
};
class Event
{
public:
	Event() = default;
	~Event() = default;
	void AddListener(Listener& aListener);
	void AddListener(ListenerWithParameter& aListener);
	void AddListener(ListenerWithParameterAndID& aListener);
	void RemoveListener(Listener& aListener);
	void RemoveListener(ListenerWithParameter& aListener);
	void RemoveListener(ListenerWithParameterAndID& aListener);
	void Invoke();
	void Invoke(void* aPointer);
	void Invoke(void* aPointer, unsigned short aId);
	void ClearAllListeners();

private:
	std::unordered_map<unsigned int, Listener> myListeners;
	std::unordered_map<unsigned int, ListenerWithParameter> myListenersWithParameter;
	std::unordered_map<unsigned int, ListenerWithParameterAndID> myListenersWithParameterAndID;
};