#include "GameObject.h"
#include "Component.h"

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey, GameObject);
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

	void GameObject::Serialize(SerializationNode& node)
	{
		SerializationNode gameObjectNode = node.AppendChild();
		gameObjectNode.SetMap();
		gameObjectNode.WriteData("Name", name);
		gameObjectNode.WriteData("Type", Type);
		gameObjectNode.WriteData("Active", active);
		gameObjectNode.WriteData("ID", id);

		SerializationNode componentsNode = gameObjectNode.CreateSequenceNode("Components");

		auto serializeComponent = [&componentsNode](Component* component)
			{
				component->Serialize(componentsNode);
			};

		ComponentManager::ExecuteOnGameObjectComponents(id, serializeComponent);
	}

	void GameObject::Deserialize(SerializationNode& node)
	{
		assert(node.IsMap());
		node.ReadData("Name", name);
		node.ReadData("Active", active);
		node.ReadData("ID", id);

		SerializationNode componentsNode = node.GetNode("Components");
		assert(componentsNode.IsSequence());
		assert(componentsNode.HasChildren());

		for (size_t i = 0; i < componentsNode.ChildCount(); ++i)
		{
			SerializationNode componentNode = componentsNode.GetChild(i);
			assert(componentNode.IsMap());

			std::string componentType;
			componentNode.ReadData("Name", componentType);
			
			if (componentNode.HasChild("Fields"))
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