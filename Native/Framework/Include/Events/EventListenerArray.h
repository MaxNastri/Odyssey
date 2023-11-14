#pragma once

namespace Odyssey
{
	// Empty abstract base class so we can store the templated version in containers
	class IEventListenerArray
	{
	};

	template<typename EventType>
	class EventListenerArray : public IEventListenerArray
	{
	public:

		void AddCallback(std::function<void(EventType*)> callback)
		{
			callbacks.push_back(callback);
		}
		void ExecuteCallbacks(EventType* e)
		{
			for (auto& callback : callbacks)
			{
				callback(e);
			}
		}
	private:
		std::vector<std::function<void(EventType*)>> callbacks;
	};
}