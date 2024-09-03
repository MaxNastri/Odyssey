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
		GameObject() = default;
		GameObject(Scene* scene, entt::entity entity);
		GameObject(Scene* scene, uint32_t id);

	public:
		operator entt::entity() const { return m_Entity; }
		operator bool() const { return m_Scene; }
		bool Equals(const GameObject& other) { return m_Entity == other.m_Entity && m_Scene == other.m_Scene; }

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetParent(const GameObject& parent);
		void RemoveParent();
		GameObject GetParent();
		std::vector<GameObject> GetChildren();

	public:
		const std::string& GetName();
		GUID GetGUID();
		void SetName(const std::string& name);
		void SetGUID(GUID guid);

	public:
		void Destroy();

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

	public:
		CLASS_DECLARATION(GameObject);

	private: // Non-serialized
		entt::entity m_Entity;
		Scene* m_Scene = nullptr;
	};
}