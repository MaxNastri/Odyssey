#include "ComponentManager.h"

namespace Odyssey::Entities
{
	std::unordered_map<std::type_index, unsigned int> ComponentManager::componentTypes;
	std::unordered_map<std::type_index, IComponentArray*> ComponentManager::componentArrays;
}