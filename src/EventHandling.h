#pragma once
#include <list>
#include <mutex>
#include "f4se/PapyrusEvents.h"

/*
	Defines commonly used Event Handling templates
*/

//TODO: Merge scaleform events too?
#pragma region C++ Events

//Registers a Plugin to a listeners pool for a generic event
template <typename ListenerCallbackT>
void GenericRegisterForEvent(ListenerCallbackT listenerCallback, std::list<ListenerCallbackT>* listenerList, std::mutex* listenerListMutex)
{
	if (listenerCallback)
	{
		std::lock_guard<std::mutex> lock(*listenerListMutex);
		listenerList->remove(listenerCallback);
		listenerList->push_back(listenerCallback);
	}
};

//Unregister a Plugin by removing it from the listeners pool
template <typename ListenerCallbackT>
void GenericUnregisterForEvent(ListenerCallbackT listenerCallback, std::list<ListenerCallbackT>* listenerList, std::mutex* listenerListMutex)
{
	if (listenerCallback)
	{
		std::lock_guard<std::mutex> lock(*listenerListMutex);
		listenerList->remove(listenerCallback);
	}
};

//Dispatches an event call to the listeners that previously registered a ListenerCallbackT callback
template <typename ListenerCallbackT, typename... Args>
void GenericDispactchEvent(std::list<ListenerCallbackT>* listenerLists, std::mutex* listenerListMutex, Args ... args)
{
	std::lock_guard<std::mutex> lock(*listenerListMutex);

	for (ListenerCallbackT& listener : *listenerLists)
		(*listener)(args...); //Expand Args
};

#pragma endregion
