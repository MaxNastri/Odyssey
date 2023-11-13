#pragma once
#include "Globals.h"
#include <Serialization.h>
#include <ryml.hpp>

namespace Odyssey::Entities
{
	class GameObject
	{
	public:
		GameObject();
		GameObject(unsigned int ID);
		void Serialize(json& jsonObject);
		void Deserialize(const json& jsonObject);
		void Serialize(ryml::NodeRef& node);
		
	public:
		bool operator==(const GameObject& other) { return id == other.id; }

	public:
		bool active;
		unsigned int id;
		std::string name;
		CLASS_DECLARATION(GameObject);
		ODYSSEY_SERIALIZE(GameObject, Type, active, id, name);
	};
}