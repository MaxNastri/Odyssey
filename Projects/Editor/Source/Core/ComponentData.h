#pragma once
#include "Ref.h"
#include "GameObject.h"
#include "Inspectors.h"

namespace Odyssey
{
	class ComponentData
	{
	public:
		static void Init();

	private:
		struct Properties
		{
		public:
			std::string Category;
			uint32_t Priority;
			std::string Type;
			std::string Name;
			std::function<Ref<Inspector>(GameObject&)>CreateInspectorFunc;
			std::function<void(GameObject&)> AddComponent;
		};

		std::map<std::string, size_t> s_TypeToComponent;
		std::map<std::string, size_t> s_CategoryToComponent;
		std::vector<Properties> m_Properties;
	};
}