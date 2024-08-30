#include "GameObject.h"
#include "Scene.h"
#include "PropertiesComponent.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "ScriptComponent.h"
#include "Animator.h"

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
		PropertiesComponent& properties = GetComponent<PropertiesComponent>();

		SerializationNode gameObjectNode = node.AppendChild();
		gameObjectNode.SetMap();
		gameObjectNode.WriteData("Name", properties.Name);
		gameObjectNode.WriteData("GUID", properties.GUID.CRef());
		gameObjectNode.WriteData("Type", Type);

		SerializationNode componentsNode = gameObjectNode.CreateSequenceNode("Components");

		if (Animator* animator = TryGetComponent<Animator>())
			animator->Serialize(componentsNode);

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

		if (!HasComponent<PropertiesComponent>())
			AddComponent<PropertiesComponent>();

		PropertiesComponent& properties = GetComponent<PropertiesComponent>();
		node.ReadData("Name", properties.Name);
		node.ReadData("GUID", properties.GUID.Ref());

		SerializationNode componentsNode = node.GetNode("Components");
		assert(componentsNode.IsSequence());
		assert(componentsNode.HasChildren());

		for (size_t i = 0; i < componentsNode.ChildCount(); ++i)
		{
			SerializationNode componentNode = componentsNode.GetChild(i);
			assert(componentNode.IsMap());

			std::string componentType;
			componentNode.ReadData("Type", componentType);

			if (componentType == Animator::Type)
			{
				Animator& animator = AddComponent<Animator>();
				animator.Deserialize(componentNode);
			}
			else if (componentType == Camera::Type)
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
		return GetComponent<PropertiesComponent>().Name;
	}

	GUID GameObject::GetGUID()
	{
		return GetComponent<PropertiesComponent>().GUID;
	}

	void GameObject::SetName(const std::string& name)
	{
		GetComponent<PropertiesComponent>().Name = name;
	}

	void GameObject::SetGUID(GUID guid)
	{
		GetComponent<PropertiesComponent>().GUID = guid;
	}

	void GameObject::Destroy()
	{
		if (m_Scene)
			m_Scene->DestroyGameObject(*this);
	}
}