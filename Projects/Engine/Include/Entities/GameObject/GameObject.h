#pragma once
#include "Globals.h"
#include "Asset.h"
#include "AssetSerializer.h"
#include "entt.hpp"

namespace Odyssey
{
	class Scene;

	class GameObject
	{
	public:
		CLASS_DECLARATION(Odyssey, GameObject)

	public:
		GameObject() = default;
		GameObject(Scene* scene, entt::entity entity);
		GameObject(Scene* scene, uint32_t id);

	public:
		operator entt::entity() const { return m_Entity; }
		bool Equals(const GameObject& other) { return m_Entity == other.m_Entity && m_Scene == other.m_Scene; }
		bool IsValid();

	public:
		void Awake();
		void OnCollisionEnter(GameObject& body, float3 contactNormal);
		void OnCollisionStay(GameObject& body, float3 contactNormal);
		void OnCollisionExit(GameObject& body);

	public:
		void Serialize(SerializationNode& gameObjectNode);
		void SerializeAsPrefab(SerializationNode& gameObjectNode, std::map<GUID, GUID>& remap);
		void Deserialize(SerializationNode& gameObjectNode);
		void DeserializeAsPrefab(SerializationNode& gameObjectNode, std::map<GUID, GUID>& remap);

	public:
		void SetParent(const GameObject& parent);
		void RemoveParent();
		GameObject GetParent();
		std::vector<GameObject> GetChildren();
		std::vector<GameObject> GetAllChildren();
		Scene* GetScene() { return m_Scene; }

	public:
		const std::string& GetName();
		GUID GetGUID();
		void SetName(std::string_view name);
		void SetGUID(GUID guid);

	public:
		void Destroy();

	private:
		void Serialize(SerializationNode& gameObjectNode, bool prefab);
		void Deserialize(SerializationNode& gameObjectNode, bool prefab);

	public:
		template<typename T, typename... Args>
		T& AddComponent(Args&&... params);

		template<typename T>
		T& GetComponent();

		template<typename T>
		const T& GetComponent() const;

		template<typename T>
		T* TryGetComponent();

		template<typename T>
		const T* TryGetComponent() const;

		template<typename... T>
		bool HasComponent();

		template<typename T>
		bool RemoveComponent();

	private: // Non-serialized
		entt::entity m_Entity;
		Scene* m_Scene = nullptr;
	};
}