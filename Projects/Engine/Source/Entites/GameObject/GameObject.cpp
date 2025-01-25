#include "GameObject.h"
#include "Scene.h"
#include "Components.h"

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

	void GameObject::Awake()
	{
		if (Camera* camera = TryGetComponent<Camera>())
			camera->Awake();
		if (ScriptComponent* script = TryGetComponent<ScriptComponent>())
			script->Awake();
		if (RigidBody* rigidBody = TryGetComponent<RigidBody>())
			rigidBody->Awake();
	}

	void GameObject::Serialize(SerializationNode& gameObjectNode)
	{
		Serialize(gameObjectNode, false);
	}

	void GameObject::SerializeAsPrefab(SerializationNode& gameObjectNode, std::map<GUID, GUID>& remap)
	{
		PropertiesComponent& properties = GetComponent<PropertiesComponent>();

		// Remap the guid to the new values in the prefab
		GUID guid = properties.GUID;
		if (remap.contains(guid))
			guid = remap[guid];

		gameObjectNode.WriteData("Name", properties.Name);
		gameObjectNode.WriteData("GUID", guid.CRef());
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

		// Important: We use a separate serialize function to pass along the remapped guids
		if (ScriptComponent* userScript = TryGetComponent<ScriptComponent>())
			userScript->SerializeAsPrefab(componentsNode, remap);

		if (Light* light = TryGetComponent<Light>())
			light->Serialize(componentsNode);

		if (ParticleEmitter* particleSystem = TryGetComponent<ParticleEmitter>())
			particleSystem->Serialize(componentsNode);

		if (SpriteRenderer* spriteRenderer = TryGetComponent<SpriteRenderer>())
			spriteRenderer->Serialize(componentsNode);

		if (CharacterController* characterController = TryGetComponent<CharacterController>())
			characterController->Serialize(componentsNode);

		if (BoxCollider* boxCollider = TryGetComponent<BoxCollider>())
			boxCollider->Serialize(componentsNode);

		if (CapsuleCollider* capsuleCollider = TryGetComponent<CapsuleCollider>())
			capsuleCollider->Serialize(componentsNode);

		if (SphereCollider* sphereCollider = TryGetComponent<SphereCollider>())
			sphereCollider->Serialize(componentsNode);

		if (RigidBody* rigidBody = TryGetComponent<RigidBody>())
			rigidBody->Serialize(componentsNode);

		if (FluidBody* fluidBody = TryGetComponent<FluidBody>())
			fluidBody->Serialize(componentsNode);
	}

	void GameObject::Deserialize(SerializationNode& gameObjectNode)
	{
		Deserialize(gameObjectNode, false);
	}

	void GameObject::DeserializeAsPrefab(SerializationNode& gameObjectNode, std::map<GUID, GUID>& remap)
	{
		PropertiesComponent& properties = GetComponent<PropertiesComponent>();
		gameObjectNode.ReadData("Name", properties.Name);
		gameObjectNode.ReadData("GUID", properties.GUID.Ref());

		// Remap the guid to the new instance
		if (remap.contains(properties.GUID))
			properties.GUID = remap[properties.GUID];

		SerializationNode componentsNode = gameObjectNode.GetNode("Components");
		assert(componentsNode.IsSequence());

		for (size_t i = 0; i < componentsNode.ChildCount(); ++i)
		{
			SerializationNode componentNode = componentsNode.GetChild(i);
			assert(componentNode.IsMap());

			std::string componentType;
			componentNode.ReadData("Type", componentType);

			if (componentType == Animator::Type)
				AddComponent<Animator>(componentNode);
			else if (componentType == Camera::Type)
				AddComponent<Camera>(componentNode);
			else if (componentType == MeshRenderer::Type)
				AddComponent<MeshRenderer>(componentNode);
			else if (componentType == Transform::Type)
				AddComponent<Transform>();
			else if (componentType == ScriptComponent::Type)
				AddComponent<ScriptComponent>(componentNode, remap);
			else if (componentType == Light::Type)
				AddComponent<Light>(componentNode);
			else if (componentType == ParticleEmitter::Type)
				AddComponent<ParticleEmitter>(componentNode);
			else if (componentType == SpriteRenderer::Type)
				AddComponent<SpriteRenderer>(componentNode);
			else if (componentType == CharacterController::Type)
				AddComponent<CharacterController>(componentNode);
			else if (componentType == BoxCollider::Type)
				AddComponent<BoxCollider>(componentNode);
			else if (componentType == CapsuleCollider::Type)
				AddComponent<CapsuleCollider>(componentNode);
			else if (componentType == SphereCollider::Type)
				AddComponent<SphereCollider>(componentNode);
			else if (componentType == RigidBody::Type)
				AddComponent<RigidBody>(componentNode);
			else if (componentType == FluidBody::Type)
				AddComponent<FluidBody>(componentNode);
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

	std::vector<GameObject> GameObject::GetAllChildren()
	{
		return m_Scene->GetSceneGraph().GetAllChildren(*this);
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

		m_Entity = entt::entity();
		m_Scene = nullptr;
	}

	void GameObject::Serialize(SerializationNode& gameObjectNode, bool prefab)
	{
		PropertiesComponent& properties = GetComponent<PropertiesComponent>();

		gameObjectNode.WriteData("Name", properties.Name);
		if (!prefab)
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

		if (CharacterController* characterController = TryGetComponent<CharacterController>())
			characterController->Serialize(componentsNode);

		if (BoxCollider* boxCollider = TryGetComponent<BoxCollider>())
			boxCollider->Serialize(componentsNode);

		if (CapsuleCollider* capsuleCollider = TryGetComponent<CapsuleCollider>())
			capsuleCollider->Serialize(componentsNode);

		if (SphereCollider* sphereCollider = TryGetComponent<SphereCollider>())
			sphereCollider->Serialize(componentsNode);

		if (RigidBody* rigidBody = TryGetComponent<RigidBody>())
			rigidBody->Serialize(componentsNode);

		if (FluidBody* fluidBody = TryGetComponent<FluidBody>())
			fluidBody->Serialize(componentsNode);
	}

	void GameObject::Deserialize(SerializationNode& gameObjectNode, bool prefab)
	{
		PropertiesComponent& properties = GetComponent<PropertiesComponent>();
		gameObjectNode.ReadData("Name", properties.Name);
		if (!prefab)
			gameObjectNode.ReadData("GUID", properties.GUID.Ref());

		SerializationNode componentsNode = gameObjectNode.GetNode("Components");
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
			else if (componentType == CharacterController::Type)
			{
				CharacterController& characterController = AddComponent<CharacterController>();
				characterController.Deserialize(componentNode);
			}
			else if (componentType == BoxCollider::Type)
			{
				BoxCollider& boxCollider = AddComponent<BoxCollider>();
				boxCollider.Deserialize(componentNode);
			}
			else if (componentType == CapsuleCollider::Type)
			{
				CapsuleCollider& capsuleCollider = AddComponent<CapsuleCollider>();
				capsuleCollider.Deserialize(componentNode);
			}
			else if (componentType == SphereCollider::Type)
			{
				SphereCollider& sphereCollider = AddComponent<SphereCollider>();
				sphereCollider.Deserialize(componentNode);
			}
			else if (componentType == RigidBody::Type)
			{
				RigidBody& rigidBody = AddComponent<RigidBody>();
				rigidBody.Deserialize(componentNode);
			}
			else if (componentType == FluidBody::Type)
			{
				FluidBody& fluidBody = AddComponent<FluidBody>();
				fluidBody.Deserialize(componentNode);
			}
		}
	}
}