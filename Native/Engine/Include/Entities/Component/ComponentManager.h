#pragma once
#include "ComponentArray.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "UserScript.h"
#include <typeindex>
#include <string>

namespace Odyssey
{
	class ComponentManager
	{
	public: // Public templates
		template<typename... Args>
		static Component* AddComponentByName(const GameObject* gameObject, const std::string& fqName, Args&&... params)
		{
			if (fqName == Transform::Type)
			{
				return AddComponent<Transform>(gameObject, params...);
			}
			else if (fqName == Camera::Type)
			{
				return AddComponent<Camera>(gameObject, params...);
			}
			else if (fqName == UserScript::Type)
			{
				return AddUserScript(gameObject, fqName, params...);
			}
			return nullptr;
		}

		template<typename T, typename... Args>
		static T* AddComponent(const GameObject* gameObject, Args&&... params)
		{
			std::type_index typeID = typeid(T);
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			unsigned int index = componentArray->InsertData(gameObject->id, params...);

			if (index != -1)
			{
				std::pair indexPair = std::make_pair(typeID, index);
				gameObjectToComponentArrayIndex[gameObject->id].push_back(indexPair);
			}

			return componentArray->GetComponentData(gameObject->id);
		}

		template<typename... Args>
		static UserScript* AddUserScript(const GameObject* gameObject, const std::string& managedName, Args&&... params)
		{
			ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName);
			unsigned int index = userScriptArray->InsertData(gameObject->id, params...);
			if (index != -1)
			{
				gameObjectToUserScriptIndex[gameObject->id].push_back(std::make_pair(managedName, index));
			}

			UserScript* userScript = userScriptArray->GetComponentData(gameObject->id);
			if (userScript)
			{
				userScript->SetManagedType(managedName);
			}

			return userScript;
		}

		template<typename T>
		static void RemoveComponent(const GameObject* gameObject)
		{
			if (ComponentArray<T>* componentArray = GetComponentArray<T>())
			{
				componentArray->RemoveGameObject(gameObject->id);
			}

			std::type_index typeID = typeid(T);

			for (int i = 0; i < gameObjectToComponentArrayIndex[gameObject->id].size(); ++i)
			{
				auto& pair = gameObjectToComponentArrayIndex[gameObject->id][i];

				if (pair.first == typeID)
				{
					gameObjectToComponentArrayIndex[gameObject->id].erase(gameObjectToComponentArrayIndex[gameObject->id].begin() + i);

					if (gameObjectToComponentArrayIndex[gameObject->id].size() == 0)
					{
						gameObjectToComponentArrayIndex.erase(gameObject->id);
					}

					break;
				}
			}
		}

		template<typename T>
		static T* GetComponent(const GameObject* gameObject)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			return componentArray->GetComponentData(gameObject->id);
		}

		template<typename T>
		static bool HasComponent(const GameObject* gameObject)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			return componentArray->HasComponent(gameObject->id);
		}

	private: // Private templates
		template<typename T>
		static ComponentArray<T>* GetComponentArray()
		{
			std::type_index typeIndex = typeid(T);
			if (componentArrays.find(typeIndex) == componentArrays.end())
			{
				componentArrays[typeIndex] = std::make_unique<ComponentArray<T>>();
			}
			return static_cast<ComponentArray<T>*>(componentArrays[typeIndex].get());
		}

	public:
		static void RemoveUserScript(const GameObject* gameObject, const std::string& managedName);
		static UserScript* GetUserScript(const GameObject* gameObject, const std::string& managedName);
		static std::vector<std::pair<std::string, UserScript*>> GetAllUserScripts(const GameObject* gameObject);

		static bool HasUserScript(const GameObject* gameObject, const std::string& managedName);

		static void ExecuteOnGameObjectComponents(const GameObject* gameObject, std::function<void(Component*)> func);
		static void RemoveGameObject(const GameObject* gameObject);

	private:
		static ComponentArray<UserScript>* GetUserScriptArray(const std::string& managedName);

	private:
		inline static std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> componentArrays;
		inline static std::unordered_map<unsigned int, std::vector<std::pair<std::type_index, unsigned int>>> gameObjectToComponentArrayIndex;

		inline static std::unordered_map<std::string, std::unique_ptr<ComponentArray<UserScript>>> userScriptArrays;
		inline static std::unordered_map<unsigned int, std::vector<std::pair<std::string, unsigned int>>> gameObjectToUserScriptIndex;
	};
}