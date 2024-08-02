#include "GameObject.h"
#include "Scene.h"
#include "IDComponent.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "ScriptComponent.h"

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey, GameObject);

	GameObject::GameObject()
	{
	}

	GameObject::GameObject(Scene* scene, entt::entity entity)
	{
		m_Entity = entity;
		m_Scene = scene;
	}

	GameObject::GameObject(Scene* scene, uint32_t id)
	{
		m_Entity = (entt::entity)id;
		m_Scene = scene;
	}

	void GameObject::Serialize(SerializationNode& node)
	{
		IDComponent& idComponent = GetComponent<IDComponent>();

		SerializationNode gameObjectNode = node.AppendChild();
		gameObjectNode.SetMap();
		gameObjectNode.WriteData("Name", idComponent.Name);
		gameObjectNode.WriteData("GUID", idComponent.GUID);
		gameObjectNode.WriteData("Type", Type);

		SerializationNode componentsNode = gameObjectNode.CreateSequenceNode("Components");

		if (Camera* camera = TryGetComponent<Camera>())
			camera->Serialize(componentsNode);

		if (MeshRenderer* meshRenderer = TryGetComponent<MeshRenderer>())
			meshRenderer->Serialize(componentsNode);

		if (Transform* transform = TryGetComponent<Transform>())
			transform->Serialize(componentsNode);

		if (ScriptComponent* userScript = TryGetComponent<ScriptComponent>())
			userScript->Serialize(componentsNode);
	}

	void GameObject::Deserialize(SerializationNode& node)
	{
		assert(node.IsMap());

		IDComponent& idComponent = AddComponent<IDComponent>();
		node.ReadData("Name", idComponent.Name);
		node.ReadData("GUID", idComponent.GUID);

		SerializationNode componentsNode = node.GetNode("Components");
		assert(componentsNode.IsSequence());
		assert(componentsNode.HasChildren());

		for (size_t i = 0; i < componentsNode.ChildCount(); ++i)
		{
			SerializationNode componentNode = componentsNode.GetChild(i);
			assert(componentNode.IsMap());

			std::string componentType;
			componentNode.ReadData("Type", componentType);

			if (componentType == Camera::Type)
			{
				Camera& camera = AddComponent<Camera>();
				camera.Deserialize(componentNode);
			}
			else if (componentType == MeshRenderer::Type)
			{
				MeshRenderer& meshRenderer = AddComponent<MeshRenderer>();
				meshRenderer.Deserialize(componentNode);
			}
			else if (componentType == Transform::Type)
			{
				Transform& transform = AddComponent<Transform>();
				transform.Deserialize(componentNode);
			}
			else if (componentType == ScriptComponent::Type)
			{
				ScriptComponent& script = AddComponent<ScriptComponent>();
				script.Deserialize(componentNode);
			}
		}
	}
	const std::string& GameObject::GetName()
	{
		return GetComponent<IDComponent>().Name;
	}

	const std::string& GameObject::GetGUID()
	{
		return GetComponent<IDComponent>().GUID;
	}

	void GameObject::SetName(const std::string& name)
	{
		GetComponent<IDComponent>().Name = name;
	}

	void GameObject::SetGUID(const std::string& guid)
	{
		GetComponent<IDComponent>().GUID = guid;
	}
}