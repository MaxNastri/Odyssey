#include "GameObject.h"
#include "Component.h"

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey.Entities, GameObject);
	GameObject::GameObject()
	{
		name = "GameObject";
		id = -1;
		active = false;
	}

	GameObject::GameObject(int32_t ID)
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

		ryml::NodeRef componentsNode = gameObjectNode["Components"];
		componentsNode |= ryml::SEQ;

		auto serializeComponent = [&componentsNode](Component* component)
			{
				component->Serialize(componentsNode);
			};

		ComponentManager::ExecuteOnGameObjectComponents(id, serializeComponent);
	}

	void GameObject::Deserialize(ryml::NodeRef& node)
	{
		assert(node.is_map());
		node["Name"] >> name;
		node["Active"] >> active;
		node["ID"] >> id;

		ryml::NodeRef componentsNode = node["Components"];
		assert(componentsNode.is_seq());
		assert(componentsNode.has_children());

		for (size_t i = 0; i < componentsNode.num_children(); ++i)
		{
			ryml::ConstNodeRef componentNode = componentsNode.child(i);
			assert(componentNode.is_map());

			std::string componentType;
			componentNode["Name"] >> componentType;
			
			if (componentNode.has_child("Fields"))
			{
				UserScript* userScript = ComponentManager::AddUserScript(id, componentType);
				userScript->SetGameObject(this);
				userScript->Deserialize(componentNode);
			}
			else
			{
				Component* component = ComponentManager::AddComponentByName(id, componentType);
				component->SetGameObject(this);
				component->Deserialize(componentNode);
			}
		}

	}
}