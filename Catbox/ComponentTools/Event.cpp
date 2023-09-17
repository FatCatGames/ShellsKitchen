#include "stdafx.h"
#include "Event.h"

unsigned int ListenerIDCount = 0;
void Event::AddListener(Listener& aListener)
{
	aListener.listenerId = ListenerIDCount++;
	myListeners.insert({ aListener.listenerId, aListener });
}

void Event::RemoveListener(Listener& aListener)
{
	myListeners.erase(aListener.listenerId);
}

void Event::RemoveListener(ListenerWithParameter& aListener)
{
	myListenersWithParameter.erase(aListener.listenerId);
}
void Event::RemoveListener(ListenerWithParameterAndID& aListener)
{
	myListenersWithParameterAndID.erase(aListener.listenerId);
}


void Event::AddListener(ListenerWithParameter& aListener)
{
	aListener.listenerId = ListenerIDCount++;
	myListenersWithParameter.insert({ aListener.listenerId, aListener });
}

void Event::AddListener(ListenerWithParameterAndID& aListener)
{
	aListener.listenerId = ListenerIDCount++;
	myListenersWithParameterAndID.insert({ aListener.listenerId, aListener });
}

void Event::Invoke(void* aPointer)
{
	for (auto& [id, listener] : myListenersWithParameter)
	{
		listener.action(aPointer);
	}
}

void Event::Invoke(void* aPointer, unsigned short aId)
{
	for (auto& [id, listener] : myListenersWithParameterAndID)
	{
		listener.action(aPointer, aId);
	}
}

void Event::ClearAllListeners()
{
	myListeners.clear();
	myListenersWithParameter.clear();
	myListenersWithParameterAndID.clear();
}

void Event::Invoke()
{
	for (auto& [id, listener] : myListeners)
	{
		listener.action();
	}
}
