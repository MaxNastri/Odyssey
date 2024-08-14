#pragma once
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
		template<typename EventType, typename... Args>
		static void Dispatch(Args&&... params)
		{
			std::type_index typeID = typeid(EventType);

			EventType* currentEvent = new EventType(params...);

			EventListenerArray<EventType>* eventListenerArray = GetEventListenerArray<EventType>();
			eventListenerArray->ExecuteCallbacks(currentEvent);

			delete currentEvent;
		}

		template<typename EventType>
		static std::shared_ptr<IEventListener> Listen(std::function<void(EventType*)> callback)
		{
			EventListenerArray<EventType>* eventListenerArray = GetEventListenerArray<EventType>();
			return eventListenerArray->AddListener(callback);
		}

		template<typename EventType>
		static void RemoveListener(std::shared_ptr<IEventListener> listener)
		{
			EventListenerArray<EventType>* eventListenerArray = GetEventListenerArray<EventType>();
			std::shared_ptr<EventListener<EventType>> eventListener = std::static_pointer_cast<EventListener<EventType>>(listener);
			eventListenerArray->RemoveListener(eventListener);
		}

		template<typename EventType>
		static EventListenerArray<EventType>* GetEventListenerArray()
		{
			std::type_index typeID = typeid(EventType);
			if (eventTypeToListeners.find(typeID) == eventTypeToListeners.end())
			{
				eventTypeToListeners[typeID] = std::make_unique<EventListenerArray<EventType>>();
			}
			return static_cast<EventListenerArray<EventType>*>(eventTypeToListeners[typeID].get());
		}

	private:
		static std::map<std::type_index, std::unique_ptr<IEventListenerArray>> eventTypeToListeners;
	};
}