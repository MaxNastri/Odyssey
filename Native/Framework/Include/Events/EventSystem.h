#pragma once
#include <typeindex>
#include <map>
#include <functional>
#include "EventListenerArray.h"

namespace Odyssey
{
	struct Event
	{
	public:
		void* data;
	};

	class EventSystem
	{
	public:
		template<typename T, typename... Args>
		static void Dispatch(Args&&... params)
		{
			std::type_index typeID = typeid(T);

			T* currentEvent = new T(params...);

			EventListenerArray<T>* eventListenerArray = GetEventListenerArray<T>();
			eventListenerArray->ExecuteCallbacks(currentEvent);

			delete currentEvent;
		}

		template<typename T>
		static void Listen(std::function<void(T*)> callback)
		{
			EventListenerArray<T>* eventListenerArray = GetEventListenerArray<T>();
			eventListenerArray->AddCallback(callback);
		}

		template<typename T>
		static EventListenerArray<T>* GetEventListenerArray()
		{
			std::type_index typeID = typeid(T);
			if (eventTypeToListeners.find(typeID) == eventTypeToListeners.end())
			{
				eventTypeToListeners[typeID] = std::make_unique<EventListenerArray<T>>();
			}
			return static_cast<EventListenerArray<T>*>(eventTypeToListeners[typeID].get());
		}

	private:
		static std::map<std::type_index, std::unique_ptr<IEventListenerArray>> eventTypeToListeners;
	};
}