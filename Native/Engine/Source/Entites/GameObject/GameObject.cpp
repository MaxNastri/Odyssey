#include "GameObject.h"
#include "Component.h"
#include "ComponentManager.h"

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey.Entities, GameObject);
	GameObject::GameObject()
	{
		name = "GameObject";
		id = -1;
		active = false;
	}

	GameObject::GameObject(unsigned int ID)
	{
		name = "GameObject";
		id = ID;
		active = true;
	}

	void GameObject::Serialize(ryml::NodeRef& node)
	{
		ryml::NodeRef gameObjectNode = node.append_child();
		gameObjectNode |= ryml::MAP;
		gameObjectNode["Name"] << name;
		gameObjectNode["Type"] << Type;
		gameObjectNode["Active"] << active;
		gameObjectNode["ID"] << id;
		gameObjectNode["UUID"] << uuid;

		ryml::NodeRef componentsNode = gameObjectNode["Components"];
		componentsNode |= ryml::SEQ;

		auto serializeComponent = [&componentsNode](Component* component)
			{
				component->Serialize(componentsNode);
			};

		ComponentManager::ExecuteOnGameObjectComponents(*this, serializeComponent);
	}

	void GameObject::Deserialize(ryml::NodeRef& node)
	{
		assert(node.is_map());
		node["Name"] >> name;
		node["Active"] >> active;
		node["ID"] >> id;
		node["UUID"] >> uuid;

		ryml::NodeRef componentsNode = node["Components"];
		assert(componentsNode.is_seq());
		assert(componentsNode.has_children());

		for (size_t i = 0; i < componentsNode.num_children(); ++i)
		{
			ryml::ConstNodeRef componentNode = componentsNode.child(i);
			assert(componentNode.is_map());

			std::string componentType;
			std::string componentUUID;
			componentNode["Name"] >> componentType;
			componentNode["UUID"] >> componentUUID;

			if (componentUUID.starts_with("U_"))
			{
				UserScript* userScript = ComponentManager::AddUserScript(*this, componentType);
				userScript->Deserialize(componentNode);
			}
			else
			{
				Component* component = ComponentManager::AddComponentByName(*this, componentType);
				component->Deserialize(componentNode);
			}
		}

	}
}