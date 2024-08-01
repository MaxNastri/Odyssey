#pragma once
#include "Globals.h"
#include "Asset.h"
#include "Scene.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	class GameObject
	{
	public:
		GameObject();
		GameObject(Scene* m_Scene, int32_t ID);

	public:
		bool operator==(const GameObject& other) { return id == other.id; }
		operator bool() { return id != -1; }

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		std::vector<UserScript*> GetUserScripts();
		UserScript* GetUserScript(const std::string& managedName);

	public:
		template<typename T, typename... Args>
		T* AddComponent(Args&&... params)
		{
			auto component = m_Scene->GetComponentRegistry()->AddComponent<T>(id, params...);
			component->SetGameObject(this);
			return component;
		}

		template<typename... Args>
		UserScript* AddUserScript(const std::string& managedName, Args&&... params)
		{
			auto userScript = m_Scene->GetComponentRegistry()->AddUserScript(id, managedName, params...);
			userScript->SetGameObject(this);
			return userScript;
		}

		template<typename T>
		void RemoveComponent()
		{
			m_Scene->GetComponentRegistry()->RemoveComponent<T>(id);
		}

		template<typename T>
		T* GetComponent()
		{
			return m_Scene->GetComponentRegistry()->GetComponent<T>(id);
		}
		
		template<typename T>
		bool HasComponent()
		{
			return m_Scene->GetComponentRegistry()->HasComponent<T>(id);
		}

	public:
		CLASS_DECLARATION(GameObject);

	public: // Serialized
		bool active = true;
		int32_t id = -1;
		std::string name;
		bool m_IsHidden = false;

	private: // Non-serialized
		Scene* m_Scene;
	};
}