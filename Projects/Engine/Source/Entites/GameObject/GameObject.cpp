#include "GameObject.h"
#include "Scene.h"
#include "PropertiesComponent.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "ScriptComponent.h"
#include "Animator.h"
#include "Light.h"
#include "ParticleEmitter.h"
#include "SpriteRenderer.h"

namespace Odyssey
{
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

	bool GameObject::IsValid()
	{
		return m_Scene && m_Scene->m_Registry.valid(m_Entity);
	}

	void GameObject::Serialize(SerializationNode& gameObjectNode)
	{
		PropertiesComponent& properties = GetComponent<PropertiesComponent>();

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

		if (Light* light = TryGetComponent<Light>())
			light->Serialize(componentsNode);

		if (ParticleEmitter* particleSystem = TryGetComponent<ParticleEmitter>())
			particleSystem->Serialize(componentsNode);

		if (SpriteRenderer* spriteRenderer = TryGetComponent<SpriteRenderer>())
			spriteRenderer->Serialize(componentsNode);
	}

	void GameObject::Deserialize(SerializationNode& node)
	{
		PropertiesComponent& properties = GetComponent<PropertiesComponent>();
		node.ReadData("Name", properties.Name);
		node.ReadData("GUID", properties.GUID.Ref());

		SerializationNode componentsNode = node.GetNode("Components");
		assert(componentsNode.IsSequence());

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
			else if (componentType == Light::Type)
			{
				Light& light = AddComponent<Light>();
				light.Deserialize(componentNode);
			}
			else if (componentType == ParticleEmitter::Type)
			{
				ParticleEmitter& particleSystem = AddComponent<ParticleEmitter>();
				particleSystem.Deserialize(componentNode);
			}
			else if (componentType == SpriteRenderer::Type)
			{
				SpriteRenderer& spriteRenderer = AddComponent<SpriteRenderer>();
				spriteRenderer.Deserialize(componentNode);
			}
		}
	}

	void GameObject::SetParent(const GameObject& parent)
	{
		m_Scene->GetSceneGraph().SetParent(parent, *this);
	}

	void GameObject::RemoveParent()
	{
		m_Scene->GetSceneGraph().RemoveParent(*this);
	}

	GameObject GameObject::GetParent()
	{
		return m_Scene->GetSceneGraph().GetParent(*this);
	}

	std::vector<GameObject> GameObject::GetChildren()
	{
		return m_Scene->GetSceneGraph().GetChildren(*this);
	}

	const std::string& GameObject::GetName()
	{
		return GetComponent<PropertiesComponent>().Name;
	}

	GUID GameObject::GetGUID()
	{
		return GetComponent<PropertiesComponent>().GUID;
	}

	void GameObject::SetName(std::string_view name)
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