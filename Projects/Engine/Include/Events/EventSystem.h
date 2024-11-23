#pragma once
#include "Ref.h"
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

			GetEventListenerArray<EventType>()->ExecuteCallbacks(currentEvent);

			delete currentEvent;
		}

		template<typename EventType>
		static Ref<IEventListener> Listen(std::function<void(EventType*)> callback)
		{
			return GetEventListenerArray<EventType>()->AddListener(callback);
		}

		template<typename EventType>
		static void RemoveListener(Ref<IEventListener> listener)
		{
			GetEventListenerArray<EventType>()->RemoveListener(listener.As<EventListener<EventType>>());
		}

		template<typename EventType>
		static Ref<EventListenerArray<EventType>> GetEventListenerArray()
		{
			std::type_index typeID = typeid(EventType);

			if (eventTypeToListeners.find(typeID) == eventTypeToListeners.end())
			{
				eventTypeToListeners[typeID] = new EventListenerArray<EventType>();
			}

			return eventTypeToListeners[typeID].As<EventListenerArray<EventType>>();
		}

	private:
		inline static std::map<std::type_index, Ref<IEventListenerArray>> eventTypeToListeners;
	};
}