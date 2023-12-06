#pragma once
#include "ComponentArray.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "UserScript.h"
#include <typeindex>
#include <string>

namespace Odyssey
{
	class ComponentManager
	{
	public: // Public templates
		template<typename... Args>
		static Component* AddComponentByName(const uint32_t id, const std::string& fqName, Args&&... params)
		{
			if (fqName == Transform::Type)
			{
				return AddComponent<Transform>(id, params...);
			}
			else if (fqName == Camera::Type)
			{
				return AddComponent<Camera>(id, params...);
			}
			else if (fqName == MeshRenderer::Type)
			{
				return AddComponent<MeshRenderer>(id, params...);
			}
			else if (fqName == UserScript::Type)
			{
				return AddUserScript(id, fqName, params...);
			}
			return nullptr;
		}

		template<typename T, typename... Args>
		static T* AddComponent(const uint32_t id, Args&&... params)
		{
			std::type_index typeID = typeid(T);
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			unsigned int index = componentArray->InsertData(id, params...);

			if (index != -1)
			{
				std::pair indexPair = std::make_pair(typeID, index);
				gameObjectToComponentArrayIndex[id].push_back(indexPair);
			}

			return componentArray->GetComponentData(id);
		}

		template<typename... Args>
		static UserScript* AddUserScript(const uint32_t id, const std::string& managedName, Args&&... params)
		{
			ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName);
			unsigned int index = userScriptArray->InsertData(id, params...);
			if (index != -1)
			{
				gameObjectToUserScriptIndex[id].push_back(std::make_pair(managedName, index));
			}

			UserScript* userScript = userScriptArray->GetComponentData(id);
			if (userScript)
			{
				userScript->SetManagedType(managedName);
			}

			return userScript;
		}

		template<typename T>
		static void RemoveComponent(const uint32_t id)
		{
			if (ComponentArray<T>* componentArray = GetComponentArray<T>())
			{
				componentArray->RemoveGameObject(id);
			}

			std::type_index typeID = typeid(T);

			for (int i = 0; i < gameObjectToComponentArrayIndex[id].size(); ++i)
			{
				auto& pair = gameObjectToComponentArrayIndex[id][i];

				if (pair.first == typeID)
				{
					gameObjectToComponentArrayIndex[id].erase(gameObjectToComponentArrayIndex[id].begin() + i);

					if (gameObjectToComponentArrayIndex[id].size() == 0)
					{
						gameObjectToComponentArrayIndex.erase(id);
					}

					break;
				}
			}
		}

		template<typename T>
		static T* GetComponent(const uint32_t id)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			return componentArray->GetComponentData(id);
		}

		template<typename T>
		static bool HasComponent(const uint32_t id)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			return componentArray->HasComponent(id);
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
		static void RemoveUserScript(const uint32_t id, const std::string& managedName);
		static UserScript* GetUserScript(const uint32_t id, const std::string& managedName);
		static std::vector<std::pair<std::string, UserScript*>> GetAllUserScripts(const uint32_t id);

		static bool HasUserScript(const uint32_t id, const std::string& managedName);

		static void ExecuteOnGameObjectComponents(const uint32_t id, std::function<void(Component*)> func);
		static void RemoveGameObject(const uint32_t id);

	private:
		static ComponentArray<UserScript>* GetUserScriptArray(const std::string& managedName);

	private:
		inline static std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> componentArrays;
		inline static std::unordered_map<unsigned int, std::vector<std::pair<std::type_index, unsigned int>>> gameObjectToComponentArrayIndex;

		inline static std::unordered_map<std::string, std::unique_ptr<ComponentArray<UserScript>>> userScriptArrays;
		inline static std::unordered_map<unsigned int, std::vector<std::pair<std::string, unsigned int>>> gameObjectToUserScriptIndex;
	};
}