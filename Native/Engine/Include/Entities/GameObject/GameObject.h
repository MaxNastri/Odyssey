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
		GameObject();
		GameObject(Scene* scene, entt::entity entity);
		GameObject(Scene* scene, uint32_t id);

	public:
		operator entt::entity() const { return m_Entity; }
		operator bool() const { return m_Scene; }

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		const std::string& GetName();
		const std::string& GetGUID();
		void SetName(const std::string& name);
		void SetGUID(const std::string& guid);

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
		void RemoveComponent();

	public:
		CLASS_DECLARATION(GameObject);

	private: // Non-serialized
		entt::entity m_Entity;
		Scene* m_Scene = nullptr;
		bool m_IsHidden = false;
	};
}