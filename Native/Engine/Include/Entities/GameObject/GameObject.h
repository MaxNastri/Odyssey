#pragma once
#include "Asset.h"
#include "Globals.h"
#include "ComponentManager.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	class GameObject
	{
	public:
		GameObject();
		GameObject(int32_t ID);

	public:
		bool operator==(const GameObject& other) { return id == other.id; }

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		template<typename T, typename... Args>
		T* AddComponent(Args&&... params)
		{
			return ComponentManager::AddComponent<T>(id, params...);
		}

		template<typename... Args>
		UserScript* AddUserScript(const std::string& managedName, Args&&... params)
		{
			return ComponentManager::AddUserScript(id, managedName, params...);
		}

		template<typename T>
		void RemoveComponent()
		{
			ComponentManager::RemoveComponent<T>(id);
		}

		template<typename T>
		T* GetComponent()
		{
			return ComponentManager::GetComponent<T>(id);
		}
		template<typename T>
		bool HasComponent()
		{
			return ComponentManager::HasComponent<T>(id);
		}

	public:
		UserScript* GetUserScript(const std::string& managedName)
		{
			return ComponentManager::GetUserScript(id, managedName);
		}

	public:
		bool active = true;
		int32_t id = -1;
		std::string name;
		CLASS_DECLARATION(GameObject);
	};
}