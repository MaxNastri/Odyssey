#pragma once
#include "Asset.h"
#include "Globals.h"
#include <ryml.hpp>

namespace Odyssey
{
	class GameObject : public Asset
	{
	public:
		GameObject();
		GameObject(uint32_t ID);
		void Serialize(ryml::NodeRef& node);
		void Deserialize(ryml::NodeRef& node);

	public:
		bool operator==(const GameObject& other) { return id == other.id; }

	public:
		bool active;
		uint32_t id;
		std::string name;
		CLASS_DECLARATION(GameObject);
	};
}