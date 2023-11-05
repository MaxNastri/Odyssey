#pragma once
#include <string>

namespace Odyssey::Entities
{
	class ComponentBindings
	{
	public:
		static void Initialize();
		static void AddComponentByTypeName(const std::string& classNamespace, const std::string& className);

	};
}