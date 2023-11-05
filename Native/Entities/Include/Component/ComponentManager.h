#pragma once
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <queue>
#include <memory>
#include "ComponentArray.h"
#include "GameObject.h"

#include "Transform.h"

namespace Odyssey::Entities
{
	class ComponentManager
	{
	public: // GameObject functions
		static void ExecuteOnGameObjectComponents(const GameObject& gameObject, std::function<void(Component*)> func);
		static void RemoveGameObject(const GameObject& gameObject);

	public: // Class type functions
		template<typename... Args>
		static Component* AddComponentByName(const GameObject& gameObject, const std::string& fqName, Args&&... params)
		{
			if (fqName == Transform::Type)
			{
				return AddComponent<Transform>(gameObject, params...);
			}
			return nullptr;
		}

	public: // Templated component functions
		template<typename T, typename... Args>
		static T* AddComponent(const GameObject& gameObject, Args&&... params)
		{
			std::type_index typeID = typeid(T);
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			unsigned int index = componentArray->InsertData(gameObject.id, params...);

			if (index != -1)
			{
				std::pair indexPair = std::make_pair(typeID, index);
				gameObjectToComponentArrayIndex[gameObject.id].push_back(indexPair);
			}

			return componentArray->GetComponentData(gameObject.id);
		}

		template<typename T>
		static void RemoveComponent(const GameObject& gameObject)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			if (componentArray)
			{
				componentArray->RemoveGameObject(gameObject.id);
			}

			std::type_index typeID = typeid(T);

			for (int i = 0; i < gameObjectToComponentArrayIndex[gameObject.id].size(); ++i)
			{
				auto& pair = gameObjectToComponentArrayIndex[gameObject.id][i];
				
				if (pair.first == typeID)
				{
					gameObjectToComponentArrayIndex[gameObject.id].erase(gameObjectToComponentArrayIndex[gameObject.id].begin() + i);
					
					if (gameObjectToComponentArrayIndex[gameObject.id].size() == 0)
					{
						gameObjectToComponentArrayIndex.erase(gameObject.id);
					}
					
					break;
				}
			}
		}

		template<typename T>
		static T* GetComponent(const GameObject& gameObject)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			return componentArray->GetComponentData(gameObject.id);
		}

	private: // Templated helpers
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

	private:
		static std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> componentArrays;
		static std::unordered_map<unsigned int, std::vector<std::pair<std::type_index, unsigned int>>> gameObjectToComponentArrayIndex;
	};
}