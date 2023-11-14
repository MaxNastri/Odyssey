#include "EventSystem.h"

namespace Odyssey
{
	std::map<std::type_index, std::unique_ptr<IEventListenerArray>> EventSystem::eventTypeToListeners;
}